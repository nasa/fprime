const opener = require('opener')

const { URL } = require('url')

// attempt to open URL in web-browser, print address otherwise:
const open = async (npm, url, errMsg, isFile) => {
  url = encodeURI(url)
  const browser = npm.config.get('browser')

  function printAlternateMsg () {
    const json = npm.config.get('json')
    const alternateMsg = json
      ? JSON.stringify({
        title: errMsg,
        url,
      }, null, 2)
      : `${errMsg}:\n  ${url}\n`

    npm.output(alternateMsg)
  }

  if (browser === false) {
    printAlternateMsg()
    return
  }

  // We pass this in as true from the help command so we know we don't have to
  // check the protocol
  if (!isFile) {
    try {
      if (!/^https?:$/.test(new URL(url).protocol)) {
        throw new Error()
      }
    } catch (_) {
      throw new Error('Invalid URL: ' + url)
    }
  }

  const command = browser === true ? null : browser
  await new Promise((resolve, reject) => {
    opener(url, { command }, (err) => {
      if (err) {
        if (err.code === 'ENOENT') {
          printAlternateMsg()
        } else {
          return reject(err)
        }
      }
      return resolve()
    })
  })
}

module.exports = open
