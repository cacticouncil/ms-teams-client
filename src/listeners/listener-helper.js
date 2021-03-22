const { handler: eventMessageHandler } = require('./event-message')
class ListenerHelper {
  static eventHandlers = []
  /**
  * @param {string} eventName the event name to register
  * @param {Function} eventHandler the event handler to register
  */
  static addEvent(eventName, eventHandler) {
    this.eventHandlers.push({ eventName, eventHandler })
  }

  static addAllEvents() {
    console.log('adding an event...')
    ListenerHelper.addEvent('event-message', eventMessageHandler)
  }

  static registerAll(client) {
    ListenerHelper.addAllEvents()
    ListenerHelper.eventHandlers.forEach(({ eventName, eventHandler }) => client.on(eventName, eventHandler))

    console.log('done with registering')
  }
}

module.exports = { ListenerHelper }