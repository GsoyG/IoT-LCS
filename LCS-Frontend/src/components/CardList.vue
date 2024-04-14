<template>
  <v-container>
    <v-row dense>
      <v-col v-for="item in items" cols="12" sm="6" md="4">
        <v-card outlined>
          <v-card-title>{{ item.Device }}</v-card-title>
          <div class="px-4 pb-4">
            <v-row class="pb-8" no-gutters justify="center">
              <v-col cols="auto">
                <v-btn
                  :prepend-icon="`${item.Reachable && Boolean(item.Power) ? 'mdi-lightbulb' : 'mdi-lightbulb-off'}`"
                  :color="`${item.Reachable && Boolean(item.Power) ? 'primary' : ''}`"
                  @click="switchLightingPower(item)" stacked>
                  {{ getLightingState(item) }}
                </v-btn>
              </v-col>
            </v-row>
            <v-progress-linear :model-value="item.Dimmer" :color="`#${item.RGB}`" height="20"
              rounded></v-progress-linear>
            <v-row class="pt-4" no-gutters justify="end">
              <v-col cols="auto">
                <v-btn>编辑</v-btn>
              </v-col>
            </v-row>
          </div>
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

onMounted(() => {
  fetchCardData();
});

async function fetchCardData() {
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
}

</script>