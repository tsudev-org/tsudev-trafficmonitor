// Kiểm thử tự động cho logic thuần di trú tên sản phẩm (tsutramon/LegacyNames.h, ADR-0001 mục 3.4).
// Build và chạy trong CI (build.yml, job "Kiểm thử C++"); chạy tay:
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /sdl /I tsudev-trafficmonitor tests\LegacyNamesTests.cpp /Fe:LegacyNamesTests.exe
//   g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -I tsutramon tests/LegacyNamesTests.cpp

#include "LegacyNames.h"
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace LegacyNames;

static int g_failed = 0;
static int g_passed = 0;

static void Check(bool cond, const char* expr, int line)
{
    if (cond)
    {
        g_passed++;
    }
    else
    {
        g_failed++;
        std::cerr << "THAT BAI (dong " << line << "): " << expr << "\n";
    }
}
#define CHECK(x) Check((x), #x, __LINE__)

// Hệ thống giả: tập tệp tồn tại và ProductName của từng exe.
struct FakeSystem
{
    std::set<std::wstring> files;
    std::map<std::wstring, std::wstring> products;
    auto Exists() const { return [this](const std::wstring& p) { return files.count(p) > 0; }; }
    auto Product() const
    {
        return [this](const std::wstring& p) {
            auto it = products.find(p);
            return it == products.end() ? std::wstring() : it->second;
        };
    }
};

static void TestBangTen()
{
    // Tên hiện tại đúng ADR-0001 mục 3.4.
    CHECK(std::wstring(kCurrent.run_value) == L"TsuTramon");
    CHECK(std::wstring(kCurrent.appdata_dir) == L"tsutramon");
    CHECK(std::wstring(kCurrent.exe_name) == L"TsuTramon.exe");
    CHECK(std::wstring(kCurrent.product_name) == L"Tsu Tramon");
    CHECK(std::wstring(kCurrent.task_folder) == L"\\TsuTramon");
    // Đủ hai thế hệ cũ, mới nhất trước.
    CHECK(kLegacyCount == 2);
    CHECK(std::wstring(kLegacyGenerations[0].appdata_dir) == L"tsudev-trafficmonitor");
    CHECK(std::wstring(kLegacyGenerations[1].appdata_dir) == L"TrafficMonitor");
    // Cùng một loại định danh không trùng giữa các thế hệ (không thế hệ nào tự chặn/xoá chính mình).
    auto unique = [](const wchar_t* Generation::*field) {
        std::set<std::wstring> seen{ kCurrent.*field };
        for (const auto& g : kLegacyGenerations)
            seen.insert(g.*field);
        return seen.size() == kLegacyCount + 1;
    };
    CHECK(unique(&Generation::run_value));
    CHECK(unique(&Generation::mutex_release));
    CHECK(unique(&Generation::mutex_debug));
    CHECK(unique(&Generation::window_class));
    CHECK(unique(&Generation::product_name));
    for (const auto& g : kLegacyGenerations)
    {
        CHECK(!EqualsNoCase(g.appdata_dir, kCurrent.appdata_dir));
        CHECK(!EqualsNoCase(g.task_folder, kCurrent.task_folder));
        CHECK(!EqualsNoCase(g.exe_name, kCurrent.exe_name));
    }
    // Mutex/lớp cửa sổ của bản đã phát hành giữ đúng chuỗi cũ (đổi là mất khả năng chặn chạy song song).
    CHECK(std::wstring(kLegacyGenerations[0].mutex_release) == L"tsudev-trafficmonitor-1419J3XLKL1w8OZc");
    CHECK(std::wstring(kLegacyGenerations[0].window_class) == L"tsudev-trafficmonitor_r7XZaS4p");
    CHECK(std::wstring(kLegacyGenerations[1].mutex_release) == L"TrafficMonitor-1419J3XLKL1w8OZc");
    CHECK(std::wstring(kLegacyGenerations[1].window_class) == L"TrafficMonitor_r7XZaS4p");
}

static void TestTachDuongDan()
{
    CHECK(ExtractExePath(L"\"C:\\A B\\x.exe\"") == L"C:\\A B\\x.exe");
    CHECK(ExtractExePath(L"C:\\A\\x.exe") == L"C:\\A\\x.exe");
    CHECK(ExtractExePath(L"  \"C:\\A\\x.exe\"  ") == L"C:\\A\\x.exe");
    CHECK(ExtractExePath(L"\"C:\\A B\\x.exe\" -hide") == L"C:\\A B\\x.exe");
    CHECK(ExtractExePath(L"\"C:\\A\\x.exe") == L"C:\\A\\x.exe");    // thiếu ngoặc đóng
    CHECK(ExtractExePath(L"") == L"");
    CHECK(ExtractExePath(L"   ") == L"");
    CHECK(ExtractExePath(L"\"\"") == L"");
    CHECK(DirectoryOf(L"C:\\A\\x.exe") == L"C:\\A\\");
    CHECK(DirectoryOf(L"C:/A/x.exe") == L"C:/A/");
    CHECK(DirectoryOf(L"x.exe") == L"");
    CHECK(EqualsNoCase(L"C:\\Users\\An\\", L"c:\\users\\AN\\"));
    CHECK(!EqualsNoCase(L"C:\\a", L"C:\\b"));
    CHECK(!EqualsNoCase(L"C:\\a", L"C:\\a\\"));
}

static void TestTenSanPham()
{
    CHECK(IsOwnProductName(L"Tsu Tramon"));
    CHECK(IsOwnProductName(L"tsudev-trafficmonitor"));
    CHECK(IsOwnProductName(L"TrafficMonitor-VN"));
    CHECK(!IsOwnProductName(L"TrafficMonitor"));        // dự án gốc
    CHECK(!IsOwnProductName(L"trafficmonitor-vn"));     // phân biệt hoa thường
    CHECK(!IsOwnProductName(L""));
}

static void TestMucTuKhoiDong()
{
    const std::wstring module_dir = L"C:\\Apps\\TsuTramon\\";
    FakeSystem sys;
    sys.files = { L"C:\\Apps\\TsuTramon\\tsudev-trafficmonitor.exe", L"C:\\Old\\tsudev-trafficmonitor.exe",
                  L"C:\\Old\\TrafficMonitor.exe", L"C:\\Goc\\TrafficMonitor.exe", L"C:\\Khac\\x.exe" };
    sys.products = { { L"C:\\Old\\tsudev-trafficmonitor.exe", L"tsudev-trafficmonitor" },
                     { L"C:\\Old\\TrafficMonitor.exe", L"TrafficMonitor-VN" },
                     { L"C:\\Goc\\TrafficMonitor.exe", L"TrafficMonitor" } };
    auto own = [&](const std::wstring& cmd) { return IsOwnAutoRunTarget(cmd, module_dir, sys.Exists(), sys.Product()); };

    // Bản 1202 ở thư mục khác: nhận qua ProductName.
    CHECK(own(L"\"C:\\Old\\tsudev-trafficmonitor.exe\""));
    // Bản trước 1201 (TrafficMonitor-VN).
    CHECK(own(L"\"C:\\Old\\TrafficMonitor.exe\""));
    // Giải nén đè vào cùng thư mục: nhận theo thư mục (không phân biệt hoa thường).
    CHECK(own(L"\"c:\\apps\\tsutramon\\tsudev-trafficmonitor.exe\""));
    // exe đã bị xoá: mục mồ côi, nhận.
    CHECK(own(L"\"C:\\DaXoa\\tsudev-trafficmonitor.exe\""));
    // Dự án gốc TrafficMonitor còn cài: KHÔNG đụng.
    CHECK(!own(L"\"C:\\Goc\\TrafficMonitor.exe\""));
    // exe lạ không đọc được ProductName: KHÔNG đụng.
    CHECK(!own(L"\"C:\\Khac\\x.exe\""));
    // Lệnh rỗng: KHÔNG đụng.
    CHECK(!own(L""));
    CHECK(!own(L"\"\""));
    // module_dir rỗng (không xác định được) không được khớp mọi đường dẫn không có thư mục.
    FakeSystem sys2;
    sys2.files = { L"x.exe" };
    CHECK(!IsOwnAutoRunTarget(L"x.exe", L"", sys2.Exists(), sys2.Product()));
}

static void TestNguonDiTru()
{
    auto choose = [](std::set<std::wstring> have) {
        return ChooseAppDataMigrationSource([&](const std::wstring& d) { return have.count(d) > 0; });
    };
    // Máy mới, chưa có gì: không chép.
    CHECK(choose({}) == -1);
    // Đã có thư mục mới: không chép, kể cả khi còn thư mục cũ.
    CHECK(choose({ L"tsutramon" }) == -1);
    CHECK(choose({ L"tsutramon", L"tsudev-trafficmonitor", L"TrafficMonitor" }) == -1);
    // Nâng từ 1201/1202.
    CHECK(choose({ L"tsudev-trafficmonitor" }) == 0);
    // Nâng thẳng từ bản trước 1201.
    CHECK(choose({ L"TrafficMonitor" }) == 1);
    // Cả hai thế hệ cũ: lấy bản mới nhất (1202 đã chép từ TrafficMonitor và có lịch sử mới hơn).
    CHECK(choose({ L"tsudev-trafficmonitor", L"TrafficMonitor" }) == 0);
    // Hàm hỏi đúng thứ tự: thư mục mới trước, dừng ở thế hệ cũ đầu tiên có cấu hình.
    std::vector<std::wstring> asked;
    int r = ChooseAppDataMigrationSource([&](const std::wstring& d) { asked.push_back(d); return d == L"tsudev-trafficmonitor"; });
    CHECK(r == 0);
    CHECK(asked.size() == 2);
    CHECK(asked.size() == 2 && asked[0] == L"tsutramon" && asked[1] == L"tsudev-trafficmonitor");
}

int main()
{
    TestBangTen();
    TestTachDuongDan();
    TestTenSanPham();
    TestMucTuKhoiDong();
    TestNguonDiTru();
    std::cout << "LegacyNamesTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed == 0 ? 0 : 1;
}
