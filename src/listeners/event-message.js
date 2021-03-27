function handler(event, client) {
  switch (event.resourceType) {
    case 'ConversationUpdate': {
      client.emit('reply-message', event)
      break
    }
    case 'NewMessage': {
      const threadType = event.resource.threadtype

      if (threadType === 'topic') {
        client.emit('new-message', event)
      } else if (threadType === 'streamofnotifications') {
        client.emit('message-notification-stream', event)
      }
      break
    }
    case 'MessageUpdate': {
      client.emit('update-message', event)
      break
    }
    default: {
      console.log('Unknown resource type')
      break
    }
  }
}

module.exports = {
  handler
}
