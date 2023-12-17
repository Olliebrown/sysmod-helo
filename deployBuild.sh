#!/bin/bash

# Attempt to deploy to the switch via FTP
pushd tools
npm run deploy
popd
