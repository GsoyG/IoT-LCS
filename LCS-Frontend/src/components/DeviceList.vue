<template>
  <v-container>
    <v-row dense>
      <v-col v-for="device in deviceList" cols="12" sm="6" md="4">
        <v-card outlined>
          <v-card-title>{{ device.Device }}</v-card-title>

          <v-card-text>
            <v-row class="pb-8" justify="center">
              <v-col cols="auto">
                <v-btn :prepend-icon="device.Reachable && device.Dimmer > 1 ? 'mdi-lightbulb' : 'mdi-lightbulb-off'"
                  :color="device.Reachable && device.Dimmer > 1 ? 'primary' : ''" variant="tonal" rounded="lg"
                  @click="switchDevicePower(device)" :disabled="disabledEdit || !device.Reachable" stacked>
                  {{ getStatusText(device) }}
                </v-btn>
              </v-col>
            </v-row>

            <v-progress-linear :model-value="device.Dimmer / 254 * 100" :color="`#${device.RGB}`" height="20"
              rounded></v-progress-linear>
          </v-card-text>

          <v-card-actions>
            <v-spacer></v-spacer>
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" text="编辑" :disabled="!device.Reachable"
                  @click="openEditDialog(device)"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="设备编辑">
                  <v-card-text>
                    <v-slider min="1" max="254" step="1" v-model="deviceConfig.Dimmer" label="亮度"
                      :disabled="disabledEdit"
                      @end="uploadDeviceState(device, 'Dimmer', deviceConfig.Dimmer)"></v-slider>
                  </v-card-text>

                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn text="关闭" @click="isActive.value = false"></v-btn>
                  </v-card-actions>
                </v-card>
              </template>
            </v-dialog>
          </v-card-actions>

        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
import axios from 'axios';
import { ref, onMounted } from 'vue';

const deviceList = ref([
  {
    'Device': '灯泡1',
    'Power': 0,
    'Dimmer': 20,
    'RGB': 'FF5722',
  },
  {
    'Device': '灯泡2',
    'Power': 1,
    'Dimmer': 50,
    'RGB': '4CAF50',
  },
  {
    'Device': '0xA821',
    'IEEEAddr': '0xA4C138727417E5E1',
    'ModelId': 'TS0505B',
    'Manufacturer': '_TZ3210_mja6r5ix',
    'Endpoints': [
      1
    ],
    'Config': [
      'O01',
      'L01'
    ],
    'Power': 1,
    'Dimmer': 10,
    'Hue': 0,
    'Sat': 250,
    'X': 10040,
    'Y': 3116,
    'CT': 0,
    'ColorMode': 2,
    'RGB': 'FF0404',
    'RGBb': '0A0000',
    'Reachable': false,
    'LastSeen': 144544,
    'LastSeenEpoch': 1712682096,
    'LinkQuality': 105
  }
]);
const deviceConfig = ref({});
const disabledEdit = ref(false);

onMounted(() => {
  fetchDeviceList();
});

function getStatusText(device) {
  if (!device.Reachable) {
    return '已离线';
  }
  if (device.Dimmer > 1) {
    return '已开启';
  }
  return '已关闭';
}

async function fetchDeviceList() {
  disabledEdit.value = true;
  try {
    const response = await axios.get('/api/lighting/devices');
    if (response.status === 200) {
      deviceList.value = response.data;
    } else {
      console.error('Failed to fetch device list:', response.statusText);
    }
  } catch (error) {
    console.error('Error fetching device list:', error);
  }
  disabledEdit.value = false;
}

async function uploadDeviceState(device, key, value, disabled = true) {
  var state = {}
  state[key] = value;

  if (disabled) disabledEdit.value = true;
  try {
    const response = await axios.get('/api/lighting/setState', {
      params: {
        device: device.Device,
        state: JSON.stringify(state),
      },
    });
    if (response.status === 200) {
      Object.assign(device, state);

      if (disabled) disabledEdit.value = false;
      return true;
    } else {
      console.error('Failed to upload device state:', response.statusText);
    }
  } catch (error) {
    console.error('Error uploading device state:', error);
  }

  if (disabled) disabledEdit.value = false;
  return false;
}

async function switchDevicePower(device) {
  var power = device.Dimmer > 1 ? 0 : 1
  disabledEdit.value = true;

  if (await uploadDeviceState(device, 'Power', power, false)) {
    await new Promise(resolve => setTimeout(resolve, 2000));
    fetchDeviceList();
  }
  disabledEdit.value = false;
}

function openEditDialog(device) {
  deviceConfig.value = {
    'Dimmer': device.Dimmer,
    'RGB': device.RGB,
    'Hue': device.Hue,
    'Sat': device.Sat,
    'X': device.X,
    'Y': device.Y,
    'CT': device.CT,
    'ColorMode': device.ColorMode,
    'RGBb': device.RGBb,
  };
}
</script>