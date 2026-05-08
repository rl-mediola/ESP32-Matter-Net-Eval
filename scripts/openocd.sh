openocd \
	-s "$OPENOCD_SCRIPTS" \
	-f "$OPENOCD_SCRIPTS/board/esp32c5-builtin.cfg" \
	-c init \
	-c targets \
	-c 'reset init'