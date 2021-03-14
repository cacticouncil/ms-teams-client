import fs from 'fs'
import path from 'path'

export const CREDENTIALS_DEFAULT_LOCATION = path.join(__dirname, '../ms-teams-credentials.local.json')

let credentialsCache = null

export function loadCredentials (filePath = CREDENTIALS_DEFAULT_LOCATION) {
  if (credentialsCache) return credentialsCache

  const contents = fs.readFileSync(filePath, { encoding: 'utf8' })

  return credentialsCache = JSON.parse(contents)
}
