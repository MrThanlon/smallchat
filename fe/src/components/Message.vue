<template>
  <div class="col-12 d-flex flex-nowrap justify-content-center mb-1"
       v-if="'action' in message">
    <span class="badge bg-secondary rounded-pill">
      {{ message.username }} {{message.action}} the room
    </span>
  </div>
  <div v-else
       class="col-12 d-flex flex-nowrap mb-1"
       :style="{flexDirection:me?'row-reverse':'row'}">
    <div class="avatar">{{ message.username[0] }}</div>
    <div class="message-box flex-grow-1">
      <div class="username d-flex" :style="{flexDirection:me?'row-reverse':'row'}">
        {{ message.username }}
      </div>
      <div class="message">
        <pre class="mb-0">{{ message.message }}</pre>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Message',
  props: ['message'],
  computed: {
    me () {
      return this.message.username === this.$route.query.username
    }
  }
}
</script>

<style scoped>
.avatar {
  width: 50px;
  height: 50px;
  border-radius: 25px;
  background-color: lightpink;
  display: flex;
  justify-content: center;
  align-items: center;
}
.message-box {
  padding: 10px;
}
.message {
  padding: 10px;
  border-radius: 10px;
  border: solid 1px #9fffff;
  background-color: lightcyan;
}
</style>
