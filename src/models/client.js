const EventEmitter = require('events')
const atob = require('atob')
const { Tokens } = require('./tokens')
const { Poller } = require('./poller')

/**
 * @typedef {object} ClientOptions
 * @property {object} tokens
 * @property {string} tokens.skypeToken
 * @property {string} tokens.chatSvcAggToken
 */

/**
 * Teams Client
 */
class Client extends EventEmitter {
  /**
   * @param {ClientOptions} options client options
   */
  constructor(options = {}) {
    super()
    const { tokens } = options
    // console.log(tokens.)
    // console.log('OPTIONS', tokens)
    if (!(tokens && tokens.skypeToken && tokens.chatSvcAggToken)) throw new InvalidClientCredentialsException('Missing credentials. Cannot build Client object without ClientOptions containing skypeToken and chatSvcToken.')

    /** @type {Tokens} */
    this.tokens = new Tokens({
      skypeToken: {
        token: tokens.skypeToken,
        exp: getExpirationFromJWT(tokens.skypeToken)
      },
      chatSvcToken: {
        token: tokens.chatSvcAggToken,
        exp: getExpirationFromJWT(tokens.chatSvcAggToken)
      }
    })

    this.poller = new Poller(this)
  }
}

class InvalidClientCredentialsException extends Error {
  constructor(msg) {
    super(msg)
  }
}

function getExpirationFromJWT(jwtToken) {
  const payload = JSON.parse(atob(jwtToken.split('.')[1]))
  return new Date(payload.exp * 1000).toISOString()
}

module.exports = {
  Client,
  InvalidClientCredentialsException
}
