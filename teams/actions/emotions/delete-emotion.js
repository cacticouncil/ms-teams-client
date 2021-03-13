const axios = require('axios').default
const { loadCredentials } = require('../../credentials')
//"url": "https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/19%3Aa0b9b7ae437c4d22b69a575393a55bdb%40thread.tacv2/messages/1615603432377/properties?name=emotions",

const url = (sampleChannelId, messageId) => `https://amer.ng.msg.teams.microsoft.com/v1/users/ME/conversations/${sampleChannelId}/messages/${messageId}/properties?name=emotions`

async function sendEmotion(channelId, messageId, value) {
  const credentials = loadCredentials()

  const payload = {
    emotions: {
      key: value,
    }
  }

  // TODO: why is the authentication failing for delete request?
  const headers = {
    headers: {
      'Content-Type': 'application/json',
      Authentication: `skypetoken=${credentials.authSkype.skypeToken}`,
      'origin': 'https://teams.microsoft.com',
      'referer': 'https://teams.microsoft.com/',
      'x-ms-client-caller': 'removeMessageReaction',
      'x-ms-session-id': 'd38abc4d-7aa2-4a76-bf20-8d1e08d4dfeb',
      'sec-fetch-mode': 'cors',
      'sec-fetch-site': 'same-site',
      'x-ms-client-version': '1415/1.0.0.2021030227',
      'user-agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.82 Safari/537.36',
      'clientinfo': 'os=windows; osVer=10; proc=x86; lcid=en-us; deviceType=1; country=us; clientName=skypeteams; clientVer=1415/1.0.0.2021030227; utcOffset=-05:00; timezone=America/New_York',
      'behavioroverride': 'redirectAs404',
      'sec-ch-ua-mobile': '?0',
      'sec-fetch-dest': 'empty',
      'sec-ch-ua': '"Google Chrome";v="89", "Chromium";v="89", ";Not A Brand";v="99"',
      'x-ms-client-env': 'pds-prod-azsc-ussc-02',
      'x-ms-user-type': 'null'
    }
  }

  const response = await axios.delete(url(channelId, messageId), payload, headers)

  return `Successfully deleted emotion with emotion data: ${response['config']['data']}`
}

if (require.main === module) {
  const sampleChannelId = `19:a0b9b7ae437c4d22b69a575393a55bdb@thread.tacv2`
  const messageId = `1615603432377`
  const key = 'like' // emotion name (e.g. like, heart)
  sendEmotion(sampleChannelId, messageId, key)
    .then(response => console.log(response)).catch(err => console.log(err))
}