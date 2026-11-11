**[Tiếng Việt](./update_log_vi.md) | English**

# Update log - tsudev-trafficmonitor

Changes in each tsudev-trafficmonitor release by tsudev, newest first. One-line summary per release: [CHANGELOG.md](../CHANGELOG.md).
Releases before the rename were called **TrafficMonitor-VN**; their entries below are kept as released.

## 26.11.1201 (12/11/2026)

### Renamed to tsudev-trafficmonitor
* The app, executable (`tsudev-trafficmonitor.exe`), folder inside the zip, documentation and website now use the name **tsudev-trafficmonitor**, in line with the tsudev ecosystem.
* **Automatic data migration:** on first run, settings and traffic history in `%AppData%\TrafficMonitor` are copied to `%AppData%\tsudev-trafficmonitor` (the old folder is kept so the old version still works).
* **Automatic start-with-Windows migration:** this app's old Registry/Task Scheduler entry is replaced with the new name; an entry belonging to a separately installed original TrafficMonitor is left untouched.
* The old and new versions do not run at the same time: starting the new one while the old one is open reports that the app is already running.
* Existing plugins keep working (the plugin interface is unchanged; `ITrafficMonitor` remains an alias of `ITsudevTrafficMonitor`).

### Interface cleanup
* Removed the Gitee update source option (disabled since the first localized release); General Settings shows "Update source: GitHub".
* Removed the original project's donation strings and resources; the About dialog links to **Support tsudev** at https://trafficmonitor.tsudev.com/ung-ho/ (the program contains no payment details).

### Branding
* New logo for the program icon, tray icon, About dialog and website; new `tsudev_Logo` skin.

## 26.11.1001 (10/11/2026)

### Fixes
* After downloading an update and verifying its SHA-256, 26.11.0905 did not open the Downloads folder; it now opens the folder with the file selected.

### Security
* Safer skin file (`skin.xml`) parsing: files up to 1 MB, limits on element count and depth; out-of-range sizes, positions, font sizes and colors are clamped; overly long strings are truncated. Invalid skins fall back to the default look. Covered by automated tests and fuzzing with the real skins.
* **Reproducible builds:** the same source and toolchain produce byte-identical `TrafficMonitor.exe` and zip packages; CI builds twice to check. The "compiled date" in About is now the source commit time. How to verify: [SECURITY.md](../SECURITY.md) (Vietnamese).

## 26.11.0905 (09/11/2026)

### Security
* When you accept an update, the app **downloads the package itself** for your architecture (with a progress dialog and a Cancel button), computes its SHA-256 while downloading and compares it with the SHA-256 in the digitally signed version information. Only on a match is it saved to your Downloads folder and the folder opened with the file selected; on a mismatch the file is deleted and an error is shown. You no longer need to compare the hash yourself.
* Downloads only from this repository's Releases page; every redirect must go to a GitHub server over HTTPS; 50 MB limit; existing files are never overwritten; the saved file is marked as downloaded from the Internet, as a browser would. The app never extracts or runs the package.
* If the download fails (network error), the app asks whether to open the link in your browser as before.
* A CI integration test downloads a real release package to exercise match, SHA-256 mismatch, oversize, cancel and redirect-to-unknown-host cases.

### Other
* The former `TrafficMonitor-VN` repository has been removed; all source code, releases and the update channel live in `tsudev-org/tsudev-trafficmonitor`.

## 26.11.0904 (09/11/2026)

> Version 26.11.0903 was not released (the build failed while generating the SBOM, before the Release was created); 26.11.0904 has the same content.

### Security
* Upgraded the tinyxml2 XML library to 11.0.0 (previously an unreleased 9.0 snapshot), fixing CVE-2024-50614 and CVE-2024-50615: a malicious `skin.xml` could crash the app.
* The update check accepts download links from both the old (`tsudev-tsudev`) and the new (`tsudev-org`) repository address; it still only trusts version information with a valid signature.
* Every release package ships with an SPDX 2.3 SBOM (`*.spdx.json`) attested by GitHub (`gh attestation verify <file.zip> -R tsudev-org/tsudev-trafficmonitor --predicate-type https://spdx.dev/Document/v2.3`).
* The update-check logic has automated tests, also run under AddressSanitizer and fuzzing in CI; CodeQL also scans the website, scripts and workflows.

### Repository
* The repository moved to the [tsudev-org](https://github.com/tsudev-org/tsudev-trafficmonitor) organization; links in the app, documentation and website point to the new address. The old address redirects, and installed copies keep receiving updates.

### Website
* Fixed the contrast of the current menu link and made code blocks scrollable with the keyboard (checked with axe-core, WCAG 2.2 AA).

## 26.11.0902 (09/11/2026)

### tsudev identity
* The program icon (exe, windows, taskbar, the fourth notification-area icon style) is the tsudev owl; the network-graph notification icon styles are unchanged.
* About dialog: new header image; copyright line "Copyright (c) 2026 tsudev"; removed the "Original project" and "Acknowledgement" links. The Anti 996 License and the required copyright notice are shown by the "License" link.
* The Vietnamese and English language packs credit tsudev as translator; other language packs no longer show a translator line.

### Skins
* New skins `tsudev_DoThi_Sang` (light) and `tsudev_DoThi_Toi` (dark): network speed graph background, also showing GPU and disk when "Show More Info" is on.

### Security
* Removed the plugin version check against a third-party repository: the app now only uses the network for the signed update check and for the public IP lookup when you open Connection Details. The plugin interface is unchanged (API version 8).

### Documentation
* Rewrote the English documentation (README, FAQ, update log); added CHANGELOG.md.

## 26.11.0901 (09/11/2026)

### New version numbering
* From this release on, versions follow tsudev's **`{YY}.{M}.{DD}{NN}`** scheme (year.month.day + sequence number within the day): `26.11.0901` is the first release of 9 Nov 2026. The previous release was V1.91-vn; versions are compared numerically, so older releases still detect the update.
* Release tags look like `V26.11.0901-vn`; the download is renamed to `tsudev-trafficmonitor_26.11.0901_<arch>_Lite.zip`.

### English documentation
* Added English documentation: Plugins (`Plugin_en-us.md`), Skin gallery (`SkinGallery_en-us.md`), Skin tutorial (`SkinTutorial_en-us.md`), Plugin development (`PluginDev_en-us.md`).
* When the UI language is not Vietnamese, the Help, Download more skins, Skin tutorial, Download more plug-ins and Plugin development links open the English documents (Help opens `README_en-us.md`); Vietnamese still opens the website.

## V1.91-vn (09/11/2026)

### Links
* The product website **https://trafficmonitor.tsudev.com** is live on its official domain (Cloudflare Pages, HSTS, strict CSP, DNSSEC, CAA).
* In-app links open the guides on the website instead of `.md` files on GitHub: FAQ (`/huong-dan/hoi-dap/`), Update log (`/nhat-ky/`), Help (`/huong-dan/`), Skin gallery, Skin tutorial, Plugins, Plugin development. The `.md` files on GitHub remain as a fallback.
* For languages other than Vietnamese, FAQ and Update log open the English documents on GitHub (`FORK_HELP_EN_URL`, `FORK_UPDATE_LOG_EN_URL`).

## V1.90-vn (09/11/2026)

### Skins
* New default skin **`0_tsudev_Cu`** (the tsudev owl, light background) and skin **`tsudev_VongMang`** (tsudev mark, dark background), generated by `tools/skin_assets/make_skins.py` from tsudev's source artwork.
* Removed 4 inherited third-party skins featuring cartoon characters: `0_MacDinh`, `MacDinh2`, `GiaoDien01`, `GiaoDien02`. Installations using them switch to `0_tsudev_Cu` automatically (the first skin by name).

## V1.89-vn (09/11/2026)

### Bug fixes
* About dialog: the "Acknowledgements" link overlapped the "tsudev.com" link (V1.88-vn) because it was placed in a third column when the layout auto-sized to the text width; the links are now laid out in two columns.

## V1.88-vn (09/11/2026) - Signed updates

### tsudev identity
* Executable file information: publisher `tsudev`, `Copyright (c) 2026 tsudev`, with `https://tsudev.com`.
* The About dialog links to [tsudev.com](https://tsudev.com).
* Product page [trafficmonitor.tsudev.com](https://trafficmonitor.tsudev.com): downloads with SHA-256, guides, update log, security.

### Security
* Every URL the app uses (including third-party library credits and the public IP lookup service) is declared in a single place, `ForkConfig.h`; removed an unused test function that called an external page.
* Added `tools/check_urls.py` to CI: blocks `http://`, URLs outside the allow list and URLs hard-coded in C++ outside `ForkConfig.h`; links are checked weekly.
* GitHub Actions are pinned by commit SHA; creating a Release requires approval through the `release` environment.
* **Signed version information:** the update check reads `version_signed.xml` from the latest release and trusts it only when its ECDSA P-256 signature is valid (verified with Windows CNG against a public key embedded in the app); an update is offered only when its version is greater than the running one. The update dialog shows the package's SHA-256 for checking after download.
* Network access: after redirects, the final URL must be HTTPS and on the allowed host list.
* SECURITY.md describes the chain of trust (signature → SHA-256 → build attestation) and how to verify it yourself.

### Full edition (temperature monitoring)
* Sensor values are read as `float` instead of `double` with an implicit conversion; GPU and disk usage are cast explicitly. CI also builds the full x64 edition (not released) with the same warning checks as Lite.

## V1.87-vn (08/11/2026) - Bridge release to the new repository

### Repository move
* The main repository moved to [tsudev-tsudev/tsudev-trafficmonitor](https://github.com/tsudev-tsudev/tsudev-trafficmonitor). From this release, update checks, downloads, help pages and bug reports use the new repository. The old `TrafficMonitor-VN` repository is archived (read-only) and keeps the full commit history.
* V1.86-vn users update to V1.87-vn as usual; later releases come from the new repository.

### Bug fixes
* Plugin version check: the latest-version table was written by the update-check thread and read by the Plugin Manager dialog without a lock (possible crash when opening the dialog during a check); it is now parsed into a temporary table and swapped under a lock, and readers get a copy.
* Plugin submenus and the "Edit skin" menu truncated the `HMENU` handle to a 32-bit `UINT` on 64-bit builds; now `UINT_PTR`.
* File reading (`GetFileContent`): when `tellg()` failed and returned -1, the value became a huge size and was allocated; errors are now checked and only the bytes actually read are kept.
* Fixed all numeric conversion warnings (C4244, C4267, C4018), pointer truncation (C4302, C4311) and Vietnamese text in `char` strings (C4566) in the Lite build; CI treats these warnings as errors.
* The build time in the About dialog lost the year ("Sun 11/08/") on en-US build machines; it is now `yyyy-MM-dd HH:mm:ss UTC`.
* Skin gallery: missing space after "Designer:".
* Messages in the language packs had a stray `=` before quoted phrases (e.g. `file= "..."`), inherited from the original; removed in every language pack.

### Links moved to TrafficMonitor-VN
* "Download more skins" (Skin gallery), "Download more plug-ins" and "Plugin development" (Plugin Manager) open TrafficMonitor-VN's own pages: [KhoGiaoDien.md](../KhoGiaoDien.md), [Plugin.md](../Plugin.md), [PhatTrienPlugin.md](../PhatTrienPlugin.md).
* The crash message in the other 14 language packs (French, German, Japanese...) asks users to report at TrafficMonitor-VN's Issues page instead of by email.
* All URLs of the localized edition are collected in `ForkConfig.h`.

### UI (after checking real screenshots)
* Shortened labels that were clipped by group boxes, and renamed the language "Tiếng Việt (Chuẩn)" → "Tiếng Việt" (it was cut off in the language drop-down).
* Old language settings saved with the display name "Tiếng Việt (Chuẩn)" are matched by BCP-47 code to the current pack, so the language selector no longer falls back to "Follow system".

### Fixes from automated UI testing (V1.87-vn)
* Automatic network adapter selection: on machines with virtual adapters (Hyper-V/WSL, VirtualBox...) the app could pick a virtual adapter because it had the most bytes, showing almost zero speed while downloading over Wi-Fi/LAN. Adapters with a default gateway are now preferred; if none has one, selection works as before.
* The floating window's default labels changed from "Up:"/"Down:" to "↑:"/"↓:" (the English text overlapped the default skin's artwork). Custom labels are not affected.
* The About dialog showed "Tiếng Việt Bản dịch tiếng Việt: …"; now "Bản dịch Tiếng Việt: …".
* Plugin Manager: the "Version" column header was clipped; column widths were redistributed.

## V1.86-vn (07/11/2026) - First localized release

Based on TrafficMonitor 1.86.

### Vietnamese localization
* Vietnamese UI by default on first run; the Vietnamese language pack covers every string (438/438) and is embedded in the executable.
* Vietnamese versions of the color picker and the sample plugin (PluginDemo).
* Translated all source code comments (≈2,800 blocks in 186 files), Visual Studio filter names, README, FAQ, skin tutorial and issue templates into Vietnamese.
* Converted 27 GBK/UTF-16 source files to UTF-8 (BOM); renamed Chinese skin folders to ASCII names.

### UI
* Fixed 13 places where Vietnamese text was clipped in the Options dialogs (measured with Segoe UI 9pt against control sizes): shorter translations and a wider "Restore default" button in the Windows 11 settings; fixed 2 similar places in English.

### Chinese removal and copyright notices
* Removed the Simplified/Traditional Chinese language packs, the Chinese color picker, the Chinese acknowledgements (replaced with Vietnamese) and the Chinese release notes.
* The executable's version information uses the Vietnamese locale and the product name "TrafficMonitor-VN".
* About dialog: removed promotions of other software and added the localized edition's copyright line; crash reports go to TrafficMonitor-VN's Issues page instead of email.
* Removed 2 skins using Microsoft Windows wallpaper/logos (GiaoDien04, GiaoDien07); the Chinese word for "memory" in GiaoDien11's background was replaced with "RAM".
* Removed Chinese UI screenshots from the documentation.
* Added NOTICE.md separating copyrights; the original license notice is kept as required by the Anti 996 License.
* `tools/check_i18n.py` scans for Chinese text in documents, UTF-16 `.rc` files, `.ini`, `.xml`... (except LICENSE_CN and the Japanese pack).

### Security
* Update checks only use the TrafficMonitor-VN repository; download links are strictly bound to the repository's Releases page.
* Versions are compared numerically (string comparison bug fixed).
* `GetURL` accepts HTTPS only, with a timeout and a size limit.
* Public IP lookup moved to ipify; IP format is validated.
* DLL hijacking protection at startup and when loading plugins.
* Control Flow Guard, CET, ASLR, DEP and `/sdl` enabled for Release builds.
* CI: build on every push/PR, CodeQL, Dependabot, SHA-256 checksums and build provenance attestation for releases.

### Bug fixes
* The ARM64EC build picked the x64 download link when checking for updates.
* `GetJsonValueSimple` could throw `out_of_range` on invalid data.
* The English issue template pointed to an unrelated repository.
* Public IP could not be retrieved ("Failed to get data"): the ipify URL lacked a "/" before "?", so WinINet dropped the `format=json` parameter; the parser now accepts both JSON and plain text.
* `GetURL` reads the response as raw bytes instead of wrapping it in a `CString` (avoids trailing garbage on odd byte counts); debug log when the HTTP status is not 200 or the response is too large.
* IPv6: when the machine has no global IPv6 address, "No IPv6 connection" is shown instead of "Failed to get data".
* Connection Details dialog: the public IP thread no longer touches the dialog's controls or object directly (they may already be destroyed); repeated double-clicks don't spawn multiple threads; double-click again to refresh; IPv4 is shown without waiting for IPv6.
* Connection Details dialog uses a copy of the data instead of pointers into the adapter table that the monitor thread may reallocate (crash risk); received/sent bytes update every second.
* 64-bit traffic counters (`GetIfEntry2`): the old 32-bit counters wrapped every 4 GiB, losing traffic statistics and showing wrong totals; adapter speeds above 4 Gbps (10 GbE) display correctly.
* Thread safety: reinitializing network connections runs only on the UI thread, and the adapter table is locked between the monitor and UI threads.
* Detection of a change in the number of network connections (VPN on/off, cable plugged...) never worked because the counter was always -1.
* The adapter table is allocated safely when the number of adapters changes between two calls (previously an uninitialized table could be read); empty tables are skipped.
* Network speed is computed with a monotonic clock instead of system time: clock changes/NTP sync could cause a division by zero (crash) or wrong speeds.
* Traffic history is saved via a temporary file and then replaced, so a power loss or full disk while writing no longer wipes the history; history is not overwritten when the file is temporarily unreadable.
* The Connection Details dialog opens the page of the active connection with the most traffic when "Select all" is in use or the selected connection is inactive (previously it could open a disabled adapter showing all zeros and IP 0.0.0.0); inactive connections are marked "Inactive" and greyed out.
