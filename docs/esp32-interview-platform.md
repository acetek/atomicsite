# ESP32 Captive Portal Interview Platform (Colorado Onsite CIO)

## Goal
Build a portable, no-internet-required ESP32 demo appliance that showcases practical onsite IT workflows Doug Jones is likely to value:
- Device onboarding and quick client-facing UX.
- Repeatable software deployment playbooks.
- Branding and customization controls for client handoff.
- Lightweight service tooling for laptop replacement and transition tasks.

## Why this is a strong interview demo
This concept demonstrates both technical depth and business pragmatism:
- **Field-ready engineering**: Works from a single microcontroller and power bank.
- **Client empathy**: Guides users through migration and setup with clear, branded flows.
- **Operational thinking**: Encodes day-to-day technician tasks into reusable tooling.
- **Security awareness**: Can be designed with signed packages, checksum validation, and audit logs.

## High-level architecture

### Hardware
- ESP32 DevKitC (or ESP32-S3 board with larger RAM/flash).
- MicroSD card module for scripts/packages/assets.
- Optional OLED for status (SSID, IP, active clients).
- USB power bank for portable demo mode.

### Firmware stack
- Wi-Fi AP mode + DNS catch-all + HTTP server for captive portal behavior.
- Embedded web app UI (HTML/CSS/JS) served from flash or SD.
- REST endpoints for:
  - package catalog reads,
  - branding config reads/writes,
  - runbook metadata,
  - device/session logging.

### Admin workflow
1. Power on ESP32.
2. Candidate/interviewer joins SSID (e.g., `OnsiteCIO-Demo`).
3. Captive portal auto-opens to dashboard.
4. Select a scenario:
   - “New laptop replacement”,
   - “Standard app bundle deployment”,
   - “Client brand handoff kit”, etc.

## Feature set to include

### 1) Captive portal onboarding
- Friendly landing page: who this tool is for and what it automates.
- “Technician mode” and “Client mode” toggle.
- Quick status card for connected technician devices.

### 2) Laptop replacement playbook builder
- Checklist-style workflow for migration:
  - inventory old machine,
  - identity/email profile setup,
  - browser profile transfer steps,
  - line-of-business app verification.
- Generate a **handoff report** (HTML/JSON) with completed items and notes.

### 3) Software deployment catalog
- Curated package tiles (Office, Chrome, Zoom, RMM agent, VPN).
- Deployment profile presets:
  - “Small business standard”,
  - “Accounting workstation”,
  - “Executive travel laptop”.
- Each tile includes:
  - description,
  - install order,
  - post-install checks,
  - rollback notes.

> Practical implementation note: ESP32 itself cannot deploy software directly to Windows endpoints at enterprise scale. For realism, model this as a runbook + script launcher pattern where scripts are exported/downloaded and run on technician laptop.

### 4) Branding and customization studio
- Upload/choose logo.
- Set company name, support email, support phone.
- Theme color picker.
- Preview client handoff page in real time.

### 5) Client handoff microsite
- “Welcome to your new device” page.
- Links to support, FAQ, and first-day checklist.
- Downloadable PDF/print-friendly quick start.

### 6) Audit and activity timeline
- Session log with timestamped actions.
- Export log to JSON/CSV.
- Useful talking point: accountability and consistency in field services.

## Technical implementation roadmap

### Phase 1: Core captive portal MVP (1 weekend)
- AP + DNS redirect + web server.
- Static dashboard with 3 mock tools.
- Branding config persisted in SPIFFS/LittleFS.

### Phase 2: Real utility workflows (week 2)
- Add replacement checklist engine.
- Add package catalog + profile presets.
- Add exportable handoff report.

### Phase 3: Polish for interview (week 3)
- Cleaner UX + mobile layout.
- Add demo data reset button.
- Add quick “scenario scripts” to walk interviewer through in 5 minutes.

## Interview demo script (5-7 minutes)
1. Connect to SSID and show captive portal launch.
2. Open “Laptop Replacement” and complete a mock migration checklist.
3. Apply “Accounting workstation” software profile.
4. Switch to Branding Studio and change logo/theme to “Colorado Onsite CIO”.
5. Show generated client handoff page + exported report/log.

## Extra ideas to stand out
- QR code on landing page for instant portal access.
- “Common issue simulator” cards (e.g., missing Outlook profile, OneDrive sync pause) with guided remediation steps.
- Skill-level mode selector for junior vs senior technician guidance.
- Optional offline AI FAQ (small curated troubleshooting prompts) served locally.
- USB backup integrity checker workflow with checksum verification.

## Risks and mitigations
- **Limited ESP32 resources** → Keep UI lightweight and externalize assets to SD.
- **Captive portal behavior varies by OS** → Include direct local URL fallback (e.g., `http://192.168.4.1`).
- **Security concerns for real client data** → Use synthetic demo data in interviews and enforce data wipe action.

## Suggested project structure
- `firmware/` - ESP32 source (AP, DNS, HTTP, storage).
- `web/` - dashboard UI and assets.
- `profiles/` - software deployment profiles (JSON).
- `branding/` - default logos/themes.
- `docs/` - runbooks and demo scripts.

## What this says about you
This project communicates that you can:
- translate repetitive support tasks into repeatable systems,
- design tools around real technician pain points,
- deliver polished client-facing experiences,
- and think in terms of maintainability, not just one-off fixes.
