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
            <v-avatar>
              <v-img :alt="userData.user"
                :src="`http://q2.qlogo.cn/headimg_dl?dst_uin=${userData.qq}&spec=100`"></v-img>
            </v-avatar>
          </v-btn>
        </template>
        <v-card>
          <v-card-text>
            <div class="mx-auto text-center">
              <v-avatar size="80">
                <v-img :alt="userData.user"
                  :src="`http://q2.qlogo.cn/headimg_dl?dst_uin=${userData.qq}&spec=100`"></v-img>
              </v-avatar>
              <h2 class="mt-2">{{ userData.user }}</h2>
              <p class="mt-2">{{ userData.email }}</p>
              <v-divider class="my-2"></v-divider>
              <v-btn variant="text" rounded>
                编辑信息
              </v-btn>
              <v-divider class="my-2"></v-divider>
              <v-btn variant="text" rounded>
                退出登录
              </v-btn>
            </div>
          </v-card-text>
        </v-card>
      </v-menu>
    </template>
  </v-app-bar>

  <!-- 左侧抽屉 桌面端 -->
  <v-navigation-drawer v-model="drawer" :rail="rail" permanent @click="rail = false" v-if="!$vuetify.display.mobile">
    <v-list>
      <v-list-item prepend-avatar="http://q2.qlogo.cn/headimg_dl?dst_uin=2754205302&spec=100" title="作者：Gsoy"
        subtitle="日期：2024年4月1日"></v-list-item>
    </v-list>
    <v-divider></v-divider>
    <v-list nav>
      <v-list-item prepend-icon="mdi-lightbulb-group" title="设备管理" value="device"></v-list-item>
      <v-list-item prepend-icon="mdi-calendar-clock" title="定时任务" value="timing"></v-list-item>
      <v-list-item prepend-icon="mdi-view-quilt" title="情景模式" value="scene"></v-list-item>
      <v-list-item prepend-icon="mdi-account-box-multiple" title="账户管理" value="account"></v-list-item>
      <v-list-item prepend-icon="mdi-text-box-search" title="日志查看" value="log"></v-list-item>
    </v-list>
  </v-navigation-drawer>

  <!-- 左侧抽屉 移动端 -->
  <v-navigation-drawer v-model="rail" temporary @click="rail = false" v-if="$vuetify.display.mobile">
    <v-list>
      <v-list-item prepend-avatar="http://q2.qlogo.cn/headimg_dl?dst_uin=2754205302&spec=100" title="作者：Gsoy"
        subtitle="日期：2024年4月1日"></v-list-item>
    </v-list>
    <v-divider></v-divider>
    <v-list nav>
      <v-list-item prepend-icon="mdi-lightbulb-group" title="设备管理" value="device"></v-list-item>
      <v-list-item prepend-icon="mdi-calendar-clock" title="定时任务" value="timing"></v-list-item>
      <v-list-item prepend-icon="mdi-view-quilt" title="情景模式" value="scene"></v-list-item>
      <v-list-item prepend-icon="mdi-account-box-multiple" title="账户管理" value="account"></v-list-item>
      <v-list-item prepend-icon="mdi-text-box-search" title="日志查看" value="log"></v-list-item>
    </v-list>
  </v-navigation-drawer>
</template>

<script setup>
import { ref } from 'vue';

const userData = ref({
  'user': 'Gsoy',
  'qq': '2754205302',
  'email': 'GsoyG@icloud.com',
});
const drawer = ref(true);
const rail = ref(false);
</script>
