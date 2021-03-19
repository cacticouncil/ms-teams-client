const axios = require('axios').default
const { loadCredentials } = require('../credentials')

const url = (params) => `https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/${params}/messages`

async function sendMessage(text, params) {
  const credentials = loadCredentials()
  const payload = {
    content: text,
    messagetype: 'Text',
    contenttype: 'text',
    asmreferences: [],
    properties: {
      importance: '',
      subject: null
    }
  }
  const headers = {
    headers: {
      Authentication: `skypetoken=${credentials.authSkype.skypeToken}`
    }
  }

  await axios.post(url(params), payload, headers)
  return `Your message was successfully sent as: ${text}`
}

if (require.main === module) {
  const sampleChannelId = '19:a0b9b7ae437c4d22b69a575393a55bdb@thread.tacv2'
  const text = 'This message was reverse engineered!'
  sendMessage(text, sampleChannelId)
    .then(response => console.log(response)).catch(err => console.log(err))
}

module.exports = {
  sendMessage
}