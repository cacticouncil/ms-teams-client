const { sendMessage } = require('./send-message')

/**
 * Send a direct message
 * @param {string} text message
 * @param {string} senderId sender id
 * @param {string} receiverId receiver id
 * @returns {Promise<any>} api response
 */
function sendDirectMessage(text, senderId, receiverId) {
  const params = `19:${receiverId}_${senderId}@unq.gbl.spaces`
  return sendMessage(text, params)
}

module.exports = { sendDirectMessage }
