function handler(event) {
  console.log('\n====================START HANDLE MESSAGE====================')
  // console.log('EVENT:', event)
  switch (event.resourceType) {
    case 'ConversationUpdate': {
      handleReply(event)
      break
    }
    case 'NewMessage': {
      const threadType = event.resource.threadtype

      if (threadType === 'topic') {
        console.log('in topic')
        handleNewMessage(event)
      } else if (threadType === 'streamofnotifications') {
        console.log('in notificationstream')
        handleMessageNotification(event)
      }
      break
    }
    case 'MessageUpdate': {
      handleMessageUpdate(event)
      break
    }
    default: {
      console.log('Unknown resource type')
      break
    }
  }
  console.log('====================END HANDLE MESSAGE====================\n')
}
//threadtype might be a good way to differentiate between poll responses that come back with the same resourceType, but do different things
//for example, a notification might come back as resourceType=NewMessage but have threadtype=streamofnotifications
//while a regular will have the same resourceType but threadtype=topic

// resourceType = NewMessage
function handleNewMessage(event) {
  console.log('User', event.resource.imdisplayname, 'sent a new message:', event.resource.content)
}

function handleMessageNotification(event) {
  console.log('You received a message notification! Ding!')
}

//whenever a message is updated, such as editing/reacting/deleting/undoing/creating polls/poll voting/pinning
function handleMessageUpdate(event) {
  console.log('Message was updated boi or edited')
}

// resourceType = ConversationUpdate
function handleReply(event) {
  console.log('User', event.resource.imdisplayname, 'sent a new reply:', event.resource.content)
}

module.exports = {
  handler
}