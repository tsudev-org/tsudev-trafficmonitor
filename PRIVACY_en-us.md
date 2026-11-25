**[Tiếng Việt](./PRIVACY.md) | English**

# Privacy and offline mode - Tsu Tramon

Tsu Tramon runs with every feature **without an account and without a network connection**. This page lists every
network connection the app can open, when it opens it and what it sends, so you can check for yourself. The privacy
policy of the tsudev ecosystem: [tsudev.com/privacy](https://tsudev.com/privacy).

## Summary

* **Not signed in to a tsudev account** (the default): the app sends **none of your data** over the network. Settings and
  traffic history stay on your computer (`%AppData%\tsutramon` or the program folder). No telemetry, no ads, no
  identifiers sent anywhere.
* **Full offline mode** (Options → tsudev account): the app **makes no connections on its own** - no update check, no
  public IP lookup, no TsuDEV Pro sign-in or refresh. Third-party plugins manage their own connections.
* **Signed in to a tsudev account** (optional, only for TsuDEV Pro): the app talks only to `tsudev.com` to verify the
  account and the Pro entitlement. Your traffic, history and settings are **not** sent.

## Every connection the app makes

| Connection | When | Sends | Receives |
| --- | --- | --- | --- |
| GitHub: signed version info (`version_signed.xml` + `.sig`) | At startup (option "Check for update at startup") or when you click "Check for Update..." | None of your data (a plain GET request) | Version, download links, SHA-256 - trusted only with a valid signature |
| GitHub: update package `.zip` | Only when you accept an update | Nothing | The zip, saved only if its SHA-256 matches |
| `api.ipify.org`, `api6.ipify.org` | Only when you open the "Connection Details" dialog | Nothing (the service sees your public IP - which is what is being asked) | Your public IP address, not stored |
| `tsudev.com` (OIDC sign-in, Pro entitlement) | **Only when signed in**: at sign-in, 15 seconds after startup then every 12 hours, on "Refresh status", on sign-out | One-time sign-in code + PKCE, session tokens, the **installation ID** (128 random bits, renewed on sign-out) | Session tokens, a digitally signed TsuDEV Pro status |
| Web pages (Help, Update log, Support, Skin gallery) | Only when you click a link | Opens your default browser | - |

**Full offline mode blocks every row above** (except links you click yourself, which open your browser).

## Account data kept on your computer

When you sign in, the app stores on your computer:

* The sign-in session (refresh token) in **Windows Credential Manager** (entry `tsudev/tsutramon/refresh_token`),
  encrypted by Windows for your user account.
* The signed TsuDEV Pro status (`tsudev_entitlement.jws`) and the tsudev.com public keys (`tsudev_jwks.json`) in the
  configuration folder, so the app knows your Pro status while offline.
* The installation ID in `config.ini` (section `[tsudev_account]`).

**Options → tsudev account → Sign out and clear local data** revokes the session on tsudev.com, deletes all three and
creates a new installation ID. Tsu Tramon settings and traffic history are not affected.

## Verify it

The source code is open at [tsudev-org/tsutramon](https://github.com/tsudev-org/tsutramon): the connections above are
inventoried in [ADR-0002](./docs/adr/0002-tsudev-pro-ban-mien-phi-cuc-bo.md) (Vietnamese); automated tests
(`tests/ProAccountTests.cpp`) check that nothing calls `tsudev.com` while signed out or in full offline mode. How to
check a download is genuine: [SECURITY.md](./SECURITY.md).
