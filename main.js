const { app, BrowserWindow } = require('electron')
const fs = require('fs')
const atob = require('atob')

const details = {}

function createWindow() {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      nodeIntegration: false,
      devTools: true
    }
  })

  win.webContents.openDevTools()

  win.webContents.session.clearCache()
  win.webContents.session.clearAuthCache()
  win.webContents.session.clearStorageData()

  // Pretend to be standard Chrome browser (otherwise, MS Teams will run Electron-specific code and fail to start)
  win.webContents.userAgent =
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.72 Safari/537.36'

  // win.loadFile('index.html')
  win.loadURL('https://teams.microsoft.com')

  win.webContents.on('will-redirect', (event, url) => {
    const match = url.match(/go#id_token=([^&]+)/)
    if (match) {
      details.idToken = match[1]
      details.oid = JSON.parse(atob(match[1].split('.')[1])).oid
    }
  })

  win.webContents.on('will-navigate', (event, url) => {
    console.log(`navigation wil go to to`, url)
  })

  win.webContents.on('did-stop-loading', (event) => {
    const timestamp = Date.now()
    win.webContents.executeJavaScript('window.localStorage')
      .then(result => {
        if (!details.oid) return
        const skypeToken = result[`ts.${details.oid}.auth.skype.token`]
        if (!skypeToken) return
        details.authSkype = JSON.parse(skypeToken)
        details.chatSvcAggToken = JSON.parse(result[`ts.${details.oid}.cache.token.https://chatsvcagg.teams.microsoft.com`])
        fs.writeFileSync('ms-teams-credentials.local.json', JSON.stringify(details))
        process.exit(0)
        // if (!fs.existsSync('output')) {
        //   fs.mkdirSync('output')
        // } else if (!fs.lstatSync('output').isDirectory()) {
        //   console.error('"output" is not a folder. Please move or remove it.')
        //   process.exit(1)
        // }
        // fs.writeFileSync(`output/local-storage-${timestamp}.json`, JSON.stringify(result))
      })
  })
}

app.whenReady().then(createWindow)

app.on('window-all-closed', () => {
  app.quit()
})

// Might not be needed since app will quit when all widnows closed
app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow()
  }
})
