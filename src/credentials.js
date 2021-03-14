// TODO: remove this file
// Currently kept for compatiblity

const fs = require('fs')
const path = require('path')

const CREDENTIALS_DEFAULT_LOCATION = path.join(__dirname, '../ms-teams-credentials.local.json')

let credentialsCache = null

function loadCredentials (filePath = CREDENTIALS_DEFAULT_LOCATION) {
  if (credentialsCache) return credentialsCache

  const contents = fs.readFileSync(filePath, { encoding: 'utf8' })

  return credentialsCache = JSON.parse(contents)
}

module.exports = {
  CREDENTIALS_DEFAULT_LOCATION,
  loadCredentials
}
