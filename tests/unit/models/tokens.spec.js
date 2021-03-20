const { Tokens } = require('@/models/tokens')

describe('tokens', () => {
  /** @type {Tokens} */
  let tokens

  beforeEach(() => {
    tokens = new Tokens({
      foobar: {
        token: '1234',
        exp: new Date(Date.now() + 600000).toISOString()
      }
    })
  })

  it('caches tokens', () => {
    expect(tokens.get('foobar')).toBe('1234')
  })

  it('throws error if invalid key', () => {
    expect(() => tokens.get('foobaz')).toThrow()
  })

  it('throws if token expired', () => {
    tokens.setTokens({
      foobar: {
        token: '1234',
        exp: new Date(Date.now() - 1000).toISOString()
      }
    })

    expect(() => tokens.get('foobar')).toThrow()
  })
})
