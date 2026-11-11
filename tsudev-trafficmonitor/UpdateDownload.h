#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////
// UpdateDownload.h - Tự tải gói cập nhật và kiểm SHA-256 (không phụ thuộc MFC).
//
// Dùng bởi CUpdateHelper::DownloadPackage (TsudevTrafficMonitor.cpp hỏi người dùng rồi gọi ở luồng nền).
// Không phụ thuộc MFC để CI chạy được kiểm thử tích hợp với gói phát hành thật trên GitHub
// (tests/UpdateDownloadTests.cpp, job "Kiểm thử C++" trong build.yml).
//
// Bảo vệ:
//   * Chỉ tải liên kết qua UpdateSecurity::IsTrustedDownloadUrl, chỉ lưu tệp .zip có tên an toàn.
//   * Tự theo chuyển hướng (tối đa 5 bước); MỌI bước phải là HTTPS tới host trong allowed_hosts.
//   * TLS luôn được kiểm (không cờ IGNORE_CERT_*), không cookie, không bộ nhớ đệm.
//   * Giới hạn kích thước (Content-Length và số byte thực nhận) và thời gian.
//   * Ghi ra tệp tạm "<tên>.part" (CREATE_NEW) cạnh tệp đích, băm SHA-256 trong lúc ghi.
//   * Khớp SHA-256 (đã ký trong version_signed.xml) mới đổi tên thành tệp đích, không ghi đè tệp có sẵn;
//     lệch hay lỗi thì xóa tệp tạm. Tệp lưu được gắn "Mark of the Web" (Zone.Identifier) như khi tải bằng
//     trình duyệt, để Windows SmartScreen vẫn kiểm chương trình giải nén từ gói.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <wininet.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <string>
#include <vector>
#include "UpdateSecurity.h"

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uuid.lib")      //FOLDERID_Downloads, CLSID_ProgressDialog

namespace UpdateDownload
{
    enum class Result
    {
        Ok,             //Đã tải, SHA-256 khớp, đã lưu vào thư mục đích
        Cancelled,      //Người dùng hủy
        HashMismatch,   //SHA-256 không khớp: tệp đã bị xóa, KHÔNG được mở
        TooLarge,       //Gói vượt giới hạn kích thước
        NetworkError,   //Lỗi mạng, mã HTTP khác 200, quá thời gian hoặc chuyển hướng tới máy chủ không được phép
        FileError,      //Không ghi được tệp vào thư mục đích
        Unsupported     //Liên kết/SHA-256 không đủ điều kiện để tự tải (nơi gọi dùng đường cũ: mở trình duyệt)
    };

    inline const wchar_t* ResultName(Result r)
    {
        switch (r)
        {
        case Result::Ok: return L"Ok";
        case Result::Cancelled: return L"Cancelled";
        case Result::HashMismatch: return L"HashMismatch";
        case Result::TooLarge: return L"TooLarge";
        case Result::NetworkError: return L"NetworkError";
        case Result::FileError: return L"FileError";
        case Result::Unsupported: return L"Unsupported";
        }
        return L"?";
    }

    //Hàm báo tiến độ: (số byte đã tải, tổng số byte hoặc 0 nếu chưa biết, context); trả về false để hủy
    typedef bool (*ProgressFn)(unsigned long long done, unsigned long long total, void* context);

    struct Options
    {
        std::wstring user_agent{ L"tsudev-trafficmonitor" };
        std::wstring allowed_hosts{ FORK_UPDATE_DOWNLOAD_HOSTS };
        unsigned long long max_bytes{ UpdateSecurity::MAX_UPDATE_PACKAGE_BYTES };
        DWORD network_timeout_ms{ 30000 };
        ULONGLONG total_timeout_ms{ 10 * 60 * 1000 };
        int max_redirects{ 5 };
        bool mark_of_the_web{ true };
        ProgressFn progress{ nullptr };
        void* context{ nullptr };
        std::wstring* log{ nullptr };       //Nếu khác nullptr: nhận mô tả lỗi (một dòng) để ghi nhật ký
    };

    //Thư mục Downloads của người dùng (theo vị trí đã đổi nếu có), kết thúc bằng '\'; rỗng nếu không lấy được
    inline std::wstring GetDownloadsFolder()
    {
        std::wstring dir;
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_DEFAULT, nullptr, &path)) && path != nullptr)
            dir = path;
        CoTaskMemFree(path);
        if (!dir.empty() && dir.back() != L'\\')
            dir.push_back(L'\\');
        return dir;
    }

    namespace Detail
    {
        class InternetHandle
        {
        public:
            InternetHandle() = default;
            ~InternetHandle() { Reset(nullptr); }
            InternetHandle(const InternetHandle&) = delete;
            InternetHandle& operator=(const InternetHandle&) = delete;
            void Reset(HINTERNET h)
            {
                if (m_h != nullptr)
                    InternetCloseHandle(m_h);
                m_h = h;
            }
            HINTERNET Get() const { return m_h; }
        private:
            HINTERNET m_h{};
        };

        inline bool PathExists(const std::wstring& path)
        {
            return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
        }

        //Đọc một tiêu đề HTTP dạng chuỗi (giới hạn 4096 ký tự)
        inline std::wstring QueryHeader(HINTERNET request, DWORD info)
        {
            std::vector<wchar_t> buffer(4097);
            DWORD len = static_cast<DWORD>((buffer.size() - 1) * sizeof(wchar_t));
            if (!HttpQueryInfoW(request, info, buffer.data(), &len, nullptr))
                return std::wstring();
            return std::wstring(buffer.data());
        }

        inline void Log(const Options& options, const std::wstring& text)
        {
            if (options.log != nullptr)
                *options.log = text;
        }

        //Gắn "Mark of the Web" như trình duyệt (ZoneId=3: Internet). Lỗi (ví dụ ổ FAT32) thì bỏ qua.
        inline void WriteMarkOfTheWeb(const std::wstring& path, const std::wstring& url)
        {
            std::string content = "[ZoneTransfer]\r\nZoneId=3\r\nHostUrl=";
            for (wchar_t ch : url)
                content.push_back(static_cast<char>(ch));   //URL đã qua IsHttpsUrl: chỉ ký tự in được
            content += "\r\n";
            HANDLE ads = CreateFileW((path + L":Zone.Identifier").c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (ads == INVALID_HANDLE_VALUE)
                return;
            DWORD written{};
            WriteFile(ads, content.data(), static_cast<DWORD>(content.size()), &written, nullptr);
            CloseHandle(ads);
        }
    }

    //Tải url vào thư mục dest_dir (kết thúc bằng '\'), kiểm SHA-256 với expected_sha256.
    //Ok: saved_path là đường dẫn tệp đã lưu. HashMismatch: actual_sha256 là SHA-256 nhận được.
    inline Result DownloadPackage(const std::wstring& url, const std::wstring& expected_sha256, const std::wstring& dest_dir,
        const Options& options, std::wstring& saved_path, std::wstring& actual_sha256)
    {
        using namespace Detail;
        saved_path.clear();
        actual_sha256.clear();

        const std::wstring file_name = UpdateSecurity::SafeFileNameFromUrl(url);
        if (file_name.empty() || !UpdateSecurity::IsSha256Hex(expected_sha256))
        {
            Log(options, L"DownloadPackage: unsupported link or SHA-256: " + url);
            return Result::Unsupported;
        }
        if (dest_dir.empty() || dest_dir.back() != L'\\' || !PathExists(dest_dir))
        {
            Log(options, L"DownloadPackage: destination folder not found: " + dest_dir);
            return Result::FileError;
        }
        UpdateSecurity::Sha256Stream hasher;
        if (!hasher.IsValid())
        {
            Log(options, L"DownloadPackage: SHA-256 provider unavailable");
            return Result::FileError;
        }

        //Kết nối, tự theo chuyển hướng và kiểm từng bước
        InternetHandle session;
        session.Reset(InternetOpenW(options.user_agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0));
        if (session.Get() == nullptr)
        {
            Log(options, L"DownloadPackage: InternetOpen failed, error " + std::to_wstring(GetLastError()));
            return Result::NetworkError;
        }
        DWORD timeout = options.network_timeout_ms;
        InternetSetOptionW(session.Get(), INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
        InternetSetOptionW(session.Get(), INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
        InternetSetOptionW(session.Get(), INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

        const DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE
            | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_NO_UI;
        InternetHandle request;
        std::wstring current = url;
        for (int hop = 0; ; hop++)
        {
            if (hop > options.max_redirects || !UpdateSecurity::IsTrustedHostUrl(current, options.allowed_hosts))
            {
                Log(options, L"DownloadPackage: redirect not allowed (hop " + std::to_wstring(hop) + L"): " + current.substr(0, 200));
                return Result::NetworkError;
            }
            request.Reset(InternetOpenUrlW(session.Get(), current.c_str(), nullptr, 0, flags, 0));
            if (request.Get() == nullptr)
            {
                Log(options, L"DownloadPackage: InternetOpenUrl failed, error " + std::to_wstring(GetLastError()));
                return Result::NetworkError;
            }
            DWORD status{};
            DWORD status_len = sizeof(status);
            if (!HttpQueryInfoW(request.Get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &status_len, nullptr))
            {
                Log(options, L"DownloadPackage: no HTTP status");
                return Result::NetworkError;
            }
            if (status == 301 || status == 302 || status == 303 || status == 307 || status == 308)
            {
                current = QueryHeader(request.Get(), HTTP_QUERY_LOCATION);     //URL tương đối/không HTTPS bị từ chối ở vòng sau
                continue;
            }
            if (status != 200)
            {
                Log(options, L"DownloadPackage: HTTP status " + std::to_wstring(status));
                return Result::NetworkError;
            }
            break;
        }

        //Kích thước khai báo (nếu có) không được vượt giới hạn
        unsigned long long total = 0;
        std::wstring content_length = QueryHeader(request.Get(), HTTP_QUERY_CONTENT_LENGTH);
        if (!content_length.empty())
            total = _wcstoui64(content_length.c_str(), nullptr, 10);
        if (total > options.max_bytes)
        {
            Log(options, L"DownloadPackage: package too large: " + content_length);
            return Result::TooLarge;
        }

        //Tệp tạm "<tên>.part" cạnh tệp đích; chọn tên chưa có để không ghi đè
        auto name_taken = [&](const std::wstring& name) {
            return PathExists(dest_dir + name) || PathExists(dest_dir + name + L".part");
        };
        std::wstring final_name = UpdateSecurity::UniqueFileName(file_name, name_taken);
        if (final_name.empty())
        {
            Log(options, L"DownloadPackage: no free file name for " + file_name);
            return Result::FileError;
        }
        const std::wstring part_path = dest_dir + final_name + L".part";
        HANDLE file = CreateFileW(part_path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE)
        {
            Log(options, L"DownloadPackage: cannot create " + part_path + L", error " + std::to_wstring(GetLastError()));
            return Result::FileError;
        }

        //Đọc theo khối: băm và ghi ra đĩa cùng lúc
        Result result = Result::Ok;
        std::vector<char> buffer(64 * 1024);
        unsigned long long done = 0;
        const ULONGLONG start = GetTickCount64();
        if (options.progress != nullptr && !options.progress(0, total, options.context))
            result = Result::Cancelled;
        while (result == Result::Ok)
        {
            DWORD read{};
            if (!InternetReadFile(request.Get(), buffer.data(), static_cast<DWORD>(buffer.size()), &read))
            {
                Log(options, L"DownloadPackage: read failed, error " + std::to_wstring(GetLastError()));
                result = Result::NetworkError;
                break;
            }
            if (read == 0)
                break;
            done += read;
            if (done > options.max_bytes)
            {
                Log(options, L"DownloadPackage: package exceeds size limit");
                result = Result::TooLarge;
                break;
            }
            DWORD written{};
            if (!hasher.Update(buffer.data(), read) || !WriteFile(file, buffer.data(), read, &written, nullptr) || written != read)
            {
                Log(options, L"DownloadPackage: write failed, error " + std::to_wstring(GetLastError()));
                result = Result::FileError;
                break;
            }
            if (options.progress != nullptr && !options.progress(done, total, options.context))
            {
                result = Result::Cancelled;
                break;
            }
            if (GetTickCount64() - start > options.total_timeout_ms)
            {
                Log(options, L"DownloadPackage: timed out");
                result = Result::NetworkError;
                break;
            }
        }
        if (result == Result::Ok && total != 0 && done != total)
        {
            Log(options, L"DownloadPackage: incomplete download " + std::to_wstring(done) + L"/" + std::to_wstring(total));
            result = Result::NetworkError;
        }
        if (result == Result::Ok && !FlushFileBuffers(file))
            result = Result::FileError;
        CloseHandle(file);
        request.Reset(nullptr);

        if (result != Result::Ok)
        {
            DeleteFileW(part_path.c_str());
            return result;
        }

        //So SHA-256 với giá trị đã ký; lệch thì xóa, không bao giờ giữ lại tệp
        actual_sha256 = hasher.FinishHex();
        if (!UpdateSecurity::Sha256Equals(actual_sha256, expected_sha256))
        {
            DeleteFileW(part_path.c_str());
            Log(options, L"DownloadPackage: SHA-256 mismatch, expected " + expected_sha256 + L", got " + actual_sha256);
            return Result::HashMismatch;
        }

        //Đổi tên thành tệp đích; không ghi đè (MoveFileEx không có MOVEFILE_REPLACE_EXISTING)
        for (int attempt = 0; attempt < 5 && !final_name.empty(); attempt++)
        {
            const std::wstring final_path = dest_dir + final_name;
            if (MoveFileExW(part_path.c_str(), final_path.c_str(), MOVEFILE_WRITE_THROUGH))
            {
                saved_path = final_path;
                break;
            }
            DWORD error = GetLastError();
            if (error != ERROR_ALREADY_EXISTS && error != ERROR_FILE_EXISTS)
                break;
            //Có tệp cùng tên vừa xuất hiện: chọn tên khác
            final_name = UpdateSecurity::UniqueFileName(file_name, [&](const std::wstring& name) { return PathExists(dest_dir + name); });
        }
        if (saved_path.empty())
        {
            DeleteFileW(part_path.c_str());
            Log(options, L"DownloadPackage: cannot rename " + part_path);
            return Result::FileError;
        }
        if (options.mark_of_the_web)
            WriteMarkOfTheWeb(saved_path, url);
        return Result::Ok;
    }
}
