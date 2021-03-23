const { Client: MSTeamsClient } = require('./models/client')
const { handler: eventMessageHandler } = require('./listeners/event-message')

module.exports = {
  MSTeamsClient,
  eventMessageHandler
}
