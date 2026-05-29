# ESP32-Matter-Net-Eval (for ESP32-C5 Devkit)

Modified Matter ESP32 Lighting Example.
Changes:
- external CHIP platform
- CHIP BLE advertising will not be turned off
- an additional BLE GATT service is running
- Interrupt on Boot button (GPIO 28)

## Wi-Fi
Define Wi-Fi configs.
```
CONFIG_DEFAULT_WIFI_SSID="sample"
CONFIG_DEFAULT_WIFI_PASSWORD="sample"
```
Ping the Wi-Fi IP address.

## BLE
Run `scripts/test-ble.sh`

## Thread
```
./chip-tool pairing ble-thread $NODE_ID hex:$THREAD_DATASET_HEX $SETUP_PASSCODE $DISCRIMINATOR
while true; do ./chip-tool onoff toggle 1 1; sleep 1; done
```