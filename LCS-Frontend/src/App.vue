<template>
  <v-app>
    <v-main>
      <AppTitleBar />
      <component :is="currentView" />
    </v-main>
  </v-app>
</template>

<script setup>
import { ref, computed } from 'vue'
import DeviceList from './pages/DeviceList.vue'
import TimingTask from './pages/TimingTask.vue'
import SceneMode from './pages/SceneMode.vue'
import LogViewing from './pages/LogViewing.vue'
import NotFound from './pages/NotFound.vue'

const routes = {
  '/': DeviceList,
  '/device': DeviceList,
  '/timing': TimingTask,
  '/scene': SceneMode,
  '/log': LogViewing
}
const currentPath = ref(window.location.hash)

window.addEventListener('hashchange', () => {
  currentPath.value = window.location.hash
})

const currentView = computed(() => {
  return routes[currentPath.value.slice(1) || '/'] || NotFound
})
</script>
