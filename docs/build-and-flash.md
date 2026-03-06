# Build, Package, and Flash (ESP Web Tools)

## Build firmware and LittleFS image
From `firmware/`:

```bash
pio run
pio run -t buildfs
```

Expected outputs:
- `.pio/build/esp32dev/bootloader.bin`
- `.pio/build/esp32dev/partitions.bin`
- `.pio/build/esp32dev/firmware.bin`
- `.pio/build/esp32dev/littlefs.bin`

Copy these into `firmware/build/` with the same names referenced by `installer/manifest.json`.

## Upload from browser
Host the `installer/` directory over HTTP (GitHub Pages or local static server), then open `installer/index.html` and click the install button.

## Direct serial fallback
```bash
pio run -t upload
pio run -t uploadfs
```
