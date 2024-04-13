<template>
  <v-container>
    <v-row dense>
      <v-col v-for="item in items" cols="12" sm="6" md="4">
        <v-card outlined>
          <v-card-title>{{ item.Device }}</v-card-title>
          <div class="px-4 pb-4">
            <v-row class="pb-8" no-gutters justify="center">
              <v-col cols="auto">
                <v-btn prepend-icon="mdi-lightbulb" :color="`${item.Reachable && Boolean(item.Power) ? 'primary' : ''}`"
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

<script>
import axios from 'axios';

export default {
  props: {
    items: {
      type: Array,
      required: true,
    },
  },
  methods: {
    getLightingState(item) {
      if (!item.Reachable) {
        return '已离线';
      }
      if (item.Power) {
        return '已开启';
      }
      return '已关闭';
    },
    async switchLightingPower(item) {
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
          console.log(item);
        } else {
          console.error('Failed to switch lighting power:', response.statusText);
        }
      } catch (error) {
        console.error('Error switching lighting power:', error);
      }
    },
  },
};
</script>