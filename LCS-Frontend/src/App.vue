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
import DeviceList from './components/DeviceList.vue'
import NotFound from './pages/NotFound.vue'

const routes = {
  '/': DeviceList
}

const currentPath = ref(window.location.hash)

window.addEventListener('hashchange', () => {
  currentPath.value = window.location.hash
})

const currentView = computed(() => {
  return routes[currentPath.value.slice(1) || '/'] || NotFound
})
</script>
