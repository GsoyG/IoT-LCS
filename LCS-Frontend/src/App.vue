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

export default {
  components: {
    CardList,
  },
  data() {
    return {
      cardData: [
        {
          id: 1,
          title: '灯泡1',
          brightness: 20,
          color: '#FF5722',
          switchState: false,
        },
        {
          id: 2,
          title: '灯泡2',
          brightness: 50,
          color: '#4CAF50',
          switchState: true,
        },
      ],
    };
  },
};
</script>
