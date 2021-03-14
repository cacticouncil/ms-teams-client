import axios from 'axios'
import { loadCredentials } from '../credentials'

const url = 'https://teams.microsoft.com/api/csa/api/v1/teams/users/me?isPrefetch=false&enableMembershipSummary=true'

export async function getTeamsList() {
  const credentials = loadCredentials()
  const response = await axios.get(url, {
    headers: {
      Authorization: `Bearer ${credentials.chatSvcAggToken.token}`
    }
  })

  return response.data
}

if (require.main === module) {
  getTeamsList().then(response => {
    response.teams.forEach((team, index) => {
      if (index !== 0) console.log()
      console.log(`${index + 1}. ${team.displayName}`)
      console.log(`       Description: ${team.description}`)
      console.log('       Channels:')
      team.channels.forEach((channel, index) => {
        console.log(`           ${index + 1}. ${channel.displayName} (id: ${channel.id})`)
      })
    })
  })
}
