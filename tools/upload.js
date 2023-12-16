import dotenv from 'dotenv'
import path from 'path'
import { connectAndAction, ensureFileExists, uploadFile } from './ftpHelper.js'

// Import data from .env file to process.env
dotenv.config()

// Module config values
const SYSMOD_NAME = process.env.SYSMOD_NAME ?? 'sys-helo'
const APP_ID = process.env.APP_ID ?? '0100000000000234'

// Useful local paths
const SYSMOD_FROM_DIR = path.join('..', 'sysmodule', 'out')
const OVERLAY_FROM_DIR = path.join('..', 'overlay', 'out')

// Use FTP to upload the latest things
function uploadEverything () {
  connectAndAction(async (client) => {
    // Upload config files (log directory)
    await ensureFileExists(client, 'log.flag', SYSMOD_FROM_DIR, path.join('/config', SYSMOD_NAME))

    // Upload sys module nsp
    const appPath = path.join('/atmosphere/contents', APP_ID)
    await uploadFile(client, SYSMOD_FROM_DIR, `${SYSMOD_NAME}.nsp`, appPath, 'exefs.nsp')

    // Set the boot flag
    await ensureFileExists(client, 'boot2.flag', SYSMOD_FROM_DIR, path.join(appPath, 'flags'))

    // Upload overlay
    await uploadFile(client, OVERLAY_FROM_DIR, 'overlay.ovl', '/switch/.overlays', `${SYSMOD_NAME}.ovl`, false)
  })
}

uploadEverything()
