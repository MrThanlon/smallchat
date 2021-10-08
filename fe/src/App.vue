<template>
  <router-view/>
  <footer>
      <span>
        Power
        by
        &copy;<a target="_blank" href="https://github.com/MrThanlon/smallchat">SmallChat</a>
      </span>
  </footer>
</template>

<style>
body {
  padding: 0 !important;
  margin: 0 !important;
}

footer {
  position: fixed;
  bottom: 3px;
  display: flex;
  width: 100%;
  height: 1.2rem;
  justify-content: center;
}
</style>
