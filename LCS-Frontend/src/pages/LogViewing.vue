<template>
  <v-container>
    <v-card title="日志查看">
      <v-data-table-server v-model:items-per-page="itemsPerPage" :headers="headers" :items="logList"
        :items-length="totalItems" :loading="loading" loading-text="获取日志列表中......" no-data-text="日志列表为空"
        :page-text="`第 ${(currentPage - 1) * itemsPerPage + 1} - ${(currentPage - 1) * itemsPerPage + logList.length} 条 共 ${totalItems} 条`"
        items-per-page-text="显示数量：" @update:options="loadItems"
        :items-per-page-options="itemsPerPageOptions"></v-data-table-server>
    </v-card>

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

const itemsPerPage = ref(8)
const currentPage = ref(1)
const headers = [
  { key: 'time', sortable: false, width: '15%', title: '日期时间' },
  { key: 'type', sortable: false, width: '15%', title: '操作类型' },
  { key: 'message', sortable: false, width: '45%', title: '操作信息' },
  { key: 'address', sortable: false, width: '15%', title: 'IP地址' },
  { key: 'user', sortable: false, width: '10%', title: '用户名称' },
]
const logList = ref([])
const loading = ref(true)
const totalItems = ref(0)
const itemsPerPageOptions = [
  { value: 5, title: '5' },
  { value: 10, title: '10' },
  { value: 15, title: '15' },
  { value: 20, title: '20' }
]

const snackbarConfig = ref({ // 提示消息条信息
  'show': false,
  'text': '',
  'icon': '',
  'iconColor': '',
})

onMounted(() => {
  fetchLogList(0, itemsPerPage.value, {})
})

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

// 获取日志列表
async function fetchLogList(offset, count, filter) {
  loading.value = true

  await axios.get('/api/log/logs', {
    params: {
      offset: offset,
      count: count,
      filter: JSON.stringify(filter)
    }
  }).then(response => {
    logList.value = response.data.logs
    totalItems.value = response.data.total

    logList.value.forEach(log => {
      const time = new Date(log.timestamp * 1000)
      log.time = time.toLocaleString()
    })
  }).catch(error => {
    if (error.response)
      showMessage('获取日志列表失败：' + error.response.data, 'warning')
    else showMessage('获取日志列表出错：' + error.message, 'warning')
  })
  loading.value = false
}

// 更新列表显示
function loadItems({ page, itemsPerPage, sortBy }) {
  currentPage.value = page
  fetchLogList((page - 1) * itemsPerPage, itemsPerPage, {})
}
</script>
