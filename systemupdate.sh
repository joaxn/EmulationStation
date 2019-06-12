#!/bin/sh
esdir="$(dirname $0)"
spacer="------------------------------------------\n"
pl="----  "
nl="\n"

echo $spacer
echo $pl"Updating Setup Script"$nl
echo $spacer

cd /home/pi/Retropie-Setup/
git pull >/dev/null