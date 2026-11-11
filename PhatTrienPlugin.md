**Tiếng Việt | [English](./PluginDev_en-us.md)** · [Trang chủ](./README.md) · [Plugin](./Plugin.md)

# Phát triển plugin cho tsudev-trafficmonitor

Trang này được mở từ liên kết **Hướng dẫn làm Plugin** trong hộp thoại **Quản lý Plugin**.

Plugin là một DLL Windows (C/C++) xuất hàm `TMPluginGetInstance`, trả về đối tượng triển khai giao diện `ITMPlugin`. Toàn bộ giao diện nằm trong một tệp tiêu đề [`include/PluginInterface.h`](./include/PluginInterface.h) (đã chú thích tiếng Việt). Phiên bản giao diện hiện tại: **8**.

Ví dụ đầy đủ: [`PluginDemo`](./PluginDemo) - 2 mục văn bản (ngày, giờ), 2 mục tự vẽ, hộp thoại tùy chỉnh, lệnh trên menu chuột phải.

## 1. Tạo dự án

1. Visual Studio 2022 → **Dynamic-Link Library (DLL)** (C++). Dùng MFC hay không tùy bạn; `PluginDemo` dùng MFC dạng DLL dùng chung.
2. Thêm `include/PluginInterface.h` vào dự án.
3. Build đủ các kiến trúc người dùng cần: **x86**, **x64**, **ARM64EC**. Plugin phải cùng kiến trúc với `tsudev-trafficmonitor.exe`.

## 2. Ba thành phần bắt buộc

### Mục hiển thị - `IPluginItem`

Mỗi mục là một ô trên cửa sổ nổi/thanh tác vụ. Các hàm bắt buộc:

| Hàm | Trả về |
|---|---|
| `GetItemName()` | Tên mục, hiện trong danh sách chọn mục |
| `GetItemId()` | ID **duy nhất và cố định** (chương trình lưu cấu hình theo ID này) |
| `GetItemLableText()` | Nhãn, ví dụ `"Ngày:"` |
| `GetItemValueText()` | Giá trị hiện tại - **chỉ trả về chuỗi đã lưu**, không lấy dữ liệu ở đây vì hàm được gọi rất thường xuyên |
| `GetItemValueSampleText()` | Chuỗi mẫu dài nhất có thể, dùng để tính chiều rộng ô |

Muốn tự vẽ: ghi đè `IsCustomDraw()` trả về `true` và vẽ trong `DrawItem()` (GDI) hoặc `DrawItemEx()` (qua `IPluginDrawer`, hỗ trợ cả Direct2D).

### Plugin - `ITMPlugin`

| Hàm | Việc cần làm |
|---|---|
| `GetItem(int index)` | Trả về mục thứ `index`; **phải trả về `nullptr`** khi `index` vượt quá số mục |
| `DataRequired()` | Được gọi theo chu kỳ làm mới; lấy dữ liệu tại đây rồi lưu lại |
| `GetInfo(PluginInfoIndex)` | Tên, mô tả, tác giả, bản quyền, phiên bản (`TMI_VERSION`, dạng `1.2.3`), trang chủ |

Không ghi đè `GetAPIVersion()` - giá trị này do tệp tiêu đề quyết định. Các hàm tùy chọn khác: `ShowOptionsDialog`, `OnMonitorInfo` (nhận số liệu mạng/CPU/RAM/nhiệt độ của chương trình chính), `GetTooltipInfo`, các lệnh menu (`GetCommandCount`/`GetCommandName`/`OnPluginCommand`), `OnInitialize` (nhận con trỏ `ITsudevTrafficMonitor` để lấy DPI, màu chủ đạo, thư mục cấu hình, chuỗi ngôn ngữ, hiện thông báo).

### Hàm xuất - `TMPluginGetInstance`

```cpp
extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance();
```

Trả về con trỏ tới một đối tượng **toàn cục hoặc tĩnh**, tồn tại tới khi chương trình thoát.

## 3. Ví dụ tối giản

```cpp
#include <windows.h>
#include <mutex>
#include <string>
#include "PluginInterface.h"

class CUptimeItem : public IPluginItem
{
public:
    //Gọi trên luồng giám sát (từ DataRequired)
    void SetValue(const std::wstring& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_value = value;
    }
    const wchar_t* GetItemName() const override { return L"Thời gian bật máy"; }
    const wchar_t* GetItemId() const override { return L"vn-uptime-7Qk2"; }
    const wchar_t* GetItemLableText() const override { return L"Bật máy:"; }
    //Gọi trên luồng giao diện: sao chép dưới khóa vào chuỗi chỉ luồng giao diện dùng
    const wchar_t* GetItemValueText() const override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shown = m_value;
        return m_shown.c_str();
    }
    const wchar_t* GetItemValueSampleText() const override { return L"999:59"; }

private:
    mutable std::mutex m_mutex;
    std::wstring m_value;           //Luồng giám sát ghi
    mutable std::wstring m_shown;   //Chỉ luồng giao diện đọc/ghi
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
        case TMI_DESCRIPTION: return L"Hiện thời gian máy đã chạy (giờ:phút).";
        case TMI_AUTHOR: return L"Tên bạn";
        case TMI_COPYRIGHT: return L"Copyright (C) 2026";
        case TMI_VERSION: return L"1.0.0";
        case TMI_URL: return L"https://github.com/<tài-khoản>/<repo>";
        default: return L"";
        }
    }
};

static CUptimePlugin g_plugin;
extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() { return &g_plugin; }
```

## 4. Thử nghiệm

1. Chép DLL vào thư mục `plugins` cạnh `tsudev-trafficmonitor.exe` (bản build từ mã nguồn: `Bin\<kiến trúc>\<cấu hình>\plugins`).
2. Mở tsudev-trafficmonitor → **Quản lý Plugin** để kiểm tra trạng thái nạp, thông tin và phiên bản.
3. Bật mục mới trong **Hiện thêm thông số** (cửa sổ nổi) hoặc **Sắp xếp các mục...** (thanh tác vụ).

## 5. Lưu ý

* **Luồng:** `DataRequired` và `OnMonitorInfo` được gọi trên **luồng giám sát** của tsudev-trafficmonitor; `GetItemValueText`, `DrawItem`/`DrawItemEx`, hộp thoại và lệnh menu được gọi trên **luồng giao diện**. Dữ liệu ghi ở nhóm thứ nhất và đọc ở nhóm thứ hai phải được bảo vệ (khóa, hoặc ghi vào bản sao rồi hoán đổi dưới khóa).
* **Hiệu năng:** `DataRequired` chặn vòng giám sát của chương trình (tốc độ mạng, CPU, RAM chỉ cập nhật khi nó trả về). Việc chậm (mạng, đĩa) hãy làm ở luồng riêng của plugin và chỉ sao chép kết quả trong `DataRequired`.
* **Ngôn ngữ:** dùng `ITsudevTrafficMonitor::GetLanguageId()` hoặc `GetStringRes()` để hiển thị theo ngôn ngữ người dùng chọn.
* **DPI:** nhân kích thước với `GetDPI()/96` khi tự vẽ.
* **An toàn:** plugin chạy cùng quyền với tsudev-trafficmonitor. Không tải và chạy mã từ mạng, không ghi ra ngoài thư mục cấu hình (`GetPluginConfigDir()`) khi không cần.
* Lịch sử thay đổi giao diện plugin (phiên bản 1-8) ở cuối tệp [`include/PluginInterface.h`](./include/PluginInterface.h).
