// Kiểm thử tích hợp: tự tải gói cập nhật và kiểm SHA-256 (tsudev-trafficmonitor/UpdateDownload.h) với gói phát hành THẬT
// trên GitHub. Cần mạng; chạy trong CI (build.yml, job "Kiểm thử C++"). Chạy tay trên Windows:
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /I tsudev-trafficmonitor tests\UpdateDownloadTests.cpp /Fe:UpdateDownloadTests.exe
//   UpdateDownloadTests.exe
// Gói dùng để thử: bản 26.11.0904 x64 (tag bất biến), SHA-256 lấy từ SHA256SUMS.txt của bản phát hành đó.
// Liên kết dùng tiền tố owner cũ tsudev-tsudev như version_signed.xml, nên còn thử luôn chuỗi chuyển hướng
// github.com (owner cũ) -> github.com (tsudev-org) -> máy chủ tệp đính kèm của GitHub.

#include "UpdateDownload.h"
#include <cwctype>
#include <iostream>

using namespace UpdateDownload;

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

static const wchar_t* const PACKAGE_URL =
    L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/V26.11.0904-vn/tsudev-trafficmonitor_26.11.0904_x64_Lite.zip";
static const wchar_t* const PACKAGE_NAME = L"tsudev-trafficmonitor_26.11.0904_x64_Lite.zip";
static const wchar_t* const PACKAGE_SHA256 = L"89e1601002cbc8dd147976cf8b69042a2550f227065886843c3d7f7cb33724d6";
static const unsigned long long PACKAGE_SIZE = 1075148;

static bool Exists(const std::wstring& path)
{
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

//Số tệp trong thư mục (kể cả tệp .part còn sót)
static int CountFiles(const std::wstring& dir)
{
    int count = 0;
    WIN32_FIND_DATAW data{};
    HANDLE find = FindFirstFileW((dir + L"*").c_str(), &data);
    if (find == INVALID_HANDLE_VALUE)
        return 0;
    do
    {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            count++;
    } while (FindNextFileW(find, &data));
    FindClose(find);
    return count;
}

//SHA-256 của tệp trên đĩa
static std::wstring FileSha256(const std::wstring& path)
{
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return std::wstring();
    UpdateSecurity::Sha256Stream hasher;
    std::vector<char> buffer(65536);
    DWORD read{};
    while (ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr) && read > 0)
        hasher.Update(buffer.data(), read);
    CloseHandle(file);
    return hasher.FinishHex();
}

static unsigned long long g_progress_calls = 0;
static unsigned long long g_progress_last = 0;
static bool CountProgress(unsigned long long done, unsigned long long total, void*)
{
    g_progress_calls++;
    g_progress_last = done;
    return total == 0 || done <= total;
}
static bool CancelAfterFirstBlock(unsigned long long done, unsigned long long, void*)
{
    return done == 0;
}

//Chạy một lần; thử lại tối đa 3 lần nếu lỗi mạng (runner CI đôi khi mất kết nối thoáng qua)
static Result Download(const std::wstring& url, const std::wstring& sha, const std::wstring& dir, Options options,
    std::wstring& saved, std::wstring& actual, bool retry_network = true)
{
    std::wstring log;
    options.log = &log;
    options.user_agent = L"tsudev-trafficmonitor-tests";
    Result r = Result::NetworkError;
    for (int attempt = 0; attempt < 3; attempt++)
    {
        log.clear();
        r = DownloadPackage(url, sha, dir, options, saved, actual);
        if (r != Result::NetworkError || !retry_network)
            break;
        std::wcerr << L"  (thu lai sau loi mang: " << log << L")\n";
        Sleep(3000);
    }
    std::wcout << L"  " << ResultName(r) << (log.empty() ? L"" : L": ") << log << L"\n";
    return r;
}

int wmain()
{
    //Thư mục tạm riêng cho lần chạy
    wchar_t temp[MAX_PATH]{};
    GetTempPathW(MAX_PATH, temp);
    std::wstring dir = std::wstring(temp) + L"tmvn-download-test-" + std::to_wstring(GetCurrentProcessId()) + L"\\";
    CHECK(CreateDirectoryW(dir.c_str(), nullptr) != FALSE);
    CHECK(!GetDownloadsFolder().empty());

    std::wstring saved, actual;
    Options options;

    //1. SHA-256 đúng: lưu đúng tên, nội dung khớp, không còn .part, có Mark of the Web
    std::wcout << L"1. SHA-256 dung\n";
    options.progress = CountProgress;
    CHECK(Download(PACKAGE_URL, PACKAGE_SHA256, dir, options, saved, actual) == Result::Ok);
    CHECK(saved == dir + PACKAGE_NAME);
    CHECK(UpdateSecurity::Sha256Equals(actual, PACKAGE_SHA256));
    CHECK(UpdateSecurity::Sha256Equals(FileSha256(saved), PACKAGE_SHA256));
    CHECK(!Exists(saved + L".part"));
    CHECK(Exists(saved + L":Zone.Identifier"));
    CHECK(g_progress_calls > 1 && g_progress_last == PACKAGE_SIZE);
    CHECK(CountFiles(dir) == 1);
    options.progress = nullptr;

    //2. Tải lại: không ghi đè, lưu thành "(2)"; SHA viết hoa vẫn khớp
    std::wcout << L"2. Khong ghi de\n";
    std::wstring upper = PACKAGE_SHA256;
    for (auto& ch : upper)
        ch = static_cast<wchar_t>(towupper(ch));
    CHECK(Download(PACKAGE_URL, upper, dir, options, saved, actual) == Result::Ok);
    CHECK(saved == dir + L"tsudev-trafficmonitor_26.11.0904_x64_Lite (2).zip");
    CHECK(CountFiles(dir) == 2);

    //3. SHA-256 lệch: HashMismatch, tệp tạm bị xóa, không có tệp mới
    std::wcout << L"3. SHA-256 lech\n";
    std::wstring wrong = PACKAGE_SHA256;
    wrong[0] = (wrong[0] == L'0') ? L'1' : L'0';
    CHECK(Download(PACKAGE_URL, wrong, dir, options, saved, actual) == Result::HashMismatch);
    CHECK(saved.empty());
    CHECK(UpdateSecurity::Sha256Equals(actual, PACKAGE_SHA256));
    CHECK(CountFiles(dir) == 2);

    //4. Vượt giới hạn kích thước (theo Content-Length)
    std::wcout << L"4. Qua gioi han kich thuoc\n";
    options.max_bytes = 1000;
    CHECK(Download(PACKAGE_URL, PACKAGE_SHA256, dir, options, saved, actual) == Result::TooLarge);
    CHECK(CountFiles(dir) == 2);
    options.max_bytes = UpdateSecurity::MAX_UPDATE_PACKAGE_BYTES;

    //5. Người dùng hủy giữa chừng: xóa tệp tạm
    std::wcout << L"5. Huy\n";
    options.progress = CancelAfterFirstBlock;
    CHECK(Download(PACKAGE_URL, PACKAGE_SHA256, dir, options, saved, actual) == Result::Cancelled);
    CHECK(CountFiles(dir) == 2);
    options.progress = nullptr;

    //6. Chuyển hướng tới máy chủ ngoài danh sách: chỉ cho github.com thì bị chặn ở bước tới máy chủ tệp đính kèm
    std::wcout << L"6. Chuyen huong bi chan\n";
    options.allowed_hosts = L"github.com";
    CHECK(Download(PACKAGE_URL, PACKAGE_SHA256, dir, options, saved, actual, false) == Result::NetworkError);
    CHECK(CountFiles(dir) == 2);
    options.allowed_hosts = FORK_UPDATE_DOWNLOAD_HOSTS;

    //7. Liên kết không được tin hoặc không phải .zip, SHA sai định dạng: không kết nối mạng
    std::wcout << L"7. Lien ket/SHA khong hop le\n";
    CHECK(Download(L"https://github.com/evil/repo/releases/download/V1/x.zip", PACKAGE_SHA256, dir, options, saved, actual, false) == Result::Unsupported);
    CHECK(Download(L"https://github.com/tsudev-org/tsudev-trafficmonitor/releases/download/V1/setup.exe", PACKAGE_SHA256, dir, options, saved, actual, false) == Result::Unsupported);
    CHECK(Download(PACKAGE_URL, L"", dir, options, saved, actual, false) == Result::Unsupported);
    CHECK(Download(PACKAGE_URL, std::wstring(PACKAGE_SHA256).substr(1), dir, options, saved, actual, false) == Result::Unsupported);

    //8. Tệp không tồn tại (404) và thư mục đích không tồn tại
    std::wcout << L"8. 404 va thu muc dich sai\n";
    CHECK(Download(L"https://github.com/tsudev-org/tsudev-trafficmonitor/releases/download/V26.11.0904-vn/khong-ton-tai.zip",
        PACKAGE_SHA256, dir, options, saved, actual, false) == Result::NetworkError);
    CHECK(Download(PACKAGE_URL, PACKAGE_SHA256, dir + L"khong-co\\", options, saved, actual, false) == Result::FileError);
    CHECK(Download(PACKAGE_URL, PACKAGE_SHA256, L"", options, saved, actual, false) == Result::FileError);
    CHECK(CountFiles(dir) == 2);

    //Dọn thư mục tạm
    DeleteFileW((dir + PACKAGE_NAME).c_str());
    DeleteFileW((dir + L"tsudev-trafficmonitor_26.11.0904_x64_Lite (2).zip").c_str());
    RemoveDirectoryW(dir.c_str());

    std::cout << "UpdateDownloadTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed == 0 ? 0 : 1;
}
