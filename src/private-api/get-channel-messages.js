const axios = require('axios').default

const url = (teamId, channelId, limit) => `https://teams.microsoft.com/api/csa/api/v2/teams/${teamId}/channels/${channelId}?pageSize=${limit}${teamId === channelId ? '&filterSystemMessage=true' : ''}`

/**
 * Get messages from channel
 * @param {object} options options
 * @param {object} options.tokens
 * @param {string} options.teamId
 * @param {string} options.channelId
 * @param {number} options.limit
 * @returns api response
 */
async function getChannelMessages(options = {}) {
  const { tokens, teamId, channelId, limit = 5 } = options
  const response = await axios.get(url(teamId, channelId, limit), {
    headers: {
      Authorization: `Bearer ${tokens.get('chatSvcAggToken')}`
    }
  })

  return response.data.replyChains
}

module.exports = { getChannelMessages }
