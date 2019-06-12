#!/bin/sh

esdir="$(dirname $0)"
export esdir
while true; do
    rm -f /tmp/es-restart /tmp/es-sysrestart /tmp/es-shutdown /tmp/es-update
    "$esdir/emulationstation" "$@"
    ret=$?
    [ -f /tmp/es-restart ] && continue
    if [ -f /tmp/es-sysrestart ]; then
        rm -f /tmp/es-sysrestart
        sudo reboot
        break
    fi
    if [ -f /tmp/es-shutdown ]; then
        rm -f /tmp/es-shutdown
        sudo poweroff
        break
    fi
	if [ -f /tmp/es-update ]; then
        "$esdir/systemupdate.sh"
        break
    fi
    break
done
exit $ret
