<template>
  <v-app>
    <v-main>
      <CardList :items="cardData" />
      <!-- <ControlPanel /> -->
    </v-main>
  </v-app>
</template>

<script>
import CardList from '@/components/CardList.vue';
import axios from 'axios';

export default {
  components: {
    CardList,
  },
  data() {
    return {
      cardData: [
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
      ],
    };
  },
  created() {
    this.fetchChartData();
  },
  methods: {
    async fetchChartData() {
      try {
        const response = await axios.get('/api/lighting/devices');
        if (response.status === 200) {
          this.chartData = response.data;
        } else {
          console.error('Failed to fetch chart data:', response.statusText);
        }
      } catch (error) {
        console.error('Error fetching chart data:', error);
      }
    },
  },
};
</script>
