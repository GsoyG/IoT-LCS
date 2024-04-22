<template>
  <v-container>
    <div class="d-flex flex-column fill-height justify-center align-center" v-if="taskList.length === 0">
      <h1 class="text-h4 font-weight-thin my-8">{{ emptyInfo.heading }}</h1>
      <h3 class="subheading">{{ emptyInfo.subheading }}</h3>
      <v-progress-circular color="indigo" indeterminate size="70" width="7"
        v-if="emptyInfo.subheading === ''"></v-progress-circular>
    </div>
    <v-row dense>
      <v-col v-for="task in taskList" cols="12" sm="6" md="4">
        <v-card outlined>
          <v-card-title>{{ task.name }}</v-card-title>

          <div class="mx-4">
            <span class="text-h5" v-for="device in task.devices">{{ device + '、' }}</span>
            <v-divider class="my-2"></v-divider>
            <p class="text-h6">时间：{{ task.time }}</p>
            <p class="text-h6">重复：{{ getRepeatText(task.repeat) }}</p>
            <p class="text-h6">操作：{{ getActionText(task.action) }}</p>
          </div>

          <v-card-actions>
            <!-- 删除对话框 -->
            <v-dialog max-width="400">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" color="red" icon="mdi-delete-outline"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="删除定时任务" text="确定删除此定时任务吗？此操作将不可恢复。">
                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn text="确认" color="red" @click="deleteTask(task.name, isActive)"></v-btn>
                    <v-btn text="取消" @click="isActive.value = false"></v-btn>
                  </v-card-actions>
                </v-card>
              </template>
            </v-dialog>

            <v-spacer></v-spacer>
            <!-- 编辑对话框 -->
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" color="indigo-accent-2" icon="mdi-square-edit-outline"
                  @click="editTask(task)"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="编辑定时任务">
                  <div class="mx-4 mt-4">
                    <v-text-field label="名称" v-model="taskConfig.name" disabled></v-text-field>
                    <v-select v-model="taskConfig.devices" :items="['设备1', '设备2']" label="设备" multiple></v-select>
                    <v-select v-model="taskConfig.repeat" :items="['周一', '周二', '周三', '周四', '周五', '周六', '周日']" label="重复"
                      multiple></v-select>
                    <v-divider class="mb-4"></v-divider>
                    <v-row dense>
                      <v-col cols="12" sm="4">
                        <v-text-field v-model="taskConfig.time" label="时间" type="time"></v-text-field>
                      </v-col>
                      <v-col cols="12" sm="4">
                        <v-select v-model="taskConfig.action.key" :items="['电源', '亮度', '色调', '饱和', '色温']"
                          label="操作"></v-select>
                      </v-col>
                      <v-col cols="12" sm="4">
                        <v-select v-model="taskConfig.action.value" :items="['开', '关']" label="结果"
                          v-if="taskConfig.action.key === '电源'"></v-select>
                        <v-text-field v-model="taskConfig.action.value" type="number" max="500" min="1" label="结果"
                          v-else></v-text-field>

                      </v-col>
                    </v-row>
                  </div>

                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn text="保存" color="indigo-accent-2" @click="updateTask(true, isActive)"></v-btn>
                    <v-btn text="关闭" @click="isActive.value = false"></v-btn>
                  </v-card-actions>
                </v-card>
              </template>
            </v-dialog>
          </v-card-actions>
        </v-card>
      </v-col>
    </v-row>

    <!-- 添加对话框 -->
    <v-dialog max-width="500">
      <template v-slot:activator="{ props: activatorProps }">
        <v-btn v-bind="activatorProps" icon="mdi-plus" size="large" color="indigo"
          style="position: fixed; bottom: 40px; right: 50px;"></v-btn>
      </template>

      <template v-slot:default="{ isActive }">
        <v-card title="添加定时任务">
          <div class="mx-4 mt-4">
            <v-text-field label="名称" v-model="taskConfig.name"></v-text-field>
            <v-select v-model="taskConfig.devices" :items="['设备1', '设备2']" label="设备" multiple></v-select>
            <v-select v-model="taskConfig.repeat" :items="['周一', '周二', '周三', '周四', '周五', '周六', '周日']" label="重复"
              multiple></v-select>
            <v-divider class="mb-4"></v-divider>
            <v-row dense>
              <v-col cols="12" sm="4">
                <v-text-field v-model="taskConfig.time" label="时间" type="time"></v-text-field>
              </v-col>
              <v-col cols="12" sm="4">
                <v-select v-model="taskConfig.action.key" :items="['电源', '亮度', '色调', '饱和', '色温']" label="操作"></v-select>
              </v-col>
              <v-col cols="12" sm="4">
                <v-select v-model="taskConfig.action.value" :items="['开', '关']" label="结果"
                  v-if="taskConfig.action.key === '电源'"></v-select>
                <v-text-field v-model="taskConfig.action.value" type="number" max="500" min="1" label="结果"
                  v-else></v-text-field>
              </v-col>
            </v-row>
          </div>

          <v-card-actions>
            <v-spacer></v-spacer>
            <v-btn text="添加" color="indigo-accent-2" @click="updateTask(false, isActive)"></v-btn>
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
const emptyInfo = ref({
  'heading': '获取定时任务列表中......',
  'subheading': ''
})
const snackbarConfig = ref({
  'show': false,
  'text': '',
  'icon': '',
  'iconColor': '',
})

onMounted(() => {
  fetchTaskList()
})

// 获取重复文本
function getRepeatText(repeat) {
  const days = ['周一', '周二', '周三', '周四', '周五', '周六', '周日']

  if (repeat.length === 1) {
    return days[repeat[0]]
  }
  if (repeat.length === 7) {
    return '每天'
  }
  if (repeat.length === 0) {
    return '不重复'
  }
  if (JSON.stringify(repeat) === JSON.stringify([5, 6])) {
    return '每周末'
  }
  if (JSON.stringify(repeat) === JSON.stringify([0, 1, 2, 3, 4])) {
    return '每周一至周五'
  }

  let text = ''
  repeat.forEach(day => {
    text += (days[day]) + '、'
  });
  return text
}
// 获取操作文本
function getActionText(action) {
  const actionText = { 'Power': '电源', 'Dimmer': '亮度', 'Hue': '色调', 'Sat': '饱和', 'CT': '色温' }
  const actionType = Object.keys(action)[0]

  let text = actionText[actionType] + ' 设为 '
  if (actionType === 'Power') {
    text += action[actionType] ? '开' : '关'
    return text
  }
  text += action[actionType]
  return text
}

// 显示提示消息条
function showMessage(text, icon, iconColor) {
  snackbarConfig.value = {
    show: true,
    text: text,
    icon: 'mdi-' + icon,
    iconColor: iconColor,
  }
}

// 获取定时任务列表
async function fetchTaskList() {
  await axios.get('/api/timing/tasks').then(response => {
    taskList.value = response.data

    if (taskList.value.length === 0) {
      emptyInfo.value = {
        'heading': '定时任务列表为空',
        'subheading': '未添加定时任务，请点击右下角按钮添加'
      }
    }
  }).catch(error => {
    if (error.response) {
      emptyInfo.value = {
        'heading': '获取定时任务列表失败',
        'subheading': '请稍后再试，错误信息：' + error.response.data
      }
    }
    else {
      emptyInfo.value = {
        'heading': '网络连接出错',
        'subheading': '请检查网络连接，错误信息：' + error.message
      }
    }
  })
}

// 更新定时任务，更新或添加
async function updateTask(isExisting, isActive) {
  // 处理定时任务配置字段
  let data = {
    'name': taskConfig.value.name,
    'devices': [],
    'time': taskConfig.value.time,
    'repeat': [],
    'action': {}
  }
  const days = {'周一': 0, '周二': 1, '周三': 2, '周四': 3, '周五': 4, '周六': 5, '周日': 6 }
  const actionText = { '电源': 'Power', '亮度': 'Dimmer', '色调': 'Hue', '饱和': 'Sat', '色温': 'CT' }
  for (let i = 0; i < taskConfig.value.devices.length; i++)
    data.devices.push(taskConfig.value.devices[i])
  for (let i = 0; i < taskConfig.value.repeat.length; i++)
    data.repeat.push(days[taskConfig.value.repeat[i]])
  if (taskConfig.value.action.key === '电源')
    data.action['Power'] = taskConfig.value.action.value === '开' ? 1 : 0
  else data.action[actionText[taskConfig.value.action.key]] = taskConfig.value.action.value

  // 发送更新请求
  await axios.get('/api/timing/setTask', {
    params: {
      action: isExisting ? 'update' : 'add',
      data: JSON.stringify(data)
    }
  }).then(response => {
    showMessage('更新任务成功', 'check-circle', 'green')
    isActive.value = false
    fetchTaskList()
  }).catch(error => {
    if (error.response)
      showMessage('更新任务失败：' + error.response.data, 'alert-circle', 'red')
    else showMessage('更新任务出错：' + error.message, 'alert-circle', 'red')
  });
}

// 删除定时任务
async function deleteTask(name, isActive) {
  await axios.get('/api/timing/setTask', {
    params: {
      action: 'delete',
      data: JSON.stringify({
        name: name
      })
    }
  }).then(response => {
    showMessage('删除任务成功', 'check-circle', 'green')
    fetchTaskList()
  }).catch(error => {
    if (error.response)
      showMessage('删除任务失败：' + error.response.data, 'alert-circle', 'red')
    else showMessage('删除任务出错：' + error.message, 'alert-circle', 'red')
  })
  isActive.value = false
}

// 打开编辑定时任务对话框
async function editTask(config) {
  taskConfig.value = {
    'name': config.name,
    'devices': [],
    'time': config.time,
    'repeat': [],
    'action': {}
  }

  const days = ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
  const actionText = { 'Power': '电源', 'Dimmer': '亮度', 'Hue': '色调', 'Sat': '饱和', 'CT': '色温' }
  const actionType = Object.keys(config.action)[0]

  // 处理定时任务配置字段
  for (let i = 0; i < config.devices.length; i++)
    taskConfig.value.devices.push(config.devices[i])
  for (let i = 0; i < config.repeat.length; i++)
    taskConfig.value.repeat.push(days[config.repeat[i]])
  if (actionType === 'Power') {
    taskConfig.value.action.key = '电源'
    taskConfig.value.action.value = config.action.Power ? '开' : '关'
  }
  else {
    taskConfig.value.action.key = actionText[actionType]
    taskConfig.value.action.value = Object.values(config.action)[0]
  }
}
</script>
