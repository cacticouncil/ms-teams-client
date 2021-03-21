const axios = require('axios').default
const { v4: uuid } = require('uuid')

// didn't know what guid they wanted... so did the next best thing
const url = () => `https://amer.ng.msg.teams.microsoft.com/v2/users/ME/endpoints/${uuid()}`

const payload = {
  startingTimeSpan: 0,
  endpointFeatures: 'Agent,Presence2015,MessageProperties,CustomUserProperties,NotificationStream,SupportsSkipRosterFromThreads',
  subscriptions: [
    {
      channelType: 'HttpLongPoll',
      interestedResources: ['/v1/users/ME/conversations/ALL/properties', '/v1/users/ME/conversations/ALL/messages', '/v1/threads/ALL']
    }
  ]
}

async function getPollingEndpoint(options = {}) {
  const { tokens } = options

  const response = await axios.put(url(), payload, {
    headers: {
      Authentication: `skypetoken=${tokens.get('skypeToken')}`
    }
  })

  return response.data
}

module.exports = {
  getPollingEndpoint
}

if (require.main === module) {
  getPollingEndpoint().then(console.log)
}
