/**
 * @typedef {Object} Token Expected format for token
 * @property {string} token
 * @property {string} exp
 */

/**
 * Class that contains cache of tokens
 */
class Tokens {
  constructor (tokens) {
    /** @type {Record<string, Token>} */
    this.tokenCache = {}

    this.setTokens(tokens)
  }

  setTokens (tokens) {
    this.tokenCache = { ...this.tokenCache, tokens }
  }

  /**
   * Retrieve token and throw exception if something wrong
   * @param {string} key Key of token
   * @returns {string} Token, if cached
   * @throws if token does not exist or is expired
   */
  require (key) {
    if (!this.tokenCache[key]) throw new InvalidTokenError(key)

    const obj = this.tokenCache[key]

    // expiration check
    if (new Date(obj.exp) < new Date()) throw new InvalidTokenError(key)

    return obj.token
  }
}

class InvalidTokenError extends Error {
  constructor (key) {
    super(`Token "${key}" is invalid`)
  }
}

module.exports = {
  default: Tokens,
  InvalidTokenError
}
