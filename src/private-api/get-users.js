const axios = require('axios').default

const url = 'https://teams.microsoft.com/api/mt/part/amer-02/beta/users/fetchShortProfile?isMailAddress=false&enableGuest=true&includeIBBarredUsers=true&skypeTeamsInfo=true'

/**
 * Get information for multiple users
 * @param {object} options options
 * @param {Tokens} options.tokens
 * @param {string[]} options.userIds user ids
 */
function getUsers(options = {}) {
  const { tokens, userIds } = options

  return axios.post(url, userIds, {
    headers: {
      Authorization: `Bearer ${tokens.get('chatSvcAggToken')}`
    }
  })
    .then(response => response.data.value)
}

module.exports = { getUsers }
