const fs = require('fs')
const path = require('path')
const atob = require('atob')
const { spawn } = require('child_process')

module.exports = async ({ rootDir }) => {
  const localCredentialsPath = path.join(rootDir, 'ms-teams-credentials.local.json')

  let expired = true

  if (fs.existsSync(localCredentialsPath)) {
    const contents = JSON.parse(fs.readFileSync(localCredentialsPath, { encoding: 'utf8' }))
    const id = JSON.parse(atob(contents.idToken.split('.')[1]))

    // check expiration of id token
    if (Date.now() < (id.exp * 1000)) {
      expired = false
    }
  }

  if (!expired) return

  // spawn process to show login window
  const loginProcess = spawn('npm', ["run", "login"], {
    cwd: path.resolve(rootDir)
  })

  // wait for login to complete
  await new Promise((resolve, reject) => {
    loginProcess.on('close', (code) => {
      if (code !== 0) {
        reject(new Error('Something went wrong while logging in.'))
      } else resolve()
    })
  })
}
