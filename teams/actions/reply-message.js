const axios = require('axios').default
const { loadCredentials } = require('../credentials')

// "url": "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/19%3Aa0b9b7ae437c4d22b69a575393a55bdb%40thread.tacv2%3Bmessageid%3D1615603467529/messages",

const url = (sampleChannelId, messageId) => `https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/${sampleChannelId};${messageId}/messages`

async function sendMessageToChannel(text, channelId, messageId) {
  const credentials = loadCredentials()

  // plug 768076545871816000 into clientmessageid if you want to edit a message
  // optional fields: clientmessageid, imdisplayname
  const payload = JSON.parse(`{\"content\": \"${text}\",\"messagetype\":\"Text\",\"contenttype\":\"text\",\"amsreferences\":[], \"properties\":{\"importance\":\"\",\"subject\":null}}`)
  const headers = {
    headers: {
      'Content-Type': 'application/json',
      Authentication: `skypetoken=${credentials.authSkype.skypeToken}`
    }
  }

  const response = await axios.post(url(channelId, messageId), payload, headers)

  return `Your reply was successfully sent as: ${JSON.parse(response['config']['data'])['content']}`
}

if (require.main === module) {
  const sampleChannelId = `19:a0b9b7ae437c4d22b69a575393a55bdb@thread.tacv2`
  const messageId = `messageid=1615603467529`
  const text = 'This response was reverse engineered!'
  sendMessageToChannel(text, sampleChannelId, messageId)
    .then(response => console.log(response)).catch(err => console.log(err))
}