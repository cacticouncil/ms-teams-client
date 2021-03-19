export class Tokens {
  tokenCache = {}

  constructor (tokens) {
    this.setTokens(tokens)
  }

  setTokens (tokens) {
    this.tokenCache = { ...this.tokenCache, tokens }
  }

  require (key) {
    // TODO: check expiration here
    if (this.tokenCache[key]) return this.tokenCache[key]
    else throw new InvalidTokenError(key)
  }
}

export class InvalidTokenError extends Error {
  constructor (key) {
    super(`Token "${key}" is invalid`)
  }
}
