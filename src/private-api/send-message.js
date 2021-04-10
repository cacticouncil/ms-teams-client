const axios = require('axios').default
const { loadCredentials } = require('../credentials')

const url = (params) => `https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/${params}/messages`

/**
 * Send message
 * @param {string} text message
 * @param {object} params chat id
 * @returns {Promise<any>} api response
 */
async function sendMessage(text, params) {
  // TODO: Get rid of this!!
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

module.exports = { sendMessage }
