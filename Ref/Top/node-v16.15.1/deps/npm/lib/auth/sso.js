// XXX: To date, npm Enterprise Legacy is the only system that ever
// implemented support for this type of login.  A better way to do
// SSO is to use the WebLogin type of login supported by the npm-login
// module.  This more forward-looking login style is, ironically,
// supported by the '--auth-type=legacy' type of login.
// When and if npm Enterprise Legacy is no longer supported by the npm
// CLI, we can remove this, and fold the lib/auth/legacy.js back into
// lib/adduser.js

const profile = require('npm-profile')
const npmFetch = require('npm-registry-fetch')
const log = require('../utils/log-shim')
const openUrl = require('../utils/open-url.js')
const otplease = require('../utils/otplease.js')

const pollForSession = ({ registry, token, opts }) => {
  log.info('adduser', 'Polling for validated SSO session')
  return npmFetch.json(
    '/-/whoami', { ...opts, registry, forceAuth: { token } }
  ).then(
    ({ username }) => username,
    err => {
      if (err.code === 'E401') {
        return sleep(opts.ssoPollFrequency).then(() => {
          return pollForSession({ registry, token, opts })
        })
      } else {
        throw err
      }
    }
  )
}

function sleep (time) {
  return new Promise((resolve) => setTimeout(resolve, time))
}

const login = async (npm, { creds, registry, scope }) => {
  const opts = { ...npm.flatOptions, creds, registry, scope }
  const { ssoType } = opts

  if (!ssoType) {
    throw new Error('Missing option: sso-type')
  }

  // We're reusing the legacy login endpoint, so we need some dummy
  // stuff here to pass validation. They're never used.
  const auth = {
    username: 'npm_' + ssoType + '_auth_dummy_user',
    password: 'placeholder',
    email: 'support@npmjs.com',
    authType: ssoType,
  }

  const { token, sso } = await otplease(opts,
    opts => profile.loginCouch(auth.username, auth.password, opts)
  )

  if (!token) {
    throw new Error('no SSO token returned')
  }
  if (!sso) {
    throw new Error('no SSO URL returned by services')
  }

  await openUrl(npm, sso, 'to complete your login please visit')

  const username = await pollForSession({ registry, token, opts })

  log.info('adduser', `Authorized user ${username}`)

  const scopeMessage = scope ? ' to scope ' + scope : ''
  const message = `Logged in as ${username}${scopeMessage} on ${registry}.`

  return {
    message,
    newCreds: { token },
  }
}

module.exports = login
