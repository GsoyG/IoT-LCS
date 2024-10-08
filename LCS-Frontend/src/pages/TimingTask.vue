<template>
  <v-container>
    <!-- 加载信息 -->
    <div class="d-flex flex-column fill-height justify-center align-center" v-if="taskList.length === 0">
      <h1 class="text-h4 font-weight-thin my-8">{{ emptyInfo.heading }}</h1>
      <h3 class="subheading">{{ emptyInfo.subheading }}</h3>
      <v-progress-circular color="indigo" indeterminate size="70" width="7"
        v-if="emptyInfo.subheading === ''"></v-progress-circular>
    </div>
    <v-row>
      <v-col v-for="task in taskList" cols="12" sm="6" md="4">

        <v-card outlined max-height="400">
          <v-card-title>{{ task.name }}</v-card-title>

          <!-- 显示面板 -->
          <div class="mx-4">
            <div class="text-truncate">
              <span class="text-h6" v-if="task.devices.length === 0">未绑定设备</span>
              <span class="text-h6" v-for="device in task.devices">{{ device + '、' }}</span>
            </div>
            <v-divider class="my-2"></v-divider>
            <div class="d-flex justify-space-between align-center">
              <span class="text-h5">时间：{{ task.time }}</span>
              <v-switch v-model="task.enable" :label="task.enable ? '启用' : '禁用'" color="indigo" class="text-right"
                @update:model-value="setTaskEnable(task.name, task.enable)" hide-details></v-switch>
            </div>
            <p class="text-truncate">重复：{{ parseRepeatText(task.repeat) }}</p>
            <p>操作：{{ parseActionText(task.action) }}</p>
          </div>

          <v-card-actions>
            <!-- 删除按钮 -->
            <v-btn color="red" icon="mdi-delete-outline"
              @click="deleteDialog.show = true;deleteDialog.taskName = task.name;"></v-btn>

            <v-spacer></v-spacer>
            <!-- 编辑对话框及按钮 -->
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" color="indigo-accent-2" icon="mdi-square-edit-outline"
                  @click="editTask(task)"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="编辑定时任务">
                  <div class="mx-4 mt-4">
                    <v-text-field label="名称" v-model="taskConfig.name" disabled></v-text-field>
                    <v-select v-model="taskConfig.devices" :items="deviceList" label="设备" multiple
                      no-data-text="无可用设备"></v-select>
                    <v-select v-model="taskConfig.repeat" :items="['周一', '周二', '周三', '周四', '周五', '周六', '周日']" label="重复"
                      multiple></v-select>
                    <v-divider class="mb-4"></v-divider>
                    <v-row dense>
                      <v-col>
                        <v-text-field v-model="taskConfig.time" label="时间" type="time"></v-text-field>
                      </v-col>
                      <v-col>
                        <v-select v-model="taskConfig.action.key" :items="['电源', '亮度', '色调', '饱和', '色温']"
                          label="操作"></v-select>
                      </v-col>
                      <v-col>
                        <v-select v-model="taskConfig.action.value" :items="['开', '关']" label="结果"
                          v-if="taskConfig.action.key === '电源'"></v-select>
                        <v-text-field v-model="taskConfig.action.value" type="number" max="500" min="1" label="结果"
                          v-else></v-text-field>
                      </v-col>
                    </v-row>
                  </div>

                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn text="保存" color="indigo-accent-2" @click="updateTask(false); isActive.value = false"></v-btn>
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
      <v-card title="删除定时任务" text="确定删除此定时任务吗？此操作将不可恢复。">
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn text="确认" color="red" @click="deleteTask(deleteDialog.taskName); deleteDialog.show = false"></v-btn>
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
        <v-card title="添加定时任务">
          <div class="mx-4 mt-4">
            <v-text-field label="名称" v-model="taskConfig.name"></v-text-field>
            <v-select v-model="taskConfig.devices" :items="deviceList" label="设备" multiple
              no-data-text="无可用设备"></v-select>
            <v-select v-model="taskConfig.repeat" :items="['周一', '周二', '周三', '周四', '周五', '周六', '周日']" label="重复"
              multiple></v-select>
            <v-divider class="mb-4"></v-divider>
            <v-row dense>
              <v-col>
                <v-text-field v-model="taskConfig.time" label="时间" type="time"></v-text-field>
              </v-col>
              <v-col>
                <v-select v-model="taskConfig.action.key" :items="['电源', '亮度', '色调', '饱和', '色温']" label="操作"></v-select>
              </v-col>
              <v-col>
                <v-select v-model="taskConfig.action.value" :items="['开', '关']" label="结果"
                  v-if="taskConfig.action.key === '电源'"></v-select>
                <v-text-field v-model="taskConfig.action.value" type="number" max="500" min="1" label="结果"
                  v-else></v-text-field>
              </v-col>
            </v-row>
          </div>

          <v-card-actions>
            <v-spacer></v-spacer>
            <v-btn text="添加" color="indigo-accent-2" @click="updateTask(true); isActive.value = false"></v-btn>
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

const taskList = ref([])
const deviceList = ref([])
const taskConfig = ref({
  'name': '',
  'devices': [],
  'time': '12:30:00',
  'repeat': [],
  'action': {
    'key': '',
    'value': ''
  },
})
const emptyInfo = ref({ // 加载提示信息
  'heading': '获取定时任务列表中......',
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
  'taskName': ''
})

onMounted(() => {
  fetchTaskList()
  fetchDeviceList()
})

// 获取重复文本
function parseRepeatText(repeat) {
  const days = ['周一', '周二', '周三', '周四', '周五', '周六', '周日']

  if (repeat.length === 7 || repeat.length === 0) return '每天'
  if (repeat.length === 2 && [5, 6].every(i => repeat.includes(i))) return '每周末'
  if (repeat.length === 5 && [0, 1, 2, 3, 4].every(i => repeat.includes(i))) return '每周一至周五'
  let text = ''
  repeat.forEach(day => text += days[day] + '、')
  return text.substring(0, text.length - 1)
}
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

// 获取定时任务列表
async function fetchTaskList() {
  await axios.get('/api/timing/tasks').then(response => {
    taskList.value = response.data

    if (taskList.value.length === 0)
      emptyInfo.value = {
        'heading': '定时任务列表为空',
        'subheading': '未添加定时任务，请点击右下角按钮添加'
      }
    emptyInfo.value.isDisconnect = false
  }).catch(error => {
    if (error.response)
      emptyInfo.value = {
        'heading': '获取定时任务列表失败',
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

// 发送设置任务请求，用于添加、修改、删除任务
async function setTaskRequest(action, data, infoText) {
  await axios.get('/api/timing/setTask', {
    params: {
      action: action,
      data: JSON.stringify(data)
    }
  }).then(response => {
    showMessage(infoText + '任务成功', 'success')
  }).catch(error => {
    if (error.response)
      showMessage(infoText + '任务失败：' + error.response.data, 'warning')
    else showMessage(infoText + '任务出错：' + error.message, 'warning')
  })
  fetchTaskList()
}

// 设置定时任务，更新或添加
async function updateTask(isAddTask) {
  // 处理定时任务配置字段
  let data = {
    'name': taskConfig.value.name,
    'devices': taskConfig.value.devices,
    'time': taskConfig.value.time,
    'repeat': [],
    'action': {}
  }
  const days = {'周一': 0, '周二': 1, '周三': 2, '周四': 3, '周五': 4, '周六': 5, '周日': 6 }
  const actionText = { '电源': 'Power', '亮度': 'Dimmer', '色调': 'Hue', '饱和': 'Sat', '色温': 'CT' }
  const actionType = taskConfig.value.action.key
  for (let i = 0; i < taskConfig.value.repeat.length; i++) // 处理重复列表字段
    data.repeat.push(days[taskConfig.value.repeat[i]])
  if (actionType === '电源') // 处理事件操作字段
    data.action['Power'] = taskConfig.value.action.value === '开' ? 1 : 0
  else data.action[actionText[actionType]] = taskConfig.value.action.value
  if (isAddTask) data['enable'] = true // 新建任务默认启用

  // 发送更新请求
  await setTaskRequest(isAddTask ? 'add' : 'update', data, '设置')
}

// 设置定时任务开启状态
async function setTaskEnable(taskName, isEnable) {
  const result = isEnable ? '开启' : '关闭'
  await setTaskRequest('update', { name: taskName, enable: isEnable }, result)
}

// 删除定时任务
async function deleteTask(taskName) {
  await setTaskRequest('delete', { name: taskName }, '删除')
}

// 打开编辑定时任务对话框
async function editTask(config) {
  // 处理定时任务配置字段
  taskConfig.value = {
    'name': config.name,
    'devices': config.devices,
    'time': config.time,
    'repeat': [],
    'action': {}
  }
  const days = ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
  const actionText = { 'Power': '电源', 'Dimmer': '亮度', 'Hue': '色调', 'Sat': '饱和', 'CT': '色温' }
  const actionType = Object.keys(config.action)[0]
  for (let i = 0; i < config.repeat.length; i++) // 处理重复列表字段
    taskConfig.value.repeat.push(days[config.repeat[i]])
  taskConfig.value.action.key = actionText[actionType] // 处理事件操作字段
  if (actionType === 'Power')
    taskConfig.value.action.value = config.action.Power ? '开' : '关'
  else taskConfig.value.action.value = Object.values(config.action)[0]

  // 添加配置中的设备到设备列表
  config.devices.forEach(device => {
    if (!deviceList.value.includes(device)) deviceList.value.push(device)
  })
}
</script>
