**[Tiếng Việt](./Help.md) | English**

# TrafficMonitor-VN - Frequently Asked Questions (FAQ)

Common questions about using TrafficMonitor-VN. Features and basic usage are described in the [README](./README_en-us.md).

> Menu and button names on this page are the labels of the English UI (`language/English.ini`). If you use Vietnamese, see the [Vietnamese version](./Help.md).

---

### On Windows 11, the taskbar window overlaps the Widgets

Open **Options...** → **Taskbar Window Settings** tab → click **Settings related to Windows11**, then check **Avoid overlapping with right Widgets**.

If Widgets is not shown on the taskbar, this option is greyed out.

### On Windows 11, "Taskbar window appears to the left of the taskbar" is greyed out

On Windows 11 this option is only available when the taskbar is **centered**. When the taskbar is left-aligned, there is no free space on the left for TrafficMonitor.

### On Windows 11, the taskbar window overlaps the taskbar icons when the taskbar is full

This cannot be fixed at the moment. Avoid pinning too many apps to the taskbar, or set **Combine taskbar buttons and hide labels** to **Always** in Windows Settings.

### How do I show CPU and memory (RAM) usage?

Right-click the floating window and check **Show More Info**. To show them on the taskbar window too, right-click the taskbar window → **Display Settings...**, then check CPU and memory in the dialog.

### How do I change the "CPU", "MEM"... labels on the taskbar?

The taskbar window labels can be customized: right-click the taskbar window → **Options...** → **Taskbar Window Settings** tab → **Display Text settings...**. Double-click the text in the right column to edit it, then click **OK** twice.

Because the labels are customized by you, they **do not change automatically** when you switch languages. After switching, click **Restore Default** in the **Display Text Settings** dialog.

### The network speed always shows 0KB

This usually happens right after the computer switches networks (for example Wi‑Fi to wired, or a VPN turned on/off).

1. Right-click → **Select Network Connections** → **Refresh connection list**.
2. If it persists, pick the adapter you want to monitor in **Select Network Connections** instead of **Auto Select**.
3. If it still persists, go to **Options...** → **General Settings** → **Advanced** → **Select the connection to monitor...**, check only the network you want to monitor and click **OK** twice.

### How do I set a separate color for each item on the taskbar window?

Go to **Options...** → **Taskbar Window Settings** tab, check **Specify colors for each item**, then click the color box to the right of **Text color** to open **Taskbar window color settings**. Without this option you can only set one color for all text.

### "Auto run when Windows starts" does not work

There are two auto-run methods, chosen in **General Settings** → **Auto-run mode**:

* **Registry**: creates a `TrafficMonitor` value under `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run`. No administrator rights needed, but it does not work if the program is set to always **run as administrator**.
* **Task Scheduler (requires admin)**: creates a task in Task Scheduler (open it via `Control Panel\System and Security\Administrative Tools`). The edition with temperature monitoring needs this method because the hardware library requires elevated rights.

The most common cause is that you **moved the program folder** after turning on auto run. To fix it: open **General Settings**, uncheck **Auto run when Windows starts**, click **OK**, then open it again and check it again; or click **Reset autorun**.

Some antivirus/security software can block auto run - allow TrafficMonitor in that software.

When you switch between the two methods, the program removes the auto-run entry of the old method.

### The program says "Unable to save settings"

The program cannot write to its own folder. To fix it:

1. Move the program to a writable folder (for example `D:\Tools\TrafficMonitor`); do **not** put it under `C:\Program Files` unless it runs as administrator; or
2. Go to **General Settings** → **Configuration and data files** → choose **Save to Appdata directory**. Settings are then saved in `C:\Users\<user name>\AppData\Roaming\TrafficMonitor`.

If the error persists, create (or edit) `global_cfg.ini` next to `TrafficMonitor.exe` with:

```ini
[config]
portable_mode = false
```

If you cannot create the file in place, create it on the Desktop and copy it into the program folder. As a last resort, delete `C:\Users\<user name>\AppData\Roaming\TrafficMonitor\config.ini` (it will be recreated).

When the program folder is not writable, settings are saved to AppData automatically and **Save to the program directory** is disabled.

### Settings are lost after restarting the computer

If you never saw "Unable to save settings", settings are being saved normally. The usual cause is **several copies of TrafficMonitor** on the computer, each saving settings next to its own exe. For example, you changed settings in `D:\software\TrafficMonitor\TrafficMonitor.exe`, but at startup `D:\software1\TrafficMonitor\TrafficMonitor.exe` runs.

To fix it:

1. Switch the configuration location to **Save to Appdata directory**.
2. Delete the copies you don't use, open the one you want → **General Settings** → click **Reset autorun**.

### The network speed value is cut off

Fonts have different character widths, so values can be clipped. Go to **Options...** → **Taskbar Window Settings** and choose a larger **Net speed data width**.

### How do I turn off "Mouse Penetrate"?

Right-click the TrafficMonitor icon in the **notification area** (right side of the taskbar) and uncheck **Mouse Penetrate**.

While it is on, the floating window ignores all mouse input; the notification area icon's right-click menu is the same as the floating window's. So that you never lose the menu, the notification area icon is **shown automatically** when:

* **Mouse Penetrate** is turned on;
* the floating window is hidden while the taskbar window is not shown;
* the taskbar window is closed while the floating window is hidden;
* the taskbar window is closed while **Mouse Penetrate** is on.

### Text color doesn't suit a light Windows taskbar

In **Taskbar Window Settings**, click **Preset** and choose the light mode preset.

You can also check **Auto adapt to Windows 10 dark/light themes** so the program switches presets when Windows switches between light and dark; click **Auto Adapt settings...** to choose the preset for each mode.

### The taskbar window is not shown when HDR is on

Try unchecking **Background transparent** in the **Taskbar Window Settings** tab, or switch to **Direct2D** rendering in **Rendering settings**.

### CPU usage differs from Task Manager

On Windows 10 and later, to match Task Manager, go to **General Settings** → **Advanced** → **CPU usage acquisition method:** and choose **Use the performance counter**.

### About temperature monitoring

The released Lite edition **does not measure temperatures**. Temperature monitoring exists only in the full edition, which you build yourself (see the [README](./README_en-us.md#building-from-source)).

In the full edition it is **off by default** because it can cause problems on some machines. To turn it on, go to **General Settings** → **Hardware Monitoring**. Temperature items then appear in the taskbar window's **Display Settings...**.

It relies on the open-source library [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor). If temperatures are missing or wrong, try running LibreHardwareMonitor itself:

* If LibreHardwareMonitor cannot show them either → the problem is in the library/hardware; report it to the LibreHardwareMonitor project.
* If the latest LibreHardwareMonitor shows them correctly → you can replace `LibreHardwareMonitorLib.dll` in the TrafficMonitor folder with the newer file. **Only download it from LibreHardwareMonitor's official page** and check its signature/hash before replacing.

> **Warning:** hardware monitoring (temperature, GPU usage) can increase CPU/RAM usage; some users have seen crashes or system hangs with it turned on. Only turn it on if you accept these risks.

### "MSVC\*.dll" or "mfc\*.dll" is missing at startup

Install the latest Microsoft Visual C++ Redistributable from Microsoft:
[Latest supported Visual C++ Redistributable downloads | Microsoft Learn](https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist)

### Plugins

How to install plugins and troubleshoot them: [Plugins](./Plugin_en-us.md); writing your own: [Plugin development](./PluginDev_en-us.md).

> **Security note:** a plugin is a DLL that runs **with the same rights as TrafficMonitor**. Only install plugins from sources you trust; plugins of unknown origin may contain malware.

### How do I check that a download is genuine?

Download only from [trafficmonitor.tsudev.com](https://trafficmonitor.tsudev.com/tai-ve/) or the repository's [Releases](https://github.com/tsudev-org/tsudev-trafficmonitor/releases/latest). Compare the zip's SHA-256 with `SHA256SUMS.txt` (PowerShell: `Get-FileHash <file>.zip`); the in-app update dialog also shows the SHA-256 from the signed version information. Full steps, including build attestation: [SECURITY.md](./SECURITY.md).

### The program crashes

1. Restart the program and **turn off everything** in **General Settings** → **Hardware Monitoring** - most crashes are related to this feature.
2. If it still crashes, go to **Advanced** → **Plug-in manage...**, disable all plugins and restart. (If the program won't open, delete the `.dll` files in the `plugins` folder next to `TrafficMonitor.exe`.)
3. If you have ruled out both, [open an issue](https://github.com/tsudev-org/tsudev-trafficmonitor/issues/new/choose) with the text from the crash window.

### Feedback and bug reports

Please use [TrafficMonitor-VN's GitHub Issues](https://github.com/tsudev-org/tsudev-trafficmonitor/issues). Describe the problem, error messages and what you have tried, with screenshots and your configuration file (**General Settings** → **Open configuration file directory**) if possible. Do **not** attach sensitive information.

Report security vulnerabilities privately as described in [SECURITY.md](./SECURITY.md), not publicly.
