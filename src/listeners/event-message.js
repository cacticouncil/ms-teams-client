/**
 * TODO: This needs better documentation!!
 */
function handler(event, client) {
  switch (event.resourceType) {
    case 'ConversationUpdate': {
      client.emit('reply-message', event)
      break
    }
    case 'NewMessage': {
      client.emit('new-message', event)
      break
    }
    case 'MessageUpdate': {
      client.emit('update-message', event)
      break
    }
    default: {
      console.log(`Unknown resource type: ${event.resourceType}`)
      break
    }
  }
}

module.exports = {
  handler
}
