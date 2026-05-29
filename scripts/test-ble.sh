#!/usr/bin/env bash
set -euo pipefail

dev="D0:CF:13:F0:BA:06"
char="/org/bluez/hci0/dev_D0_CF_13_F0_BA_06/service000e/char000f"

{
  echo "scan le"
  sleep 3
  echo "scan off"
  echo "connect $dev"
  sleep 5
  echo "menu gatt"
  echo "select-attribute $char"
  while true; do
    echo "read"
    sleep 1
  done
} | bluetoothctl