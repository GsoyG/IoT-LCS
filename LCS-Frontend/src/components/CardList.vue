<template>
  <v-container>
    <v-row dense>
      <v-col v-for="item in items" :key="item.id" cols="12" sm="6" md="4">
        <v-card outlined>
          <v-card-title>{{ item.title }}</v-card-title>
          <div class="px-4 pb-4">
            <v-row no-gutters justify="center">
              <v-col cols="auto">
                <v-switch v-model="item.switchState" inset />
              </v-col>
            </v-row>
            <v-progress-linear :model-value="item.brightness" :color="item.color" height="20"
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