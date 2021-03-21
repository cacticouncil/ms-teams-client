const { poll } = require('../private-api')
const { InvalidTokenError } = require('./tokens')

class Poller {
  /**
   *
   * @param {import('./client').Client} client Client instance
   */
  constructor (client) {
    this.client = client
    this.stop = true
  }

  start () {
    if (!this.stop) return
    this.stop = false
    this.__poll()
  }

  stop () {
    this.stop = true
  }

  __poll() {
    if (this.stop) return
    poll({ tokens: this.client.tokens })
      .catch(error => {
        console.log('hello world', error)
        if (error && ((error.response && error.response.status === 401) || error instanceof InvalidTokenError)) {
          this.client.emit('refresh-required')
          setTimeout(this.__poll, 5000)
        } else throw error
      })
      .then(response => {
        if (response.eventMessages) response.eventMessages.forEach((event) => {
          switch (event.type) {
            case 'EventMessage': {
              this.client.emit('event-message', event)
              break
            }
            default: {
              this.client.emit('event-unknown', event)
            }
          }
          // this.client.emit('poll', event)
        })

        return response.next
      })
      .then(next => {
        this.__poll(next)
      })
  }
}

module.exports = {
  Poller
}