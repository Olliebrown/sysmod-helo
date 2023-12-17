#!/bin/bash

# Attempt to download logs from the switch via FTP
pushd tools
npm run logs
popd
