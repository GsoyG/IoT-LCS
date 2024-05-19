<template>
  <v-container>
    <v-card title="日志查看">
      <v-data-table-server v-model:items-per-page="itemsPerPage" :headers="headers" :items="logList"
        :items-length="totalItems" :loading="loading" loading-text="获取日志列表中......" no-data-text="日志列表为空"
        :page="currentPage"
        :page-text="`第 ${(currentPage - 1) * itemsPerPage + 1} - ${(currentPage - 1) * itemsPerPage + logList.length} 条 共 ${totalItems} 条`"
        items-per-page-text="显示数量：" @update:options="loadItems" :items-per-page-options="itemsPerPageOptions">

        <!-- 表头，包含过滤器 -->
        <template v-slot:header.time>
          <v-btn append-icon="mdi-filter-menu" variant="text" text="日期时间"></v-btn>
        </template>
        <template v-slot:header.type>
          <v-btn variant="text"
            :append-icon="filterData.type != '' && filterData.type != null ? 'mdi-filter-cog' : 'mdi-filter-menu'"
            :color="filterData.type != '' && filterData.type != null ? 'indigo-accent-2' : ''">
            操作类型
            <v-menu activator="parent" :close-on-content-click="false">
              <v-card min-width="300">
                <v-card-text>
                  <v-select v-model="filterData.type" :items="['用户认证', '设备管理', '定时任务', '情景模式', '账户管理', '日志查看', '系统设置']"
                    clearable label="类型" variant="outlined" hide-details
                    @update:model-value="fetchLogList(0)"></v-select>
                </v-card-text>
              </v-card>
            </v-menu>
          </v-btn>
        </template>
        <template v-slot:header.message>
          <v-btn variant="text"
            :append-icon="filterData.message != '' && filterData.message != null ? 'mdi-filter-cog' : 'mdi-filter-menu'"
            :color="filterData.message != '' && filterData.message != null ? 'indigo-accent-2' : ''">
            操作信息
            <v-menu activator="parent" :close-on-content-click="false">
              <v-card min-width="300">
                <v-card-text>
                  <v-text-field v-model="filterData.message" label="信息" variant="outlined" hide-details clearable
                    @update:model-value="fetchLogList(0)"></v-text-field>
                </v-card-text>
              </v-card>
            </v-menu>
          </v-btn>
        </template>
        <template v-slot:header.address>
          <v-btn variant="text"
            :append-icon="filterData.address != '' && filterData.address != null ? 'mdi-filter-cog' : 'mdi-filter-menu'"
            :color="filterData.address != '' && filterData.address != null ? 'indigo-accent-2' : ''">
            IP地址
            <v-menu activator="parent" :close-on-content-click="false">
              <v-card min-width="300">
                <v-card-text>
                  <v-text-field v-model="filterData.address" label="信息" variant="outlined" hide-details clearable
                    @update:model-value="fetchLogList(0)"></v-text-field>
                </v-card-text>
              </v-card>
            </v-menu>
          </v-btn>
        </template>
        <template v-slot:header.user>
          <v-btn variant="text"
            :append-icon="filterData.user != '' && filterData.user != null ? 'mdi-filter-cog' : 'mdi-filter-menu'"
            :color="filterData.user != '' && filterData.user != null ? 'indigo-accent-2' : ''">
            用户名称
            <v-menu activator="parent" :close-on-content-click="false">
              <v-card min-width="300">
                <v-card-text>
                  <v-text-field v-model="filterData.user" label="信息" variant="outlined" hide-details clearable
                    @update:model-value="fetchLogList(0)"></v-text-field>
                </v-card-text>
              </v-card>
            </v-menu>
          </v-btn>
        </template>
      </v-data-table-server>
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

const currentPage = ref(1)
const headers = [
  { key: 'time', sortable: false, width: '15%', minWidth: '180px', title: '日期时间' },
  { key: 'type', sortable: false, width: '10%', minWidth: '100px', title: '操作类型' },
  { key: 'message', sortable: false, width: '55%', minWidth: '200px', title: '操作信息' },
  { key: 'address', sortable: false, width: '10%', minWidth: '100px', title: 'IP地址' },
  { key: 'user', sortable: false, width: '10%', minWidth: '100px', title: '用户名称' },
]
const logList = ref([])
const loading = ref(true)
const totalItems = ref(0)
const itemsPerPage = ref(8)
const itemsPerPageOptions = [ // 每页显示多少条
  { value: 5, title: '5' },
  { value: 10, title: '10' },
  { value: 15, title: '15' },
  { value: 20, title: '20' },
]
const filterData = ref({
  user: '',
  address: '',
  type: '',
  message: '',
})

const snackbarConfig = ref({ // 提示消息条信息
  'show': false,
  'text': '',
  'icon': '',
  'iconColor': '',
})

onMounted(() => {
  fetchLogList(0)
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
async function fetchLogList(offset) {
  loading.value = true
  currentPage.value = offset / itemsPerPage.value + 1 // 当前页码

  await axios.get('/api/log/logs', {
    params: {
      offset: offset,
      count: itemsPerPage.value,
      filter: JSON.stringify(filterData.value)
    }
  }).then(response => {
    logList.value = response.data.logs
    totalItems.value = response.data.total

    logList.value.forEach(log => { // 格式化时间
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
  fetchLogList((page - 1) * itemsPerPage, itemsPerPage)
}
</script>
