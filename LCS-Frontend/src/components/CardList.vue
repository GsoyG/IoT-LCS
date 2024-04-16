<template>
  <v-container>
    <v-row dense>
      <v-col v-for="item in items" cols="12" sm="6" md="4">
        <v-card outlined>
          <v-card-title>{{ item.Device }}</v-card-title>

          <v-card-text>
            <v-row class="pb-8" justify="center">
              <v-col cols="auto">
                <v-btn :prepend-icon="item.Reachable && Boolean(item.Power) ? 'mdi-lightbulb' : 'mdi-lightbulb-off'"
                  :color="item.Reachable && Boolean(item.Power) ? 'primary' : ''" variant="tonal" rounded="lg"
                  @click="switchLightingPower(item)" :disabled="disabledEdit || !item.Reachable" stacked>
                  {{ getLightingState(item) }}
                </v-btn>
              </v-col>
            </v-row>

            <v-progress-linear :model-value="item.Dimmer / 254 * 100" :color="`#${item.RGB}`" height="20"
              rounded></v-progress-linear>
          </v-card-text>

          <v-card-actions>
            <v-spacer></v-spacer>
            <v-dialog max-width="500">
              <template v-slot:activator="{ props: activatorProps }">
                <v-btn v-bind="activatorProps" text="编辑" :disabled="!item.Reachable"></v-btn>
              </template>

              <template v-slot:default="{ isActive }">
                <v-card title="设备编辑">
                  <v-card-text>
                    <v-slider min="0" max="254" step="1" v-model="item.Dimmer" label="亮度"
                      @click="changedConfig['Dimmer'] = item.Dimmer"></v-slider>
                  </v-card-text>

                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn text="设置" @click="saveLightingConfig(item)" color="primary"
                      :disabled="Object.keys(changedConfig).length === 0 || disabledEdit"></v-btn>
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

const items = ref([
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
const changedConfig = ref({});
const disabledEdit = ref(false);

onMounted(() => {
  fetchCardData();
});

async function fetchCardData() {
  disabledEdit.value = true;
  try {
    const response = await axios.get('/api/lighting/devices');
    if (response.status === 200) {
      items.value = response.data;
    } else {
      console.error('Failed to fetch card data:', response.statusText);
    }
  } catch (error) {
    console.error('Error fetching card data:', error);
  }
  await new Promise(resolve => setTimeout(resolve, 1000));
  disabledEdit.value = false;
}

function getLightingState(item) {
  if (!item.Reachable) {
    return '已离线';
  }
  if (item.Power) {
    return '已开启';
  }
  return '已关闭';
}

async function switchLightingPower(item) {
  disabledEdit.value = true;
  try {
    var power = Boolean(item.Power)
    var state = power ? '{"Power": 0}' : '{"Power": 1}';
    
    const response = await axios.get('/api/lighting/setState', {
      params: {
        device: item.Device,
        state: state,
      },
    });
    if (response.status === 200) {
      Object.assign(item, response.data);
      await new Promise(resolve => setTimeout(resolve, 1000));
      fetchCardData();
    } else {
      console.error('Failed to switch lighting power:', response.statusText);
    }
  } catch (error) {
    console.error('Error switching lighting power:', error);
  }
  disabledEdit.value = false;
}

async function saveLightingConfig(item) {
  disabledEdit.value = true;
  try {
    const response = await axios.get('/api/lighting/setState', {
      params: {
        device: item.Device,
        state: JSON.stringify(changedConfig.value),
      },
    });
  } catch (error) {
    console.error('Error saving lighting config:', error);
  }
  changedConfig.value = {};
  disabledEdit.value = false;
}
</script>