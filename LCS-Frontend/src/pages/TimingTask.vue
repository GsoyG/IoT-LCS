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
            <!-- 定时任务编辑对话框 -->
            <v-spacer></v-spacer>
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" icon="mdi-square-edit-outline" @click=""></v-btn>
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

    <v-btn icon="mdi-plus" size="large" color="indigo" style="position: fixed; bottom: 40px; right: 50px;"></v-btn>
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
function getActionText(action) {
  const actionText = {
    'Power': '电源',
    'Dimmer': '亮度',
    'Hue': '色调',
    'Sat': '饱和度',
    'CT': '色温'
  }
  const actionType = Object.keys(action)[0]

  let text = actionText[actionType] + ' 设为 '
  if (actionType === 'Power') {
    text += action[actionType] ? '开' : '关'
    return text
  }
  text += action[actionType]
  return text
}
</script>
