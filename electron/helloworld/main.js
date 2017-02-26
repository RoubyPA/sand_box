const {app, BrowserWindow}  = require('electron')
const path  = require('path')
const url   = require('url')

let win

function createWindow () {
  /* Create new window (type browser) */
  win = new BrowserWindow({width: 800, height: 600})

  /* Set window url */
  win.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes : true
  }))

  /* Open dev tools */
  win.webContents.openDevTools()

  /* When window is closed */
  win.on('closed', () => {
    win = null
  })
}

/* Call function createWindow when app is initialized */
app.on('ready', createWindow)

/* Exit process when window is closed */
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', () => {
  if (win === null) {
    createWindow()
  }
})
