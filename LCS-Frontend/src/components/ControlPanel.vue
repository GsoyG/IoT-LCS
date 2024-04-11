<template>
  <v-card>
    <v-card-title>Light Control</v-card-title>
    <v-card-text>
      <v-row>
        <v-col cols="12" sm="6">
          <h4>Switch</h4>
          <v-switch v-model="light.isOn" @change="toggleLight"></v-switch>
        </v-col>

        <v-col cols="12" sm="6">
          <h4>Color</h4>
          <v-color-picker v-model="light.color" @input="updateColor"></v-color-picker>
        </v-col>

        <v-col cols="12" sm="6">
          <h4>Brightness</h4>
          <v-slider min="0" max="100" step="1" v-model="light.brightness" @change="updateBrightness"></v-slider>
        </v-col>

        <v-col cols="12" sm="6">
          <h4>Change Button</h4>
          <v-btn color="primary" @click="sendUpdateRequest">Update Light Settings</v-btn>
        </v-col>
      </v-row>
    </v-card-text>
  </v-card>
</template>

<script>
import axios from 'axios';

export default {
  data() {
    return {
      light: {
        isOn: false,
        color: '#ffffff',
        brightness: 50,
      },
    };
  },
  methods: {
    async toggleLight() {
      this.light.isOn = !this.light.isOn;
      await this.sendUpdateRequest();
    },
    async updateColor() {
      await this.sendUpdateRequest();
    },
    async updateBrightness() {
      await this.sendUpdateRequest();
    },
    async sendUpdateRequest() {
      try {
        const apiUrl = 'https://your-api-url.com/lights/update'; // Replace with your actual API URL
        await axios.put(apiUrl, this.light);
        console.log('Light settings updated successfully.');
      } catch (error) {
        console.error('Error updating light settings:', error);
      }
    },
  },
};
</script>