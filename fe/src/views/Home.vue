<template>
  <div class="d-flex flex-column p-2 p-md-3">
    <h1>Welcome to SmallChat</h1>
    <div class="input-group mb-1">
      <span class="input-group-text w-25">Name</span>
      <input v-model="username" type="text" class="form-control" placeholder="Username" aria-label="Username">
    </div>
    <div class="input-group mb-1">
      <span class="input-group-text w-25">Room</span>
      <input v-model="roomName" type="text" class="form-control" placeholder="Room Name" aria-label="Room Name">
      <button class="btn btn-outline-dark"
              @click="roomName?$router.push(`/${roomName}?username=${username}`):0">Join</button>
    </div>
    <p class="mb-1">Or choose a room to join chat</p>
    <div class="col-12 p-0 m-0 d-flex flex-wrap justify-content-center">
      <div class="room-card card col-12 col-md-5 col-lg-3 m-1"
           @click="$router.push(`/${room.name}?username=${username}`)"
           v-for="room in roomList" :key="room.people">
        <div class="card-header">
          {{ room.name }}
        </div>
        <div class="card-body">
          People: {{ room.people }}
        </div>
      </div>
    </div>
  </div>
</template>

<script>
// import restful from 'restful-proxy'
const restful = require('restful-proxy')

export default {
  name: 'Home',
  data () {
    return {
      username: 'Someone',
      roomName: '',
      roomList: []
    }
  },
  methods: {
    getColor (name) {
      // map to color
      let hash = 0
      for (let i = 0; i < name.length; i++) {
        hash = name.charCodeAt(i) + ((hash << 5) - hash)
      }
      let color = '#'
      for (let i = 0; i < 3; i++) {
        const value = (hash >> (i * 8)) & 0xFF
        color += ('00' + value.toString(16)).substr(-2)
      }
      return color
    }
  },
  async mounted () {
    try {
      const res = await restful.room('get')
      if (res) {
        this.roomList = JSON.parse(res)
      } else {
        console.debug('failed to fetch content')
      }
    } catch (e) {
      console.debug(e)
    }
  }
}
</script>

<style scoped>
.room-card {
  cursor: pointer;
}
</style>
