const { sendMessage } = require('./send-message')

/**
 * Send a reply to a message
 * @param {string} text message
 * @param {string} channelId channel id
 * @param {string} messageId message to reply to
 * @returns {Promise<any>} api response
 */
function sendReplyMessage(text, channelId, messageId){
  const params = `${channelId};${messageId}`
  return sendMessage(text, params)
}

module.exports = { sendReplyMessage }
