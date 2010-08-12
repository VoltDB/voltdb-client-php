#!/bin/bash

if [ `pgrep -f org.voltdb.VoltDB | wc -l` -ne 0 ]; then
    echo "There is a VoltDB server running already that you should stop before continuing."
    echo "You can kill it using \"ant serverkill\" (Warning: kills ALL local VoltDB servers)."
    exit 1
fi