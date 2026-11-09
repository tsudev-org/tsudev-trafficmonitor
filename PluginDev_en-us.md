**[Tiếng Việt](./PhatTrienPlugin.md) | English** · [Home](./README_en-us.md) · [Plugins](./Plugin_en-us.md)

# Plugin development for TrafficMonitor-VN

This page opens from the **Plug-in development guide** link in the **Plug-in Manage** dialog (when the interface language is not Vietnamese).

A plugin is a Windows DLL (C/C++) that exports the function `TMPluginGetInstance`, which returns an object implementing the `ITMPlugin` interface. The whole interface lives in a single header, [`include/PluginInterface.h`](./include/PluginInterface.h) (comments in Vietnamese). Current interface version: **8**.

Complete example: [`PluginDemo`](./PluginDemo) - 2 text items (date, time), 2 custom-drawn items, an options dialog and commands in the right-click menu.

## 1. Create the project

1. Visual Studio 2022 → **Dynamic-Link Library (DLL)** (C++). MFC is optional; `PluginDemo` uses MFC in a shared DLL.
2. Add `include/PluginInterface.h` to the project.
3. Build every architecture your users need: **x86**, **x64**, **ARM64EC**. A plugin must match the architecture of `TrafficMonitor.exe`.

## 2. Three required parts

### Display item - `IPluginItem`

Each item is one cell in the main window/taskbar window. Required functions:

| Function | Returns |
|---|---|
| `GetItemName()` | Item name, shown in the item selection list |
| `GetItemId()` | A **unique and stable** ID (the program stores settings by this ID) |
| `GetItemLableText()` | Label, for example `"Date:"` |
| `GetItemValueText()` | Current value - **only return the stored string**, do not fetch data here because it is called very often |
| `GetItemValueSampleText()` | The longest possible sample string, used to compute the cell width |

To draw the item yourself, override `IsCustomDraw()` to return `true` and draw in `DrawItem()` (GDI) or `DrawItemEx()` (through `IPluginDrawer`, which also supports Direct2D).

### Plugin - `ITMPlugin`

| Function | What to do |
|---|---|
| `GetItem(int index)` | Return item number `index`; **must return `nullptr`** when `index` is past the last item |
| `DataRequired()` | Called on every refresh cycle; fetch your data here and store it |
| `GetInfo(PluginInfoIndex)` | Name, description, author, copyright, version (`TMI_VERSION`, such as `1.2.3`), home page |

Do not override `GetAPIVersion()` - the header defines it. Other optional functions: `ShowOptionsDialog`, `OnMonitorInfo` (receives the program's network/CPU/RAM/temperature readings), `GetTooltipInfo`, menu commands (`GetCommandCount`/`GetCommandName`/`OnPluginCommand`), `OnInitialize` (receives an `ITrafficMonitor` pointer for DPI, theme colour, configuration folder, language strings and notifications).

### Exported function - `TMPluginGetInstance`

```cpp
extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance();
```

Return a pointer to a **global or static** object that lives until the program exits.

## 3. Minimal example

```cpp
#include <windows.h>
#include <mutex>
#include <string>
#include "PluginInterface.h"

class CUptimeItem : public IPluginItem
{
public:
    //Called on the monitoring thread (from DataRequired)
    void SetValue(const std::wstring& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_value = value;
    }
    const wchar_t* GetItemName() const override { return L"Uptime"; }
    const wchar_t* GetItemId() const override { return L"en-uptime-7Qk2"; }
    const wchar_t* GetItemLableText() const override { return L"Up:"; }
    //Called on the UI thread: copy under the lock into a string used only by the UI thread
    const wchar_t* GetItemValueText() const override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shown = m_value;
        return m_shown.c_str();
    }
    const wchar_t* GetItemValueSampleText() const override { return L"999:59"; }

private:
    mutable std::mutex m_mutex;
    std::wstring m_value;           //Written by the monitoring thread
    mutable std::wstring m_shown;   //Read/written only by the UI thread
};

class CUptimePlugin : public ITMPlugin
{
public:
    CUptimeItem item;
    IPluginItem* GetItem(int index) override { return index == 0 ? &item : nullptr; }
    void DataRequired() override
    {
        ULONGLONG minutes = GetTickCount64() / 60000;
        wchar_t buff[32];
        swprintf_s(buff, L"%llu:%02llu", minutes / 60, minutes % 60);
        item.SetValue(buff);
    }
    const wchar_t* GetInfo(PluginInfoIndex index) override
    {
        switch (index)
        {
        case TMI_NAME: return L"Uptime";
        case TMI_DESCRIPTION: return L"Shows how long the computer has been running (hours:minutes).";
        case TMI_AUTHOR: return L"Your name";
        case TMI_COPYRIGHT: return L"Copyright (C) 2026";
        case TMI_VERSION: return L"1.0.0";
        case TMI_URL: return L"https://github.com/<account>/<repo>";
        default: return L"";
        }
    }
};

static CUptimePlugin g_plugin;
extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() { return &g_plugin; }
```

## 4. Testing

1. Copy the DLL into the `plugins` folder next to `TrafficMonitor.exe` (for a build from source: `Bin\<architecture>\<configuration>\plugins`).
2. Open TrafficMonitor → **Plug-in Manage** to check the load status, details and version.
3. Turn on the new item in **Show More Info** (main window) or **Display settings...** (taskbar window).

## 5. Notes

* **Threads:** `DataRequired` and `OnMonitorInfo` are called on TrafficMonitor's **monitoring thread**; `GetItemValueText`, `DrawItem`/`DrawItemEx`, dialogs and menu commands are called on the **UI thread**. Data written by the first group and read by the second must be protected (a lock, or write to a copy and swap it under a lock).
* **Performance:** `DataRequired` blocks the program's monitoring loop (network speed, CPU and RAM only update after it returns). Do slow work (network, disk) on your plugin's own thread and only copy the result in `DataRequired`.
* **Language:** use `ITrafficMonitor::GetLanguageId()` or `GetStringRes()` to follow the language the user selected.
* **DPI:** scale sizes by `GetDPI()/96` when drawing yourself.
* **Safety:** a plugin runs with the same privileges as TrafficMonitor. Do not download and run code from the network, and do not write outside the configuration folder (`GetPluginConfigDir()`) unless needed.
* The plugin interface change history (versions 1-8) is at the end of [`include/PluginInterface.h`](./include/PluginInterface.h).
