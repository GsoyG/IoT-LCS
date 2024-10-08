<template>
  <v-container>
    <!-- 加载信息 -->
    <div class="d-flex flex-column fill-height justify-center align-center" v-if="deviceList.length === 0">
      <h1 class="text-h4 font-weight-thin my-8">{{ emptyInfo.heading }}</h1>
      <h3 class="subheading">{{ emptyInfo.subheading }}</h3>
      <v-progress-circular color="indigo" indeterminate size="70" width="7"
        v-if="emptyInfo.subheading === ''"></v-progress-circular>
    </div>
    <v-row>
      <v-col v-for="device in deviceList" cols="12" sm="6" md="4">

        <v-card outlined>
          <v-card-title>{{ device.Device }}</v-card-title>

          <div class="mx-4">
            <!-- 设备开关按钮 -->
            <v-row justify="center">
              <v-col cols="auto">
                <v-btn :prepend-icon="device.Reachable && device.Power ? 'mdi-lightbulb' : 'mdi-lightbulb-off'"
                  :color="device.Reachable && device.Power ? '#' + device.RGB : ''" variant="tonal" rounded="lg"
                  @click="switchDevicePower(device)" :disabled="disabledEdit || !device.Reachable" stacked>
                  {{ getStatusText(device) }}
                </v-btn>
              </v-col>
            </v-row>

            <!-- 设备颜色亮度、温湿度 -->
            <div class="d-flex justify-space-between align-center">
              <v-slider min="1" max="254" step="1" v-model="device.Dimmer" label="亮度" :disabled="disabledEdit"
                @end="updateDeviceState(device, 'Dimmer', device.Dimmer)" :color="`#${device.RGB}`" hide-details
                class="mr-5"></v-slider>
              <v-switch :label="device.DimmerAuto ? '自动' : '手动'" hide-details v-model="device.DimmerAuto"
                :disabled="device.ModelId != 'SL0001'" @update:model-value="setDimmerAutoEnable(device)"></v-switch>
            </div>
            <div class="d-flex justify-space-between align-center">
              <span><v-icon icon="mdi-thermometer"></v-icon>温度：{{ device.Temperature }} ℃</span>
              <span><v-icon icon="mdi-water-percent"></v-icon>湿度：{{ device.Humidity }} ％</span>
            </div>
          </div>

          <v-card-actions>
            <v-spacer></v-spacer>
            <!-- 编辑对话框及按钮 -->
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" color="indigo-accent-2" icon="mdi-square-edit-outline"
                  :disabled="!device.Reachable" @click="parseDeviceConfig(device)"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="设备编辑">
                  <div class="mx-4">
                    <div class="d-flex justify-center my-5">
                      <v-col cols="auto"
                        :style="`width: 100px; height: 100px; background: #${deviceConfig.RGB}; border-radius: 20px;`"></v-col>
                    </div>
                    <div class="d-flex justify-space-between align-center">
                      <v-slider min="1" max="254" step="1" v-model="deviceConfig.Dimmer" label="亮度"
                        :disabled="disabledEdit" @end="updateDeviceState(device, 'Dimmer', deviceConfig.Dimmer)"
                        hide-details class="mb-2 mr-5"></v-slider>
                      <v-switch :label="device.DimmerAuto ? '自动' : '手动'" hide-details v-model="device.DimmerAuto"
                        :disabled="device.ModelId != 'SL0001'" class="mb-2"
                        @update:model-value="setDimmerAutoEnable(device)"></v-switch>
                    </div>
                    <v-slider min="0" max="254" step="1" v-model="deviceConfig.Hue" label="色调" :disabled="disabledEdit"
                      @end="updateDeviceColor(device, 'Hue', deviceConfig.Hue)" hide-details
                      class="mb-5 mr-5"></v-slider>
                    <v-slider min="0" max="254" step="1" v-model="deviceConfig.Sat" label="饱和" :disabled="disabledEdit"
                      @end="updateDeviceColor(device, 'Sat', deviceConfig.Sat)" hide-details
                      class="mb-5 mr-5"></v-slider>
                    <v-slider min="0" max="500" step="1" v-model="deviceConfig.CT" label="色温" :disabled="disabledEdit"
                      @end="updateDeviceColor(device, 'CT', deviceConfig.CT)" hide-details class="mb-5 mr-5"></v-slider>
                  </div>

                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn text="关闭" @click="isActive.value = false"></v-btn>
                  </v-card-actions>
                </v-card>
              </template>
            </v-dialog>
          </v-card-actions>

        </v-card>
      </v-col>
    </v-row>

    <!-- 添加按钮 -->
    <v-btn icon="mdi-plus" size="large" color="indigo" style="position: fixed; bottom: 40px; right: 50px;"></v-btn>

    <!-- 提示消息条 -->
    <v-snackbar timeout="2000" v-model="snackbarConfig.show" color="indigo">
      <v-icon :icon="snackbarConfig.icon" :color="snackbarConfig.iconColor" class="mr-2"></v-icon>
      {{ snackbarConfig.text }}
    </v-snackbar>
  </v-container>
</template>

<script setup>
import axios from 'axios'
import { ref, onMounted } from 'vue'

const deviceList = ref([])
const deviceConfig = ref({})
const disabledEdit = ref(false)
const emptyInfo = ref({
  'heading': '获取设备列表中......',
  'subheading': ''
})
const snackbarConfig = ref({
  'show': false,
  'text': '',
  'icon': '',
  'iconColor': '',
})

onMounted(() => {
  fetchDeviceList()
})

// 得到设备状态文字
function getStatusText(device) {
  if (!device.Reachable) return '已离线'
  if (device.Power) return '已开启'
  return '已关闭'
}
// 显示提示消息条
function showMessage(text, type) {
  switch (type) {
    case 'success':
      snackbarConfig.value = { show: true, text: text, icon: 'mdi-check-circle', iconColor: 'success' }
      break
    case 'warning':
      snackbarConfig.value = { show: true, text: text, icon: 'mdi-alert-circle', iconColor: 'warning' }
      break
    default:
      break
  }
}

// 获取设备列表
async function fetchDeviceList() {
  disabledEdit.value = true
  await axios.get('/api/lighting/devices', { timeout: 4000 }).then(response => {
    deviceList.value = response.data

    // 检查设备照明模式
    deviceList.value.forEach(device => {
      if (device.ColorMode == 2) // 更改真实颜色
        device.RGB = hslToRgb(0.083, 1, 1 - device.CT / 500 * 0.3)
      if (device.ModelId != 'SL0001') { // 非SL0001设备无温湿度
        device.Humidity = '- -'
        device.Temperature = '- -'
      }
      if (device.Y == 1000) device.DimmerAuto = false
      if (device.Y == 1001) device.DimmerAuto = true
    })
    if (deviceList.value.length === 0)
      emptyInfo.value = {
        'heading': '设备列表为空',
        'subheading': '未绑定设备，请点击右下角按钮添加'
      }
  }).catch(error => {
    if (error.response)
      emptyInfo.value = {
        'heading': '获取设备列表失败',
        'subheading': '请稍后再试，错误信息：' + error.response.data
      }
    else
      emptyInfo.value = {
        'heading': '网关连接出错',
        'subheading': '请检查网络连接或网关设备，错误信息：' + error.message
      }
  })
  disabledEdit.value = false
}

// 更新设备状态信息
async function updateDeviceState(device, key, value, disabled = true) {
  const state = { [key]: value }
  let result = false

  if (disabled) disabledEdit.value = true
  await axios.get('/api/lighting/setState', {
    params: {
      device: device.Device,
      state: JSON.stringify(state),
    },
  }).then(response => {
    Object.assign(device, state)
    // 检查是否切换电源状态
    if (key === 'Dimmer') {
      if (value < 2) device.Power = 0
      if (value > 1) device.Power = 1
    }
    result = true
  }).catch(error => {
    if (error.response)
      showMessage('设置设备配置失败：' + error.response.data, 'warning')
    else showMessage('设置设备配置出错：' + error.message, 'warning')
  })
  if (disabled) disabledEdit.value = false
  return result
}

// 颜色转换辅助方法
function hueToRgb(p, q, t) {
  if (t < 0) t += 1
  if (t > 1) t -= 1
  if (t < 1 / 6) return p + (q - p) * 6 * t
  if (t < 1 / 2) return q
  if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6
  return p
}
function hslToRgb(h, s, l) {
  let r, g, b
  const q = l < 0.5 ? l * (1 + s) : l + s - l * s
  const p = 2 * l - q
  r = Math.round(hueToRgb(p, q, h + 1 / 3) * 255)
  g = Math.round(hueToRgb(p, q, h) * 255)
  b = Math.round(hueToRgb(p, q, h - 1 / 3) * 255)
  return ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1)
}

// 更新设备颜色信息
async function updateDeviceColor(device, key, value) {
  let rgb = 'FFFFFF'
  if (key != 'CT') // 颜色模式为彩光模式
    rgb = hslToRgb(deviceConfig.value.Hue / 254, 1, 1 - deviceConfig.value.Sat / 254 / 2)
  else rgb = hslToRgb(0.083, 1, 1 - value / 500 * 0.3) // 颜色模式为白光模式

  deviceConfig.value.RGB = rgb
  if (await updateDeviceState(device, key, value))
    device.RGB = rgb
}

// 切换设备开关状态
async function switchDevicePower(device) {
  const power = !device.Power
  const text = power ? '开启' : '关闭'
  disabledEdit.value = true

  // 切换后等待获取到设备最新状态后，获取最新状态列表
  if (await updateDeviceState(device, 'Power', power, false)) {
    showMessage(text + '设备成功', 'success')
    await new Promise(resolve => setTimeout(resolve, 2000))
    fetchDeviceList()
  }
  disabledEdit.value = false
}

// 切换亮度自动模式
async function setDimmerAutoEnable(device) {
  if (device.DimmerAuto) updateDeviceState(device, 'CT', 1001)
  else updateDeviceState(device, 'CT', 1000)
}

// 打开设备编辑对话框，解析设备配置
function parseDeviceConfig(device) {
  deviceConfig.value = {
    'Dimmer': device.Dimmer,
    'RGB': device.RGB,
    'Hue': device.Hue,
    'Sat': device.Sat,
    'X': device.X,
    'Y': device.Y,
    'CT': device.CT,
    'ColorMode': device.ColorMode,
    'RGBb': device.RGBb,
  }
}
</script>
