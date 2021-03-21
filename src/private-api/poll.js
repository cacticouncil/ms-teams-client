const axios = require('axios').default
const { getPollingEndpoint } = require('./poll-endpoints')

async function poll(options) {
  const { next = null, tokens } = options
  const url = next || (await getPollingEndpoint({ tokens })).subscriptions.find((subscription) => subscription.channelType === 'HttpLongPoll').longPollUrl

  const response = await axios.get(url, {
    headers: {
      Authentication: `skypetoken=${tokens.get('skypeToken')}`
    }
  })

  return response.data
}

function __pollLoop(next = null) {
  poll(next).then(response => {
    console.log(new Date().toISOString())
    console.log(response)
    __pollLoop(response.next)
  })
}

if (require.main === module) {
  process.on('SIGINT', () => {
    console.log('\nSIGINT: stopping...')
    process.exit(0)
  })

  __pollLoop()
}

module.exports = { poll }
