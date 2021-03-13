const axios = require('axios').default
const { loadCredentials } = require('../credentials')

// not sure on the ordering of receiver vs sender id's, we think it has to do with who starts the DM
// that person's id will be first in the URL. Current work around try catching error, reversing params and trying again
const url = (sampleChannelId, senderId, receiverId) => `https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/${sampleChannelId}:${receiverId}_${senderId}@unq.gbl.spaces/messages`

async function sendDirectMessage(text, channelId, senderId, receiverId) {
  const credentials = loadCredentials()

  const payload = JSON.parse(`{\"content\": \"${text}\",\"messagetype\":\"Text\",\"contenttype\":\"text\",\"amsreferences\":[], \"properties\":{\"importance\":\"\",\"subject\":null}}`)
  const headers = {
    headers: {
      'Content-Type': 'application/json',
      Authentication: `skypetoken=${credentials.authSkype.skypeToken}`
    }
  }

  let response;
  // i would really prefer if we just didnt have to do this, so we need to figure out how to order the ids
  // TODO: determine 'owner' of DM (?), assume first id is the DM initiator
  try {
    response = await axios.post(url(channelId, senderId, receiverId), payload, headers)
  } catch (err) {
    response = await axios.post(url(channelId, receiverId, senderId), payload, headers)
  }

  return `Your reply was successfully sent as: ${JSON.parse(response['config']['data'])['content']}`
}

if (require.main === module) {
  const senderId = "bd54250d-7a64-410a-aec9-aed8352723cc" // jon's id
  const receiverId = "227bf089-261b-463e-847d-51eb93c467f6" // blake's id
  const sampleChannelId = "19"
  const text = 'This DM was reverse engineered!'
  sendDirectMessage(text, sampleChannelId, senderId, receiverId)
    .then(response => console.log(response)).catch(err => console.log(err))
}