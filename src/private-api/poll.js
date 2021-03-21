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

module.exports = { poll }
