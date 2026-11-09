**[Tiếng Việt](./Plugin.md) | English** · [Home](./README_en-us.md) · [FAQ](./Help_en-us.md) · [Plugin development](./PluginDev_en-us.md)

# Plugins — TrafficMonitor-VN

This page opens from the **Download more plug-ins** link in the **Plug-in Manage** dialog (when the interface language is not Vietnamese).

A plugin is a `.dll` file that adds new display items (date and time, weather, battery, temperature...) to the main window and the taskbar window.

> ⚠️ **Plugins run with the same privileges as TrafficMonitor** and can do anything the program can do. Only install plugins from sources you trust, and check the signature or hash published by the plugin author when available.

## Installing a plugin

1. Download the plugin `.dll` built for **the same architecture** as your TrafficMonitor-VN build (x86, x64 or ARM64EC — see **About**).
2. Exit TrafficMonitor.
3. Copy the `.dll` into the `plugins` folder next to `TrafficMonitor.exe` (create the folder if it does not exist).
4. Start TrafficMonitor again. Plugins are loaded at startup.
5. Turn on the plugin's display items: right-click menu → **Show More Info**, or **Options** → **Taskbar window** → **Display settings...**.

Manage plugins from **Options** → **General Settings** → **Advanced** → **Plug-in manage...**: view details, open plugin options, disable a plugin, open the plugin folder. Plugin configuration files are stored in the `plugins` folder of the configuration directory (default `%AppData%\TrafficMonitor\plugins`).

## Where to get plugins

* **Sample plugin** `PluginDemo` (date, time, custom-drawn items) is built together with TrafficMonitor-VN from the [`PluginDemo`](./PluginDemo) folder; use it for testing and as a template for your own plugin.
* **Plugins from the original project**: the author of TrafficMonitor (zhongyang219) publishes a set of plugins in the [TrafficMonitorPlugins](https://github.com/zhongyang219/TrafficMonitorPlugins) repository. These plugins are **not developed or reviewed by TrafficMonitor-VN**, but they use the same plugin interface and run on TrafficMonitor-VN. The **Plug-in Manage** dialog flags them when a newer version is available.
* **Your own plugin**: see [Plugin development](./PluginDev_en-us.md). To have your plugin listed here, open an [issue](https://github.com/tsudev-tsudev/tsudev-trafficmonitor/issues) with a link to its source code.

## Troubleshooting

* **The plugin is missing from the list or reports a load failure:** wrong architecture (for example an x86 plugin with the x64 build), a missing dependency, or the plugin requires a newer plugin interface than the program supports.
* **The program crashes after installing a plugin:** delete (or move out) the `.dll` you just installed from the `plugins` folder and restart; report the problem to the plugin author.
* See also the [FAQ](./Help_en-us.md).
