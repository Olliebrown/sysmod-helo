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

// Ensure a file exists on the destination with given name (an no contents)
export async function ensureFileExists (client, filename, fromDir, toDir) {
  await client.ensureDir(toDir)
  await client.clearWorkingDir()
  await touchFile(path.join(fromDir, filename))
  await client.uploadFrom(path.join(fromDir, filename), filename)
}

// Upload a local file to the destination
export async function uploadFile (client, fromDir, fromFile, toDir, toFile, clearDir = true) {
  await client.ensureDir(toDir)
  if (clearDir) { await client.clearWorkingDir() }
  await client.uploadFrom(path.join(fromDir, fromFile), toFile)
}

// Download a remote file to the destination
export async function downloadFile (client, fromDir, fromFile, toDir, toFile) {
  await client.cd(fromDir)
  await client.downloadTo(path.join(toDir, toFile), fromFile)
}
