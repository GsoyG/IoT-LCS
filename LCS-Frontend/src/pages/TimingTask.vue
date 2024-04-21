<template>
  <v-container>
    <div class="d-flex flex-column fill-height justify-center align-center" v-if="taskList.length === 0">
      <h1 class="text-h4 font-weight-thin my-8">未查询到定时任务</h1>
      <h3 class="subheading">请点击右下角添加定时任务</h3>
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
                    <v-btn text="确认" color="red" @click="deleteTask(task.name)"></v-btn>
                    <v-btn text="取消" @click="isActive.value = false"></v-btn>
                  </v-card-actions>
                </v-card>
              </template>
            </v-dialog>

            <v-spacer></v-spacer>
            <!-- 编辑对话框 -->
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" color="indigo-accent-2" icon="mdi-square-edit-outline"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="定时任务编辑">
                  <div class="mx-4">
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
                <v-select v-model="taskConfig.action.key" :items="['电源', '亮度']" label="操作"></v-select>
              </v-col>
              <v-col cols="12" sm="4">
                <v-select v-model="taskConfig.action.value" :items="['开', '关']" label="结果"></v-select>
              </v-col>
            </v-row>
          </div>

          <v-card-actions>
            <v-spacer></v-spacer>
            <v-btn text="添加" color="indigo-accent-2" @click="addTask()"></v-btn>
            <v-btn text="关闭" @click="isActive.value = false"></v-btn>
          </v-card-actions>
        </v-card>
      </template>
    </v-dialog>
  </v-container>
</template>

<script setup>
import axios from 'axios'
import { ref, onMounted } from 'vue'

const taskList = ref([
  {
    'name': '测试任务',
    'devices': ['设备1', '设备2'],
    'time': '08:00',
    'repeat': [1, 2, 3],
    'action': {
      'Power': 24
    },
  }
])
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
  const actionText = { 'Power': '电源', 'Dimmer': '亮度', 'Hue': '色调', 'Sat': '饱和度', 'CT': '色温' }
  const actionType = Object.keys(action)[0]

  let text = actionText[actionType] + ' 设为 '
  if (actionType === 'Power') {
    text += action[actionType] ? '开' : '关'
    return text
  }
  text += action[actionType]
  return text
}

onMounted(() => {
  fetchTaskList()
})

// 获取定时任务列表
async function fetchTaskList() {
  try {
    const response = await axios.get('/api/timing/tasks')
    if (response.status === 200) {
      taskList.value = response.data
    } else {
      console.error('Failed to fetch timing task list:', response.statusText)
    }
  } catch (error) {
    console.error('Error fetching timing task list:', error)
  }
}

// 添加定时任务
async function addTask() {
  // 处理定时任务配置字段
  let data = {
    'name': taskConfig.value.name,
    'devices': [],
    'time': taskConfig.value.time,
    'repeat': [],
    'action': {}
  }
  const days = {'周一': 0, '周二': 1, '周三': 2, '周四': 3, '周五': 4, '周六': 5, '周日': 6 }
  const actionText = { '电源': 'Power', '亮度': 'Dimmer', '色调': 'Hue', '饱和度': 'Sat', '色温': 'CT' }
  for (let i = 0; i < taskConfig.value.devices.length; i++)
    data.devices.push(taskConfig.value.devices[i])
  for (let i = 0; i < taskConfig.value.repeat.length; i++)
    data.repeat.push(days[taskConfig.value.repeat[i]])
  if (taskConfig.value.action.key === '电源')
    data.action['Power'] = taskConfig.value.action.value === '开' ? 1 : 0
  else data.action[actionText[taskConfig.value.action.key]] = taskConfig.value.action.value

  // 发送添加请求
  try {
    const response = await axios.get('/api/timing/setTask', {
      params: {
        action: 'add',
        data: JSON.stringify(data)
      }
    })
    if (response.status === 200) {
      fetchTaskList()
    } else {
      console.error('Failed to fetch timing task list:', response.statusText)
    }
  } catch (error) {
    console.error('Error fetching timing task list:', error)
  }
}

// 删除定时任务
async function deleteTask(name) {
  try {
    const response = await axios.get('/api/timing/setTask', {
      params: {
        action: 'delete',
        data: JSON.stringify({
          name: name
        })
      }
    })
    if (response.status === 200) {
      fetchTaskList()
    } else {
      console.error('Failed to delete timing task list:', response.statusText)
    }
  } catch (error) {
    console.error('Error deleting timing task list:', error)
  }
}
</script>
