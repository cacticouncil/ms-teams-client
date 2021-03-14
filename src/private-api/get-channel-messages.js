import axios from 'axios'
import htmlparser from 'htmlparser2'
import { loadCredentials } from '../credentials'

const url = (teamId, channelId) => `https://teams.microsoft.com/api/csa/api/v2/teams/${teamId}/channels/${channelId}?filterSystemMessage=true&pageSize=5`

export async function getChannelMessages(teamId, channelId) {
  const credentials = loadCredentials()
  const response = await axios.get(url(teamId, channelId), {
    headers: {
      Authorization: `Bearer ${credentials.chatSvcAggToken.token}`
    }
  })

  return response.data
}

if (require.main === module) {
  const sampleChannelId = `19:e6d413c6361f49feae894f81f0a85264@thread.tacv2`

  getChannelMessages(sampleChannelId, sampleChannelId)
    .then(response => {
      response.replyChains.forEach((replyChain, index) => {
        if (index !== 0) console.log('=======================================')
        replyChain.messages.forEach(msg => {
          let text
          switch (msg.messageType) {
            case 'Text': {
              text = msg.content
              break
            }
            case 'RichText/Html': {
              text = []
              const parser = new htmlparser.Parser({
                ontext(piece) {
                  text.push(piece)
                }
              })
              parser.write(msg.content)
              parser.end()
              text = text.join('')
              break
            }
            default: {
              text = `Unknown message type (${msg.messageType}): ${msg.content}`
            }
          }
          if (msg.properties.deletetime) text = "[deleted message]"

          console.log(`[${msg.composeTime}] ${msg.imDisplayName}: ${text}`)
        })
      })
    })
}
