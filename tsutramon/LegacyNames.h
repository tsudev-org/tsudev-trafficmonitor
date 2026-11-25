#pragma once
// Định danh theo từng thế hệ tên của sản phẩm và logic thuần chọn nguồn di trú (ADR-0001 mục 3.4).
// KHÔNG phụ thuộc MFC/Win32 để kiểm thử được trên mọi nền tảng (tests/LegacyNamesTests.cpp, job "Kiểm thử C++").
//
// Thế hệ tên:
//   1. TrafficMonitor-VN (trước 26.11.1201): Run "TrafficMonitor", %AppData%\TrafficMonitor, tác vụ \TrafficMonitor
//   2. tsudev-trafficmonitor (26.11.1201 - 26.11.1202): Run/%AppData%/tác vụ "tsudev-trafficmonitor"
//   3. Tsu Tramon (hiện tại): Run "TsuTramon", %AppData%\tsutramon, tác vụ \TsuTramon, exe TsuTramon.exe
// Thêm thế hệ mới: đẩy thế hệ hiện tại vào ĐẦU kLegacyGenerations (mới nhất trước), rồi sửa kCurrent.

#include <cstddef>
#include <string>

namespace LegacyNames
{
    struct Generation
    {
        const wchar_t* run_value;       // tên giá trị trong HKCU\Software\Microsoft\Windows\CurrentVersion\Run
        const wchar_t* appdata_dir;     // thư mục con trong %AppData% (Roaming)
        const wchar_t* task_folder;     // thư mục tác vụ tự khởi động trong Task Scheduler
        const wchar_t* mutex_release;   // mutex chặn chạy nhiều thực thể (bản Release)
        const wchar_t* mutex_debug;     // mutex của bản Debug
        const wchar_t* window_class;    // lớp cửa sổ chính
        const wchar_t* product_name;    // ProductName trong thông tin phiên bản của exe
        const wchar_t* exe_name;        // tên tệp exe
    };

    inline constexpr Generation kCurrent{
        L"TsuTramon", L"tsutramon", L"\\TsuTramon",
        L"TsuTramon-1419J3XLKL1w8OZc", L"TsuTramon-e8Ahk24HP6JC8hDy",
        L"TsuTramon_r7XZaS4p", L"Tsu Tramon", L"TsuTramon.exe" };

    // Mới nhất trước: khi nhiều thế hệ cùng có dữ liệu thì lấy dữ liệu của bản gần nhất.
    inline constexpr Generation kLegacyGenerations[] = {
        { L"tsudev-trafficmonitor", L"tsudev-trafficmonitor", L"\\tsudev-trafficmonitor",
          L"tsudev-trafficmonitor-1419J3XLKL1w8OZc", L"tsudev-trafficmonitor-e8Ahk24HP6JC8hDy",
          L"tsudev-trafficmonitor_r7XZaS4p", L"tsudev-trafficmonitor", L"tsudev-trafficmonitor.exe" },
        { L"TrafficMonitor", L"TrafficMonitor", L"\\TrafficMonitor",
          L"TrafficMonitor-1419J3XLKL1w8OZc", L"TrafficMonitor-e8Ahk24HP6JC8hDy",
          L"TrafficMonitor_r7XZaS4p", L"TrafficMonitor-VN", L"TrafficMonitor.exe" },
    };
    inline constexpr std::size_t kLegacyCount = sizeof(kLegacyGenerations) / sizeof(kLegacyGenerations[0]);

    // So sánh không phân biệt hoa thường (chỉ chữ ASCII, đủ cho đường dẫn so với chính nó và tên sản phẩm).
    inline bool EqualsNoCase(const std::wstring& a, const std::wstring& b)
    {
        if (a.size() != b.size())
            return false;
        for (std::size_t i = 0; i < a.size(); i++)
        {
            wchar_t x = a[i], y = b[i];
            if (x >= L'A' && x <= L'Z') x = static_cast<wchar_t>(x - L'A' + L'a');
            if (y >= L'A' && y <= L'Z') y = static_cast<wchar_t>(y - L'A' + L'a');
            if (x != y)
                return false;
        }
        return true;
    }

    // Lấy đường dẫn exe từ lệnh lưu trong Run/tác vụ: bỏ khoảng trắng hai đầu và một cặp ngoặc kép bao ngoài.
    // Lệnh có tham số sau dấu ngoặc đóng ("C:\a b\x.exe" -arg) chỉ giữ phần trong ngoặc.
    inline std::wstring ExtractExePath(const std::wstring& command)
    {
        std::size_t begin = command.find_first_not_of(L" \t");
        if (begin == std::wstring::npos)
            return std::wstring();
        std::size_t end = command.find_last_not_of(L" \t");
        std::wstring s = command.substr(begin, end - begin + 1);
        if (!s.empty() && s.front() == L'"')
        {
            std::size_t close = s.find(L'"', 1);
            if (close == std::wstring::npos)
                return s.substr(1);
            return s.substr(1, close - 1);
        }
        return s;
    }

    // Thư mục chứa tệp (gồm dấu phân cách cuối), rỗng nếu không có dấu phân cách.
    inline std::wstring DirectoryOf(const std::wstring& path)
    {
        std::size_t index = path.find_last_of(L"\\/");
        if (index == std::wstring::npos)
            return std::wstring();
        return path.substr(0, index + 1);
    }

    // ProductName có phải của chính phần mềm này (mọi thế hệ tên) không. Phân biệt chữ hoa: "TrafficMonitor"
    // (dự án gốc, người dùng có thể cài riêng) KHÔNG phải của mình.
    inline bool IsOwnProductName(const std::wstring& product)
    {
        if (product == kCurrent.product_name)
            return true;
        for (const auto& g : kLegacyGenerations)
        {
            if (product == g.product_name)
                return true;
        }
        return false;
    }

    // Mục tự khởi động (Run/tác vụ) mang tên của thế hệ cũ có phải của mình để chuyển sang tên mới không.
    // Tên cũ "TrafficMonitor" có thể thuộc dự án gốc do người dùng cài riêng, nên chỉ nhận khi:
    //   - exe đích không còn (mục mồ côi, chuyển cũng không hại), hoặc
    //   - exe đích nằm cùng thư mục với exe đang chạy, hoặc
    //   - ProductName của exe đích là một trong các tên của phần mềm này.
    // file_exists/product_of: hàm hỏi hệ thống (thật trong app, giả trong test).
    template <typename FileExists, typename ProductOf>
    bool IsOwnAutoRunTarget(const std::wstring& command, const std::wstring& module_dir,
                            FileExists file_exists, ProductOf product_of)
    {
        std::wstring path = ExtractExePath(command);
        if (path.empty())
            return false;
        if (!file_exists(path))
            return true;
        if (!module_dir.empty() && EqualsNoCase(DirectoryOf(path), module_dir))
            return true;
        return IsOwnProductName(product_of(path));
    }

    // Chọn thư mục %AppData% cũ để chép sang thư mục mới ở lần chạy đầu sau khi đổi tên.
    // has_config(tên thư mục con) cho biết thư mục đó có config.ini không.
    // Trả về chỉ số trong kLegacyGenerations, hoặc -1 nếu không cần chép (thư mục mới đã có cấu hình,
    // hoặc không thế hệ cũ nào có cấu hình). Thế hệ mới nhất có cấu hình được ưu tiên.
    template <typename HasConfig>
    int ChooseAppDataMigrationSource(HasConfig has_config)
    {
        if (has_config(std::wstring(kCurrent.appdata_dir)))
            return -1;
        for (std::size_t i = 0; i < kLegacyCount; i++)
        {
            if (has_config(std::wstring(kLegacyGenerations[i].appdata_dir)))
                return static_cast<int>(i);
        }
        return -1;
    }
}
