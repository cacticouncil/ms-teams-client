const { getTeamsList } = require('@/private-api/list-teams')

describe('list-team', () => {
  test('sample', async () => {
    const response = await getTeamsList()

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
