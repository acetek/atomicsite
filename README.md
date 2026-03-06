# atomicsite

## ESP32 Captive Portal Interview Platform

This repository now includes a working ESP32 implementation of the interview platform concept, including:

- Captive portal onboarding flow
- Laptop replacement checklist and handoff report export
- Software deployment profile catalog
- Branding and logo customization
- Client handoff microsite preview
- Audit timeline with CSV export
- ESP Web Tools installer assets (`installer/`)

## Repository layout

- `firmware/` - PlatformIO ESP32 firmware + LittleFS web app
- `installer/` - ESP Web Tools web installer page and manifest
- `docs/` - project design + build/flash instructions

## Quick start

```bash
cd firmware
pio run
pio run -t buildfs
```

Then follow [`docs/build-and-flash.md`](docs/build-and-flash.md) to package binaries and flash through ESP Web Tools.
