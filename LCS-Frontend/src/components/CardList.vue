<template>
  <v-container>
    <v-row dense>
      <v-col v-for="item in items" cols="12" sm="6" md="4">
        <v-card outlined>
          <v-card-title>{{ item.Device }}</v-card-title>
          <div class="px-4 pb-4">
            <v-row no-gutters justify="center">
              <v-col cols="auto">
                <v-switch :model-value="Boolean(item.Power)" inset />
              </v-col>
            </v-row>
            <v-progress-linear :model-value="item.Dimmer" :color="`#${item.RGB}`" height="20"
              rounded></v-progress-linear>
            <v-row class="pt-4" no-gutters justify="end">
              <v-col cols="auto">
                <v-btn @click="handleEditClick">编辑</v-btn>
              </v-col>
            </v-row>
          </div>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
export default {
  props: {
    items: {
      type: Array,
      required: true,
    },
  },
};
</script>