#!/bin/bash

set -e

dd if=/dev/urandom of=testfile bs=1M count=1
CRC=$(cat testfile | gzip -c | tail -c8 | hexdump -n4 -e '"%08X"')
rm -f crc_testfile*.dat
mv testfile crc_testfile_\[$CRC\].dat
