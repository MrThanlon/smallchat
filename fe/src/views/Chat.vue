<template>
  <div class="col-12 p-2 p-md-3 d-flex flex-column">
    <header class="mb-1 d-flex align-items-center justify-content-between">
      <button class="btn btn-outline-dark" @click="$router.push('/')">Back</button>
      <div class="status d-flex align-items-center">
        <div class="m-1"
             :style="{backgroundColor: statusColor[ws.readyState]}"
             style="width: 10px;height: 20px;border-radius: 10px"></div>
        <span>{{ statusText[ws.readyState] }}</span>
      </div>
    </header>
    <div class="chat d-flex flex-column align-content-between">
      <div class="chat-flow col-12 h-75 mb-1 border border-dark rounded p-1">
      </div>
      <div class="chat-input col-12 h-25
       d-flex flex-column justify-content-center align-content-start">
        <textarea class="col-12 mb-1 p-1 flex-grow-1 border-dark rounded" v-model="message"></textarea>
        <button class="btn btn-outline-dark" @click="sendMessage">Send</button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Chat',
  data () {
    return {
      ws: new WebSocket(`ws://${window.location.host}/message`),
      // ws: new WebSocket('ws://127.0.0.1:34783/message'),
      message: '',
      statusText: ['Connecting...', 'Online', 'Offline', 'Offline'],
      statusColor: ['#FFE032', '#64FF32', '#808080FF', '#808080FF']
    }
  },
  created () {
    const roomName = this.$route.params.name
    const username = this.$route.query.username
    // this.ws = new WebSocket('ws://127.0.0.1:34783/message')
    // open ws
    this.ws.onerror = e => {
      this.$forceUpdate()
      console.debug('[ws] error', e)
    }
    this.ws.onmessage = d => {
      console.debug('[ws] data', d)
    }
    this.ws.onclose = () => {
      this.$forceUpdate()
      console.debug('[ws] closed')
    }
    this.ws.onopen = () => {
      // connected
      this.$forceUpdate()
      console.debug('[ws] open')
      this.ws.send(JSON.stringify({ username, roomName }))
    }
  },
  mounted () {
  },
  methods: {
    sendMessage () {
      this.ws.send(this.message)
      this.message = ''
    }
  }
}
</script>

<style scoped>
.chat {
  height: 480px;
}
.status {
  width: 120px;
}
</style>
