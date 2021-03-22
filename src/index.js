const { Client } = require('./models/client')
const { loadCredentials } = require('./credentials')
const { ListenerHelper } = require('./listeners/listener-helper')
class ClientProvider {
  constructor() {
    const credentials = loadCredentials()
    this.client = new Client({
      tokens: {
        skypeToken: credentials.authSkype.skypeToken,
        chatSvcAggToken: credentials.chatSvcAggToken.token
      }
    })
  }

  getClient() {
    return this.client
  }
}

const clientProvider = new ClientProvider()
Object.freeze(clientProvider)

ListenerHelper.registerAll(clientProvider.getClient())

clientProvider.getClient().poller.start()

clientProvider.getClient().get

module.exports = {
  clientProvider
}
