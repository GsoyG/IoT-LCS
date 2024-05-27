<template>
  <v-container>
    <!-- 加载信息 -->
    <div class="d-flex flex-column fill-height justify-center align-center" v-if="modeList.length === 0">
      <h1 class="text-h4 font-weight-thin my-8">{{ emptyInfo.heading }}</h1>
      <h3 class="subheading">{{ emptyInfo.subheading }}</h3>
      <v-progress-circular color="indigo" indeterminate size="70" width="7"
        v-if="emptyInfo.subheading === ''"></v-progress-circular>
    </div>
    <v-row>
      <v-col v-for="mode in modeList" cols="12" sm="6" md="4">

        <v-card outlined max-height="400">
          <v-card-title :style="`transition-duration: 0.2s;${mode.enable ? 'background-color: #3f51b5;' : ''}`">
            {{ mode.name }}</v-card-title>

          <!-- 显示面板 -->
          <div class="mx-4 mt-4">
            <div class="text-truncate">
              <span class="text-h6" v-if="mode.devices.length === 0">未绑定设备</span>
              <span class="text-h6" v-for="device in mode.devices">{{ device + '、' }}</span>
            </div>
            <v-divider class="my-2"></v-divider>
            <div class="d-flex justify-space-between align-center">
              <span class="text-h5">{{ mode.enable ? '已启用' : '已关闭' }}</span>
              <v-btn class="my-2" text="切换" color="indigo" @click="switchModeRequest(mode.name)"></v-btn>
            </div>
            <p>操作：{{ parseActionText(mode.action) }}</p>
          </div>

          <v-card-actions>
            <!-- 删除按钮 -->
            <v-btn color="red" icon="mdi-delete-outline"
              @click="deleteDialog.show = true; deleteDialog.modeName = mode.name;"></v-btn>

            <v-spacer></v-spacer>
            <!-- 编辑对话框及按钮 -->
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" color="indigo-accent-2" icon="mdi-square-edit-outline"
                  @click="editMode(mode)"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="编辑场景模式">
                  <div class="mx-4 mt-4">
                    <v-text-field label="名称" v-model="modeConfig.name" disabled></v-text-field>
                    <v-select v-model="modeConfig.devices" :items="deviceList" label="设备" multiple
                      no-data-text="无可用设备"></v-select>
                    <v-divider class="mb-4"></v-divider>
                    <v-row dense>
                      <v-col>
                        <v-select v-model="modeConfig.action.key" :items="['电源', '亮度', '色调', '饱和', '色温']"
                          label="操作"></v-select>
                      </v-col>
                      <v-col>
                        <v-select v-model="modeConfig.action.value" :items="['开', '关']" label="结果"
                          v-if="modeConfig.action.key === '电源'"></v-select>
                        <v-text-field v-model="modeConfig.action.value" type="number" max="500" min="1" label="结果"
                          v-else></v-text-field>
                      </v-col>
                    </v-row>
                  </div>

                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn text="保存" color="indigo-accent-2" @click="updateMode(false); isActive.value = false"></v-btn>
                    <v-btn text="关闭" @click="isActive.value = false"></v-btn>
                  </v-card-actions>
                </v-card>
              </template>
            </v-dialog>
          </v-card-actions>

        </v-card>
      </v-col>
    </v-row>

    <!-- 删除对话框 -->
    <v-dialog max-width="400" v-model="deleteDialog.show">
      <v-card title="删除场景模式" text="确定删除此场景模式吗？此操作将不可恢复。">
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn text="确认" color="red" @click="deleteMode(deleteDialog.modeName); deleteDialog.show = false"></v-btn>
          <v-btn text="取消" @click="deleteDialog.show = false"></v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>

    <!-- 添加对话框及按钮 -->
    <v-dialog max-width="500">
      <template v-slot:activator="{ props: activatorProps }">
        <v-btn v-bind="activatorProps" icon="mdi-plus" size="large" color="indigo"
          style="position: fixed; bottom: 40px; right: 50px;" v-show="!emptyInfo.isDisconnect"></v-btn>
      </template>

      <template v-slot:default="{ isActive }">
        <v-card title="添加场景模式">
          <div class="mx-4 mt-4">
            <v-text-field label="名称" v-model="modeConfig.name"></v-text-field>
            <v-select v-model="modeConfig.devices" :items="deviceList" label="设备" multiple
              no-data-text="无可用设备"></v-select>
            <v-divider class="mb-4"></v-divider>
            <v-row dense>
              <v-col>
                <v-select v-model="modeConfig.action.key" :items="['电源', '亮度', '色调', '饱和', '色温']" label="操作"></v-select>
              </v-col>
              <v-col>
                <v-select v-model="modeConfig.action.value" :items="['开', '关']" label="结果"
                  v-if="modeConfig.action.key === '电源'"></v-select>
                <v-text-field v-model="modeConfig.action.value" type="number" max="500" min="1" label="结果"
                  v-else></v-text-field>
              </v-col>
            </v-row>
          </div>

          <v-card-actions>
            <v-spacer></v-spacer>
            <v-btn text="添加" color="indigo-accent-2" @click="updateMode(true); isActive.value = false"></v-btn>
            <v-btn text="关闭" @click="isActive.value = false"></v-btn>
          </v-card-actions>
        </v-card>
      </template>
    </v-dialog>

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

const modeList = ref([])
const deviceList = ref([])
const modeConfig = ref({
  'name': '',
  'devices': [],
  'action': {
    'key': '',
    'value': ''
  },
})
const emptyInfo = ref({ // 加载提示信息
  'heading': '获取场景模式列表中......',
  'subheading': '',
  'isDisconnect': true
})
const snackbarConfig = ref({ // 提示消息条信息
  'show': false,
  'text': '',
  'icon': '',
  'iconColor': '',
})
const deleteDialog = ref({ // 删除对话框信息
  'show': false,
  'modeName': ''
})

onMounted(() => {
  fetchModeList()
  fetchDeviceList()
})

// 获取操作文本
function parseActionText(action) {
  const actionText = { 'Power': '电源', 'Dimmer': '亮度', 'Hue': '色调', 'Sat': '饱和', 'CT': '色温' }
  const actionType = Object.keys(action)[0]

  let text = actionText[actionType] + ' 设为 '
  if (actionType === 'Power')
    text += action[actionType] ? '开' : '关'
  else text += action[actionType]
  return text
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
  await axios.get('/api/lighting/devices', { timeout: 4000 }).then(response => {
    response.data.every(device => deviceList.value.push(device.Device))
  }).catch(error => {
    if (error.response)
      showMessage('获取设备列表失败：' + error.response.data, 'warning')
    else showMessage('获取设备列表出错：' + error.message, 'warning')
  })
}

// 获取场景模式列表
async function fetchModeList() {
  await axios.get('/api/scene/modes').then(response => {
    modeList.value = response.data

    if (modeList.value.length === 0)
      emptyInfo.value = {
        'heading': '场景模式列表为空',
        'subheading': '未添加场景模式，请点击右下角按钮添加'
      }
    emptyInfo.value.isDisconnect = false
  }).catch(error => {
    if (error.response)
      emptyInfo.value = {
        'heading': '获取场景模式列表失败',
        'subheading': '请稍后再试，错误信息：' + error.response.data
      }
    else
      emptyInfo.value = {
        'heading': '网络连接出错',
        'subheading': '请检查网络连接，错误信息：' + error.message
      }
    emptyInfo.value.isDisconnect = true
  })
}

// 发送设置模式请求，用于添加、修改、删除模式
async function setModeRequest(action, data, infoText) {
  await axios.get('/api/scene/setMode', {
    params: {
      action: action,
      data: JSON.stringify(data)
    }
  }).then(response => {
    showMessage(infoText + '模式成功', 'success')
  }).catch(error => {
    if (error.response)
      showMessage(infoText + '模式失败：' + error.response.data, 'warning')
    else showMessage(infoText + '模式出错：' + error.message, 'warning')
  })
  fetchModeList()
}

// 设置场景模式，更新或添加
async function updateMode(isAddMode) {
  // 处理场景模式配置字段
  let data = {
    'name': modeConfig.value.name,
    'devices': modeConfig.value.devices,
    'action': {}
  }
  const actionText = { '电源': 'Power', '亮度': 'Dimmer', '色调': 'Hue', '饱和': 'Sat', '色温': 'CT' }
  const actionType = modeConfig.value.action.key
  if (actionType === '电源') // 处理事件操作字段
    data.action['Power'] = modeConfig.value.action.value === '开' ? 1 : 0
  else data.action[actionText[actionType]] = modeConfig.value.action.value
  if (isAddMode) data['enable'] = false // 新建模式默认关闭

  // 发送更新请求
  await setModeRequest(isAddMode ? 'add' : 'update', data, '设置')
}

// 删除场景模式
async function deleteMode(modeName) {
  await setModeRequest('delete', { name: modeName }, '删除')
}

// 切换场景模式
async function switchModeRequest(modeName) {
  await axios.get('/api/scene/switchMode', {
    params: {
      name: modeName
    }
  }).then(response => {
    showMessage('切换模式成功', 'success')
  }).catch(error => {
    if (error.response)
      showMessage('切换模式失败：' + error.response.data, 'warning')
    else showMessage('切换模式出错：' + error.message, 'warning')
  })
  fetchModeList()
}

// 打开编辑场景模式对话框
async function editMode(config) {
  // 处理场景模式配置字段
  modeConfig.value = {
    'name': config.name,
    'devices': config.devices,
    'action': {}
  }
  const actionText = { 'Power': '电源', 'Dimmer': '亮度', 'Hue': '色调', 'Sat': '饱和', 'CT': '色温' }
  const actionType = Object.keys(config.action)[0]
  modeConfig.value.action.key = actionText[actionType] // 处理事件操作字段
  if (actionType === 'Power')
    modeConfig.value.action.value = config.action.Power ? '开' : '关'
  else modeConfig.value.action.value = Object.values(config.action)[0]

  // 添加配置中的设备到设备列表
  config.devices.forEach(device => {
    if (!deviceList.value.includes(device)) deviceList.value.push(device)
  })
}
</script>
