const axios = require('axios').default
const { loadCredentials } = require('../../credentials')
//"url": "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/19%3Aa0b9b7ae437c4d22b69a575393a55bdb%40thread.tacv2/messages/1615603432377/properties?name=emotions",

const url = (sampleChannelId, messageId) => `https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/${sampleChannelId}/messages/${messageId}/properties?name=emotions`

async function sendEmotion(channelId, messageId, key) {
  const credentials = loadCredentials()

  const payload = {
    emotions: {
      key: key,
      value: messageId
    }
  }

  const headers = {
    headers: {
      'Content-Type': 'application/json',
      Authentication: `skypetoken=${credentials.authSkype.skypeToken}`
    }
  }

  const response = await axios.put(url(channelId, messageId), payload, headers)

  return `Successfully updated message with emotion data: ${response['config']['data']}`
}

if (require.main === module) {
  const sampleChannelId = `19:a0b9b7ae437c4d22b69a575393a55bdb@thread.tacv2`
  const messageId = `1615603432377`
  const key = 'like' // emotion name (e.g. like, heart)
  sendEmotion(sampleChannelId, messageId, key)
    .then(response => console.log(response)).catch(err => console.log(err))
}