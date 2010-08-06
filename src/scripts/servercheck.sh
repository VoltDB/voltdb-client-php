#!/bin/bash

if [ `pgrep -f org.voltdb.VoltDB | wc -l` -ne 0 ]; then
    echo "There is a VoltDB server running already that you should stop before continuing."
    exit 1
fi