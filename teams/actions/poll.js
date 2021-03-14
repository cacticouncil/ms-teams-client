const axios = require('axios').default
const { loadCredentials } = require('../credentials')
const { getEndpoints } = require('./poll-endpoints')

async function poll(next = null) {
  const credentials = loadCredentials()
  const url = next || (await getEndpoints()).subscriptions.find((subscription) => subscription.channelType === 'HttpLongPoll').longPollUrl

  const response = await axios.get(url, {
    headers: {
      Authentication: `skypetoken=${credentials.authSkype.skypeToken}`
    }
  })

  return response.data
}

if (require.main === module) {
  process.on('SIGINT', () => {
    console.log("\nSIGINT: stopping...")
    process.exit(0)
  })

  function pollLoop(next = null) {
    poll(next).then(response => {
      console.log(new Date().toISOString())
      console.log(response)
      pollLoop(response.next)
    })
  }

  pollLoop()
}
