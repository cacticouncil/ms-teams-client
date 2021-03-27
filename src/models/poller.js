const { poll } = require('../private-api')
const { InvalidTokenError } = require('./tokens')
const { handler: eventMessageHandler } = require('../listeners/event-message')

class Poller {
  /**
   *
   * @param {import('./client').Client} client Client instance
   */
  constructor(client) {
    this.client = client
    this.stop = true
  }

  start() {
    if (!this.stop) return
    this.stop = false
    this.__poll()
  }

  stop() {
    this.stop = true
  }

  __poll(next = null) {
    if (this.stop) return
    poll({ next, tokens: this.client.tokens })
      .catch(error => {
        if (error && ((error.response && error.response.status === 401) || error instanceof InvalidTokenError)) {
          this.client.emit('refresh-required')
          setTimeout(this.__poll, 5000)
        } else throw error
      })
      .then(response => {
        if (response.eventMessages) response.eventMessages.forEach((event) => {
          switch (event.type) {
            case 'EventMessage': {
              eventMessageHandler(event, this.client)
              break
            }
            default: {
              this.client.emit('event-unknown', event)
              break
            }
          }
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
