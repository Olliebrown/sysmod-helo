import * as ftp from 'basic-ftp'
import fs from 'fs/promises'
import path from 'path'
import dotenv from 'dotenv'

// Import data from .env file to process.env
dotenv.config()

// Setup connection info
const SWITCH_CONFIG = {
  host: process.env.HOST ?? '127.0.0.1',
  port: process.env.PORT ?? 5000,
  user: process.env.USERNAME ?? 'baduser',
  password: process.env.PASSWORD ?? 'badpass',
  secure: false
}

// Log progress for any transfer from now on.
function logProgress (info) {
  process.stdout.clearLine(0)
  process.stdout.cursorTo(0)

  const kBytes = (info.bytes / 1024).toFixed(0)
  const kBytesOverall = (info.bytesOverall / 1024).toFixed(0)
  const percent = (info.bytes / info.bytesOverall * 100).toFixed(0)
  process.stdout.write(`> ${info.name} (${info.type}): ${kBytes}/${kBytesOverall} KB (${percent}%)`)

  if (info.bytesOverall !== 0 && info.bytes === info.bytesOverall) {
    process.stdout.write('\n')
  }
}

// Create a new empty file locally
async function touchFile (filename) {
  try {
    const fh = await fs.open(filename, 'a')
    await fh.close()
  } catch (err) {
    console.error('Error touching file:')
    console.log(err)
  }
}

// Connect to the FTP server and run the given callback
export async function connectAndAction (actionCallback) {
  // Create client
  const client = new ftp.Client()

  try {
    // Connect to Switch FTP server
    await client.access(SWITCH_CONFIG)

    // Run the action callback
    await actionCallback(client)
  } catch (err) {
    // Log any errors
    console.error('FTP Error:')
    console.log(err)
  } finally {
    // Ensure we are disconnected
    client.close()
  }
}

// Ensure a file exists on the destination with given name (and no contents)
export async function ensureFileExists (client, filename, fromDir, toDir, clearDir = false) {
  await client.ensureDir(toDir)
  if (clearDir) { await client.clearWorkingDir() }
  await touchFile(path.join(fromDir, filename))
  process.stdout.write(`> ${filename} (set flag)\n`)
  await client.uploadFrom(path.join(fromDir, filename), filename)
}

export async function ensureFileDoesNotExist (client, filename, toDir) {
  try {
    await client.remove(`${toDir}/${filename}`)
  } catch (err) {
    if (err.code !== 550) {
      throw err
    }
  }
}

// Upload a local file to the destination
export async function uploadFile (client, fromDir, fromFile, toDir, toFile, clearDir = false) {
  await client.ensureDir(toDir)
  if (clearDir) { await client.clearWorkingDir() }
  client.trackProgress(logProgress)
  await client.uploadFrom(path.join(fromDir, fromFile), toFile)
  client.trackProgress()
}

// Download a remote file to the destination
export async function downloadFile (client, fromDir, fromFile, toDir, toFile) {
  await client.cd(fromDir)
  client.trackProgress(logProgress)
  await client.downloadTo(path.join(toDir, toFile), fromFile)
  client.trackProgress()
}
