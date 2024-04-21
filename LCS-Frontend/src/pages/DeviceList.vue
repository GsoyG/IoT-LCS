<template>
  <v-container>
    <div class="d-flex flex-column fill-height justify-center align-center" v-if="deviceList.length === 0">
      <h1 class="text-h4 font-weight-thin my-8">{{ emptyInfo.heading }}</h1>
      <h3 class="subheading">{{ emptyInfo.subheading }}</h3>
      <v-progress-circular color="indigo" indeterminate size="70" width="7"
        v-if="emptyInfo.subheading === ''"></v-progress-circular>
    </div>
    <v-row dense>
      <v-col v-for="device in deviceList" cols="12" sm="6" md="4">
        <v-card outlined>
          <v-card-title>{{ device.Device }}</v-card-title>

          <div class="mx-4">
            <!-- 设备开关按钮 -->
            <v-row class="pb-8" justify="center">
              <v-col cols="auto">
                <v-btn :prepend-icon="device.Reachable && device.Dimmer > 1 ? 'mdi-lightbulb' : 'mdi-lightbulb-off'"
                  :color="device.Reachable && device.Dimmer > 1 ? '#' + device.RGB : ''" variant="tonal" rounded="lg"
                  @click="switchDevicePower(device)" :disabled="disabledEdit || !device.Reachable" stacked>
                  {{ getStatusText(device) }}
                </v-btn>
              </v-col>
            </v-row>

            <!-- 设备颜色亮度条 -->
            <v-progress-linear :model-value="device.Dimmer / 254 * 100" :color="`#${device.RGB}`" height="20"
              rounded></v-progress-linear>
          </div>

          <v-card-actions>
            <!-- 设备编辑对话框 -->
            <v-spacer></v-spacer>
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" color="indigo-accent-2" icon="mdi-square-edit-outline"
                  :disabled="!device.Reachable" @click="openEditDialog(device)"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="设备编辑">
                  <div class="mx-4">
                    <div class="d-flex justify-center my-2">
                      <v-col cols="auto"
                        :style="`width: 100px; height: 100px; background: #${deviceConfig.RGB}; border-radius: 20px;`"></v-col>
                    </div>
                    <v-slider min="1" max="254" step="1" v-model="deviceConfig.Dimmer" label="亮度"
                      :disabled="disabledEdit"
                      @end="updateDeviceState(device, 'Dimmer', deviceConfig.Dimmer)"></v-slider>
                    <v-slider min="0" max="254" step="1" v-model="deviceConfig.Hue" label="色调" :disabled="disabledEdit"
                      @end="updateDeviceColor(device, 'Hue', deviceConfig.Hue)"></v-slider>
                    <v-slider min="0" max="254" step="1" v-model="deviceConfig.Sat" label="饱和" :disabled="disabledEdit"
                      @end="updateDeviceColor(device, 'Sat', deviceConfig.Sat)"></v-slider>
                    <v-slider min="0" max="500" step="1" v-model="deviceConfig.CT" label="色温" :disabled="disabledEdit"
                      @end="updateDeviceColor(device, 'CT', deviceConfig.CT)"></v-slider>
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

    <v-btn icon="mdi-plus" size="large" color="indigo" style="position: fixed; bottom: 40px; right: 50px;"></v-btn>
  </v-container>
</template>

<script setup>
import axios from 'axios'
import { ref, onMounted } from 'vue'

const deviceList = ref([])
const deviceConfig = ref({})
const disabledEdit = ref(false)
const emptyInfo = ref({
  'heading': '查询设备中......',
  'subheading': ''
})

onMounted(() => {
  fetchDeviceList()
})

// 得到设备状态文字
function getStatusText(device) {
  if (!device.Reachable) {
    return '已离线'
  }
  if (device.Dimmer > 1) {
    return '已开启'
  }
  return '已关闭'
}

// 获取设备列表
async function fetchDeviceList() {
  disabledEdit.value = true
  try {
    const response = await axios.get('/api/lighting/devices', { timeout: 4000 })
    if (response.status === 200) {
      deviceList.value = response.data

      // 检查设备照明模式
      deviceList.value.forEach(device => {
        if (device.ColorMode == 2) {
          device.RGB = hslToRgb(0.083, 1, 1 - device.CT / 500 * 0.3)
        }
      })
      if (deviceList.value.length === 0) {
        emptyInfo.value = {
          'heading': '未查询到设备',
          'subheading': '未绑定设备，请点击右下角添加'
        }
      }
    } else {
      console.error('Failed to fetch device list:', response.statusText)
      emptyInfo.value = {
        'heading': '未查询到设备',
        'subheading': '网关连接出错，请检查网关设备'
      }
    }
  } catch (error) {
    console.error('Error fetching device list:', error)
    emptyInfo.value = {
      'heading': '未查询到设备',
      'subheading': '网关连接出错，请检查网关设备或网络连接'
    }
  }
  disabledEdit.value = false
}

// 更新设备状态信息
async function updateDeviceState(device, key, value, disabled = true) {
  var state = {}
  state[key] = value

  if (disabled) disabledEdit.value = true
  try {
    const response = await axios.get('/api/lighting/setState', {
      params: {
        device: device.Device,
        state: JSON.stringify(state),
      },
    })
    if (response.status === 200) {
      Object.assign(device, state)

      if (disabled) disabledEdit.value = false
      return true
    } else {
      console.error('Failed to upload device state:', response.statusText)
    }
  } catch (error) {
    console.error('Error uploading device state:', error)
  }

  if (disabled) disabledEdit.value = false
  return false
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
  if (key != 'CT') {
    rgb = hslToRgb(deviceConfig.value.Hue / 254, 1, 1 - deviceConfig.value.Sat / 254 / 2)
  }
  else rgb = hslToRgb(0.083, 1, 1 - value / 500 * 0.3)

  deviceConfig.value.RGB = rgb
  if (await updateDeviceState(device, key, value)) {
    device.RGB = rgb
  }
}

// 切换设备开关状态
async function switchDevicePower(device) {
  var power = device.Dimmer > 1 ? 0 : 1
  disabledEdit.value = true

  // 切换后等待获取到设备最新状态后，获取最新状态列表
  if (await updateDeviceState(device, 'Power', power, false)) {
    await new Promise(resolve => setTimeout(resolve, 2000))
    fetchDeviceList()
  }
  disabledEdit.value = false
}

// 打开设备编辑对话框
function openEditDialog(device) {
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
