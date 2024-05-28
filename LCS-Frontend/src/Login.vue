<template>
  <v-app>
    <v-main>
      <v-container class="d-flex flex-row fill-height justify-center align-center">
        <v-row>
          <v-col cols="12">
            <v-card class="mx-auto pa-6" max-width="450" elevation="8">
              <p class="text-center text-h4 font-weight-thin mb-8 mt-4">智能照明控制系统</p>

              <!-- 登入表单 -->
              <v-form v-model="data.form" @submit.prevent="login()">
                <v-text-field label="用户名" v-model="data.username" :readonly="data.loading" :rules="[rules.username]"
                  placeholder="请输入用户名" clearable variant="outlined" prepend-icon="mdi-account" append-icon="none"
                  class="mb-4"></v-text-field>
                <v-text-field label="密码" v-model="data.password" :readonly="data.loading"
                  :rules="[rules.password, rules.passwordMin]" placeholder="请输入密码" clearable variant="outlined"
                  prepend-icon="mdi-lock" :append-icon="rules.passwordShow ? 'mdi-eye' : 'mdi-eye-off'"
                  :type="rules.passwordShow ? 'text' : 'password'"
                  @click:append="rules.passwordShow = !rules.passwordShow"></v-text-field>

                <!-- 登入提示信息 -->
                <div class="text-center mt-2" v-if="tooltipConfig.show">
                  <v-icon :icon="tooltipConfig.icon" :color="tooltipConfig.iconColor" class="mr-2"></v-icon>
                  {{ tooltipConfig.text }}
                </div>
                <!-- 登入按钮 -->
                <v-btn class="mt-4" text="登入" :disabled="!data.form" :loading="data.loading" color="indigo" size="large"
                  type="submit" variant="elevated" block></v-btn>
              </v-form>
            </v-card>
          </v-col>
        </v-row>
      </v-container>
    </v-main>
  </v-app>
</template>

<script setup>
import axios from 'axios'
import { ref, onMounted } from 'vue'

const data = ref({
  'form': false,
  'username': null,
  'password': null,
  'loading': false,
})
const rules = ref({ // 输入验证规则
  'username': value => !!value || '用户名不能为空',
  'password': value => !!value || '密码不能为空',
  'passwordMin': value => value.length >= 8 || '密码长度不能小于8位',
  'passwordShow': false
})
const tooltipConfig = ref({ // 提示文字信息
  'show': false,
  'text': '',
  'icon': '',
  'iconColor': '',
})

// 显示提示文字
function showMessage(text, type) {
  switch (type) {
    case 'success':
      tooltipConfig.value = { show: true, text: text, icon: 'mdi-check-circle', iconColor: 'success' }
      break
    case 'warning':
      tooltipConfig.value = { show: true, text: text, icon: 'mdi-alert-circle', iconColor: 'warning' }
      break
    default:
      break
  }
}

onMounted(() => {
  getUserInfo()
})

// 登入
async function login() {
  if (!data.value.form) return

  data.value.loading = true
  await axios.post('/api/login', {
    username: data.value.username,
    password: data.value.password
  }).then(response => {
    showMessage('登入成功', 'success')
    window.location.href = '/dashboard'
  }).catch(error => {
    if (error.response)
      showMessage('登入失败 ' + error.response.data, 'warning')
    else showMessage('登入出错 ' + error.message, 'warning')
  })
  data.value.loading = false
}

// 获取用户信息
async function getUserInfo() {
  await axios.get('/api/user/info').then(response => {
    window.location.href = '/dashboard'
  })
}
</script>
