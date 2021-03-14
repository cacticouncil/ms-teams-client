const axios = require('axios').default
const { loadCredentials } = require('../credentials')

// TODO: figure out what "003e8b17-0deb-4e57-a3d8-147e845a34bc" is
const url = 'https://amer.ng.msg.teams.microsoft.com/v2/users/ME/endpoints/003e8b17-0deb-4e57-a3d8-147e845a34bc'

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

async function getEndpoints() {
  const credentials = loadCredentials()
  const response = await axios.put(url, payload, {
    headers: {
      Authentication: `skypetoken=${credentials.authSkype.skypeToken}`
    }
  })

  return response.data
}

module.exports = {
  getEndpoints
}

if (require.main === module) {
  getEndpoints().then(console.log)
}
