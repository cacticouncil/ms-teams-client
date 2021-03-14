import { getTeamsList } from '@/private-api/list-teams'
import { loadCredentials } from '../credentials'

describe('list-team', () => {
  test('sample', async () => {
    const credentials = loadCredentials()
    const response = await getTeamsList({ token: credentials.chatSvcAggToken.token })

    const output = []
    response.teams.forEach((team, index) => {
      if (index !== 0) output.push('')
      output.push(`${index + 1}. ${team.displayName}`)
      output.push(`       Description: ${team.description}`)
      output.push('       Channels:')
      team.channels.forEach((channel, index) => {
        output.push(`           ${index + 1}. ${channel.displayName} (id: ${channel.id})`)
      })
    })

    console.log(output.join('\n'))
  })
})
