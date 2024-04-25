<template>
  <v-app>
    <v-main>
      <v-container class="d-flex flex-row fill-height justify-center align-center">
        <canvas id="canvas" class="position-absolute d-flex flex-row fill-height justify-center align-center"
          style="height: 100%; width: 100%;"></canvas>
        <v-row>
          <v-col cols="12">
            <v-card class="mx-auto pa-6" max-width="450" elevation="8">
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

                <div class="text-center mt-2" v-if="tooltipConfig.show">
                  <v-icon :icon="tooltipConfig.icon" :color="tooltipConfig.iconColor" class="mr-2"></v-icon>
                  {{ tooltipConfig.text }}
                </div>
                <v-btn class="mt-4" text="登录" :disabled="!data.form" :loading="data.loading" color="indigo" size="large"
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
const rules = ref({
  'username': value => !!value || '用户名不能为空',
  'password': value => !!value || '密码不能为空',
  'passwordMin': value => value.length >= 8 || '密码长度不能小于8位',
  'passwordShow': false
})
const tooltipConfig = ref({
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
      break;
    case 'warning':
      tooltipConfig.value = { show: true, text: text, icon: 'mdi-alert-circle', iconColor: 'warning' }
      break;
    default:
      break;
  }
}

// 登录
async function login() {
  if (!data.value.form) return

  data.value.loading = true
  await axios.post('/api/login', {
    username: data.value.username,
    password: data.value.password
  }).then(response => {
    showMessage('登录成功', 'success')
    window.location.href = "http://127.0.0.1:3000/";
  }).catch(error => {
    if (error.response)
      showMessage('登录失败 ' + error.response.data, 'warning')
    else showMessage('登录出错 ' + error.message, 'warning')
  });
  data.value.loading = false
}

// 背景动画
onMounted(() => {
  const rand = function (min, max) {
    return Math.random() * (max - min) + min;
  }

  let canvas = document.getElementById('canvas');
  let ctx = canvas.getContext('2d');
  canvas.width = window.innerWidth;
  canvas.height = window.innerHeight;

  window.addEventListener('resize', function () {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    ctx = canvas.getContext('2d');
    ctx.globalCompositeOperation = 'lighter';
  });
  let backgroundColors = ['#000', '#000'];
  let colors = [
    ['#ff0000', '#d000ff'],
    ['#ff0000', '#ff6300'],
    ['#0000ff', '#00b3ff'],
    ['#0000ff', '#ba00ff'],
    ['#00ff00', '#eeff00'],
    ['#00ff00', '#00ffc1'],
  ];
  let count = 70;
  let blur = [20, 70];
  let radius = [10, 120];

  if (window.innerWidth < 600) count = 30;

  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.globalCompositeOperation = 'lighter';

  let grd = ctx.createLinearGradient(0, canvas.height, canvas.width, 0);
  grd.addColorStop(0, backgroundColors[0]);
  grd.addColorStop(1, backgroundColors[1]);
  ctx.fillStyle = grd;
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  let items = [];

  while (count--) {
    let thisRadius = rand(radius[0], radius[1]);
    let thisBlur = rand(blur[0], blur[1]);
    let x = rand(-100, canvas.width + 100);
    let y = rand(-100, canvas.height + 100);
    let colorIndex = Math.floor(rand(0, 599) / 100);
    let colorOne = colors[colorIndex][0];
    let colorTwo = colors[colorIndex][1];

    ctx.beginPath();
    ctx.filter = `blur(${thisBlur}px)`;
    let grd = ctx.createLinearGradient(x - thisRadius / 2, y - thisRadius / 2, x + thisRadius, y + thisRadius);

    grd.addColorStop(0, colorOne);
    grd.addColorStop(1, colorTwo);
    ctx.fillStyle = grd;
    ctx.fill();
    ctx.arc(x, y, thisRadius, 0, Math.PI * 2);
    ctx.closePath();

    let directionX = Math.round(rand(-99, 99) / 100);
    let directionY = Math.round(rand(-99, 99) / 100);

    items.push({
      x: x,
      y: y,
      blur: thisBlur,
      radius: thisRadius,
      initialXDirection: directionX,
      initialYDirection: directionY,
      initialBlurDirection: directionX,
      colorOne: colorOne,
      colorTwo: colorTwo,
      gradient: [x - thisRadius / 2, y - thisRadius / 2, x + thisRadius, y + thisRadius],
    });
  }

  function changeCanvas(timestamp) {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    let adjX = 0.4;
    let adjY = 0.4;
    let adjBlur = 0.2;
    items.forEach(function (item) {
      if (item.x + (item.initialXDirection * adjX) >= canvas.width && item.initialXDirection !== 0 || item.x + (item.initialXDirection * adjX) <= 0 && item.initialXDirection !== 0) {
        item.initialXDirection = item.initialXDirection * -1;
      }
      if (item.y + (item.initialYDirection * adjY) >= canvas.height && item.initialYDirection !== 0 || item.y + (item.initialYDirection * adjY) <= 0 && item.initialYDirection !== 0) {
        item.initialYDirection = item.initialYDirection * -1;
      }

      if (item.blur + (item.initialBlurDirection * adjBlur) >= radius[1] && item.initialBlurDirection !== 0 || item.blur + (item.initialBlurDirection * adjBlur) <= radius[0] && item.initialBlurDirection !== 0) {
        item.initialBlurDirection *= -1;
      }

      item.x += (item.initialXDirection * adjX);
      item.y += (item.initialYDirection * adjY);
      item.blur += (item.initialBlurDirection * adjBlur);
      ctx.beginPath();
      ctx.filter = `blur(${item.blur}px)`;
      let grd = ctx.createLinearGradient(item.gradient[0], item.gradient[1], item.gradient[2], item.gradient[3]);
      grd.addColorStop(0, item.colorOne);
      grd.addColorStop(1, item.colorTwo);
      ctx.fillStyle = grd;
      ctx.arc(item.x, item.y, item.radius, 0, Math.PI * 2);
      ctx.fill();
      ctx.closePath();
    });
    window.requestAnimationFrame(changeCanvas);
  }
  window.requestAnimationFrame(changeCanvas);
})
</script>
