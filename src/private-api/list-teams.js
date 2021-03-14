import axios from 'axios'

const url = 'https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true'

/**
 * Retrieve list of teams
 * @param {Object} options
 * @param {string} options.token ChatSvcAggToken
 * @returns API response
 */
export async function getTeamsList({ token }) {
  const response = await axios.get(url, {
    headers: {
      Authorization: `Bearer ${token}`
    }
  })

  return response.data
}
