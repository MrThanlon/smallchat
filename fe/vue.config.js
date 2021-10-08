module.exports = {
  devServer: {
    proxy: {
      '/room': {
        target: 'http://127.0.0.1:34783'
      },
      '/message': {
        target: 'http://127.0.0.1:34783',
        ws: true
      }
    }
  }
}
