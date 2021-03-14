import axios from 'axios'
import { sendMessage } from './send-message'

export function sendReplyMessage(text, channelId, messageId){
  const params = `${channelId};${messageId}`
  return sendMessage(text, params)
}

if (require.main === module) {
  const sampleChannelId = `19:a0b9b7ae437c4d22b69a575393a55bdb@thread.tacv2`
  const messageId = `messageid=1615603467529`
  const text = 'This response was reverse engineered!'
  sendReplyMessage(text, sampleChannelId, messageId)
    .then(response => console.log(response)).catch(err => console.log(err))
}
