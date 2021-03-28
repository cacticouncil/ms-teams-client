const axios = require('axios').default

const url = 'https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true'

/**
 * Get object with teams and chats in account
 * @param {object} options Options
 * @param {object} options.tokens tokens object in client
 * @returns {object} teams and chat data
 */
async function getTeamsList(options = {}) {
  const { tokens } = options
  const response = await axios.get(url, {
    headers: {
      Authorization: `Bearer ${tokens.get('chatSvcAggToken')}`
    }
  })

  return response.data
}

module.exports = { getTeamsList }
