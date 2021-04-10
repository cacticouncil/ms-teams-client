const EventEmitter = require('events')
const atob = require('atob')
const { Tokens } = require('./tokens')
const { Poller } = require('./poller')
const { getTeamsList, getUsers, getChannelMessages } = require('../private-api')

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

    if (!(tokens && tokens.skypeToken && tokens.chatSvcAggToken)) throw new InvalidClientCredentialsError('Missing credentials. Cannot build Client object without ClientOptions containing skypeToken and chatSvcToken.')

    /** @type {Tokens} */
    this.tokens = null
    this.setTokens(tokens)

    this.poller = new Poller(this)
  }

  /**
   * Tokens object (jwt only)
   * @param {ClientOptions['tokens']} tokens
   */
  setTokens (tokens) {
    this.tokens = new Tokens({
      skypeToken: {
        token: tokens.skypeToken,
        exp: getExpirationFromJWT(tokens.skypeToken)
      },
      chatSvcAggToken: {
        token: tokens.chatSvcAggToken,
        exp: getExpirationFromJWT(tokens.chatSvcAggToken)
      }
    })
  }

  /**
   * Get teams in current account
   * @returns {Promise<any>} api response for teams
   */
  fetchTeams () {
    return getTeamsList({ tokens: this.tokens })
  }

  /**
   * Fetch information about multiple users
   * @param {string[]} users user ids
   * @returns {Promise<any>} api response for users
   */
  fetchUsers (userIds = []) {
    return getUsers({ tokens: this.tokens, userIds })
  }

  /**
   * Fetch channel messages
   * @param {string} teamId Team ID
   * @param {string} channelId Channel ID
   * @param {number} limit Number of top-level conversations
   * @returns api response
   */
  fetchChannelMessages (teamId, channelId, limit) {
    return getChannelMessages({ tokens: this.tokens, teamId, channelId, limit })
  }
}

/**
 * Error thrown when invalid credentials are given to client
 */
class InvalidClientCredentialsError extends Error {}

function getExpirationFromJWT(jwtToken) {
  const payload = JSON.parse(atob(jwtToken.split('.')[1]))
  return new Date(payload.exp * 1000).toISOString()
}

module.exports = {
  Client,
  InvalidClientCredentialsError
}
