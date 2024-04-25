<template>
  <v-app>
    <v-main>
      <v-container class="d-flex flex-row fill-height justify-center align-center">
        <v-row>
          <v-col cols="12">
            <v-card class="mx-auto pa-6" max-width="450" elevation="6">
              <p class="text-center text-h4 font-weight-thin mb-8 mt-4">智能照明控制系统</p>

              <v-form v-model="data.form" @submit.prevent="login()">
                <v-text-field label="用户名" v-model="data.username" :readonly="data.loading" :rules="[rules.username]"
                  placeholder="请输入用户名" clearable variant="outlined" prepend-icon="mdi-account" append-icon="none"
                  class="mb-4"></v-text-field>
                <v-text-field label="密码" v-model="data.password" :readonly="data.loading"
                  :rules="[rules.password, rules.passwordMin]" placeholder="请输入密码" clearable variant="outlined"
                  prepend-icon="mdi-lock" :append-icon="rules.passwordShow ? 'mdi-eye' : 'mdi-eye-off'"
                  :type="rules.passwordShow ? 'text' : 'password'"
                  @click:append="rules.passwordShow = !rules.passwordShow"></v-text-field>

                <br>
                <v-btn text="登录" :disabled="!data.form" :loading="data.loading" color="indigo" size="large"
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
import { ref } from 'vue'

const data = ref({
  'form': false,
  'username': null,
  'password': null,
  'loading': false,
})
const rules = ref({
  'username': value => !!value || '用户名不能为空',
  'password': value => !!value || '密码不能为空',
  'passwordMin': value => value.length >= 8 || '密码长度不能小于8位',
  'passwordShow': false
})

function login() {
  if (!data.value.form) return
  data.value.loading = true
  setTimeout(() => (data.value.loading = false), 2000)
}
</script>
