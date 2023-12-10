import * as ftp from 'basic-ftp'
import fs from 'fs/promises'
import path from 'path'

const SWITCH_CONFIG = {
  host: '192.168.50.91',
  port: 5000,
  user: '',
  password: '',
  secure: false
}

const SYSMOD_FROM_DIR = path.join('..', 'sysmodule', 'out')
const OVERLAY_FROM_DIR = path.join('..', 'overlay', 'out')

async function touchFile (filename) {
  try {
    const fh = await fs.open(filename, 'a')
    await fh.close()
  } catch (err) {
    console.error('Error touching file:')
    console.log(err)
  }
}

async function uploadSysmod () {
  // Create client
  const client = new ftp.Client()

  try {
    // Connect to Switch FTP server
    await client.access(SWITCH_CONFIG)

    // Upload config files (log directory)
    await client.ensureDir('/config/sys-helo')
    await client.clearWorkingDir()
    await touchFile(path.join(SYSMOD_FROM_DIR, 'log.flag'))
    await client.uploadFrom(path.join(SYSMOD_FROM_DIR, 'log.flag'), 'log.flag')

    // Upload sys module nsp
    await client.ensureDir('/atmosphere/contents/0100000000000234')
    await client.clearWorkingDir()
    await client.uploadFrom(path.join(SYSMOD_FROM_DIR, 'sys-helo.nsp'), 'exefs.nsp')

    // Set the boot flag
    await client.ensureDir('/atmosphere/contents/0100000000000234/flags')
    await client.clearWorkingDir()
    await touchFile(path.join(SYSMOD_FROM_DIR, 'boot2.flag'))
    await client.uploadFrom(path.join(SYSMOD_FROM_DIR, 'boot2.flag'), 'boot2.flag')

    // Upload overlay
    await client.ensureDir('/switch/.overlays')
    await client.uploadFrom(path.join(OVERLAY_FROM_DIR, 'overlay.ovl'), 'sys-helo.ovl')
  } catch (err) {
    // Log any errors
    console.error('FTP Error:')
    console.log(err)
  } finally {
    // Ensure we are disconnected
    client.close()
  }
}

uploadSysmod()
