const axios = require('axios').default
const { sendMessage } = require('./send-message')
//   let response;
//   // i would really prefer if we just didnt have to do this, so we need to figure out how to order the ids
//   // TODO: determine 'owner' of DM (?), assume first id is the DM initiator
//   try {
//     response = await axios.post(url(channelId, senderId, receiverId), payload, headers)
//   } catch (err) {
//     response = await axios.post(url(channelId, receiverId, senderId), payload, headers)
//   }
function sendDirectMessage(text, sampleChannelId, senderId, receiverId) {
  const params = `${sampleChannelId}:${receiverId}_${senderId}@unq.gbl.spaces`
  sendMessage(text, params)
    .then(response => console.log(response)).catch(err => console.log(err))
}

if (require.main === module) {
  // TODO: change names from senderId/receiverId to something that more
  // accurately reflects their purpose once we figure out why their order matters
  const senderId = "bd54250d-7a64-410a-aec9-aed8352723cc" // jon's id
  const receiverId = "227bf089-261b-463e-847d-51eb93c467f6" // blake's id
  const sampleChannelId = "19"
  const text = 'This DM was reverse engineered!'
  sendDirectMessage(text, sampleChannelId, senderId, receiverId)
}