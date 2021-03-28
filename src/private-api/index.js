module.exports = {
  ...require('./poll-endpoints'),
  ...require('./poll'),
  ...require('./list-teams'),
  ...require('./get-users'),
  ...require('./get-channel-messages')
}
