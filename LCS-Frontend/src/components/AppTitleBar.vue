<template>
  <!-- 顶部标题栏 -->
  <v-app-bar color="indigo">
    <!-- 抽屉图标 -->
    <template v-slot:prepend>
      <v-app-bar-nav-icon @click.stop="rail = !rail"></v-app-bar-nav-icon>
    </template>

    <v-app-bar-title>智能照明控制系统</v-app-bar-title>

    <!-- 头像 -->
    <template v-slot:append>
      <v-menu min-width="200px" rounded>
        <template v-slot:activator="{ props }">
          <v-btn icon v-bind="props">
            <v-avatar color="white">
              <v-img :alt="userData.username" :src="`http://q2.qlogo.cn/headimg_dl?dst_uin=${userData.qq}&spec=100`"
                v-if="userData.qq != ''"></v-img>
              <v-icon icon="mdi-account" color="indigo" size="large" v-else></v-icon>
            </v-avatar>
          </v-btn>
        </template>
        <v-card>
          <v-card-text>
            <div class="mx-auto text-center">
              <v-avatar color="white" size="80">
                <v-img :alt="userData.username" :src="`http://q2.qlogo.cn/headimg_dl?dst_uin=${userData.qq}&spec=100`"
                  v-if="userData.qq != ''"></v-img>
                <v-icon icon="mdi-account" color="indigo" size="60" v-else></v-icon>
              </v-avatar>
              <h2 class="my-4">{{ userData.username }}</h2>
              <p>{{ userData.email }}</p>
              <v-divider class="my-2"></v-divider>
              <v-btn text="编辑信息" variant="text" rounded></v-btn>
              <v-divider class="my-2"></v-divider>
              <v-btn text="退出登入" variant="text" rounded @click="logout"></v-btn>
            </div>
          </v-card-text>
        </v-card>
      </v-menu>
    </template>
  </v-app-bar>

  <!-- 左侧抽屉 移动端 -->
  <v-navigation-drawer v-model="rail" temporary @click="rail = false" v-if="$vuetify.display.mobile">
    <AppNavList />
  </v-navigation-drawer>

  <!-- 左侧抽屉 桌面端 -->
  <v-navigation-drawer v-model="drawer" :rail="rail" permanent @click="rail = false" v-else>
    <AppNavList />
  </v-navigation-drawer>

  <!-- 提示消息条 -->
  <v-snackbar timeout="2000" v-model="snackbarConfig.show" color="indigo">
    <v-icon :icon="snackbarConfig.icon" :color="snackbarConfig.iconColor" class="mr-2"></v-icon>
    {{ snackbarConfig.text }}
  </v-snackbar>
</template>

<script setup>
import axios from 'axios'
import { ref, onMounted } from 'vue'

const userData = ref({})
const drawer = ref(true) // 折叠侧边栏
const rail = ref(false) // 显示侧边栏
const snackbarConfig = ref({ // 提示消息条信息
  'show': false,
  'text': '',
  'icon': '',
  'iconColor': '',
})

onMounted(() => {
  getUserInfo()
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

// 获取用户信息
async function getUserInfo() {
  await axios.get('/api/user/info').then(response => {
    userData.value = response.data
  }).catch(error => {
    if (error.response)
      showMessage('获取用户信息失败 ' + error.response.data, 'warning')
    else showMessage('获取用户信息出错 ' + error.message, 'warning')
  })
}

// 登出
async function logout() {
  await axios.post('/api/logout').then(response => {
    window.location.href = "http://127.0.0.1:3000/login"
  }).catch(error => {
    if (error.response)
      showMessage('退出登入失败 ' + error.response.data, 'warning')
    else showMessage('退出登入出错 ' + error.message, 'warning')
  })
}
</script>
