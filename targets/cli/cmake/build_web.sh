#!/usr/bin/env bash
set -e
cd ../../web
npm run build
rm -rf ../targets/cli/build/web
cp -R dist ../targets/cli/build/web