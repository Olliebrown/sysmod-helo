import dotenv from 'dotenv'
import path from 'path'
import {
  connectAndAction, downloadFile, ensureFileExists, ensureFileDoesNotExist, uploadFile
} from './ftpHelper.js'

// Import data from .env file to process.env
dotenv.config()

// Module config values
const SYSMOD_NAME = process.env.SYSMOD_NAME ?? 'sys-helo'
const APP_ID = process.env.APP_ID ?? '0100000000000234'
const LOG_FLAG = (process.env.LOG_FLAG ?? 'true') === 'true'
const BOOT_FLAG = (process.env.BOOT_FLAG ?? 'false') === 'true'

// Useful local paths
const SYSMOD_ROOT_DIR = path.join('..', 'sysmodule')
const SYSMOD_FROM_DIR = path.join(SYSMOD_ROOT_DIR, 'out')
const OVERLAY_FROM_DIR = path.join('..', 'overlay', 'out')

// Use FTP to upload all the latest things
export function uploadEverything () {
  connectAndAction(async (client) => {
    // Set logging flag if enabled
    if (LOG_FLAG) {
      await ensureFileExists(client, 'log.flag', SYSMOD_FROM_DIR, `/config/${SYSMOD_NAME}`, true)
    } else {
      await ensureFileDoesNotExist(client, 'log.flag', `/config/${SYSMOD_NAME}`)
    }

    // Upload sys module nsp
    const appPath = `/atmosphere/contents/${APP_ID}`
    await uploadFile(client, SYSMOD_FROM_DIR, `${SYSMOD_NAME}.nsp`, appPath, 'exefs.nsp', true)

    // Upload sys module config
    await uploadFile(client, SYSMOD_ROOT_DIR, 'toolbox.json', appPath, 'toolbox.json')

    // Set/Unset the boot flag
    if (BOOT_FLAG) {
      await ensureFileExists(client, 'boot2.flag', SYSMOD_FROM_DIR, `${appPath}/flags`)
    } else {
      await ensureFileDoesNotExist(client, 'boot2.flag', `${appPath}/flags`)
    }

    // Upload overlay
    await uploadFile(client, OVERLAY_FROM_DIR, 'overlay.ovl', '/switch/.overlays', `${SYSMOD_NAME}.ovl`)
  })
}

// Use FTP to download the latest logs
export function downloadLogs () {
  connectAndAction(async (client) => {
    // Download Log files
    try {
      await downloadFile(client, `/config/${SYSMOD_NAME}`, 'log.txt', './', 'log.txt')
    } catch (err) {
      if (err.code === 450) {
        console.log('No log file found')
      } else {
        throw err
      }
    }
  })
}
