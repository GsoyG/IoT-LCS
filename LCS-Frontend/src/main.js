import { registerPlugins } from '@/plugins'
import { createApp } from 'vue'
import App from './App.vue'
import Login from './Login.vue'
import axios from 'axios'

axios.defaults.baseURL = 'http://127.0.0.1:5000'
axios.defaults.timeout = 3000
axios.defaults.withCredentials = true

if (document.getElementById('app')) {
    const app = createApp(App)
    registerPlugins(app)
    app.mount('#app')
}
else if (document.getElementById('login-app')) {
    const app = createApp(Login)
    registerPlugins(app)
    app.mount('#login-app')
}

