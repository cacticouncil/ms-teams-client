const axios = require('axios').default
const { loadCredentials } = require('../credentials')

const url = (sampleChannelId) => `https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/${sampleChannelId}/messages`

async function sendMessageToChannel(text, channelId) {
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

  const response = await axios.post(url(channelId), payload, headers)

  return `Your message was successfully sent as: ${JSON.parse(response['config']['data'])['content']}`
}

if (require.main === module) {
  const sampleChannelId = `19:a0b9b7ae437c4d22b69a575393a55bdb@thread.tacv2`
  const text = 'This message was reverse engineered!'
  sendMessageToChannel(text, sampleChannelId)
    .then(response => console.log(response)).catch(err => console.log(err))
}