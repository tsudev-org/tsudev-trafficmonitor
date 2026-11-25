#pragma once
// Tầng nền tảng (Windows) của đăng nhập tsudev cho Tsu Tramon (P9 phần 2): HTTPS tới tsudev.com, cổng quay về loopback,
// kho bí mật của hệ điều hành. Logic thuần (URL, JSON, JWS) ở ProAuth.h. KHÔNG phụ thuộc MFC; kiểm thử ở
// tests/ProNetTests.cpp (không gọi mạng ra ngoài - chỉ 127.0.0.1 và Credential Manager của tài khoản chạy test).
//
// Quy tắc (AUTH_AND_ACCOUNT mục 4, 14.1-14.2):
//  - Chỉ gọi mạng khi người dùng bấm đăng nhập/đã đăng nhập; bản miễn phí chưa đăng nhập không gọi gì.
//  - HTTPS tới ĐÚNG máy chủ tsudev.com, KHÔNG theo chuyển hướng (đích token/JWKS không được đổi giữa đường),
//    TLS >= 1.2, giới hạn kích thước phản hồi.
//  - Mở trình duyệt hệ thống, hứng mã ở http://127.0.0.1:<cổng ngẫu nhiên>/callback - KHÔNG WebView.
//  - Refresh token lưu trong Windows Credential Manager (CRED_TYPE_GENERIC, theo người dùng), KHÔNG lưu tệp cấu hình.

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <winhttp.h>
#include <wincred.h>
#include <chrono>
#include <string>
#include <vector>
#include "ProAuth.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "advapi32.lib")

namespace ProNet
{
    inline constexpr size_t kMaxResponse = 64 * 1024;
    inline constexpr const wchar_t* kHost = L"tsudev.com";
    // Tên mục trong Credential Manager. Đổi tên = mất phiên đăng nhập cũ (người dùng đăng nhập lại), không mất dữ liệu.
    inline constexpr const wchar_t* kCredentialTarget = L"tsudev/tsutramon/refresh_token";

    inline std::wstring Widen(const std::string& s)
    {
        if (s.empty()) return std::wstring();
        int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
        std::wstring w(static_cast<size_t>(n > 0 ? n : 0), L'\0');
        if (n > 0) MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), &w[0], n);
        return w;
    }

    // Đích hợp lệ: FORK_TSUDEV_HTTPS_ORIGIN + <đường dẫn> (không cổng lạ, không thông tin đăng nhập trong URL, không máy chủ khác).
    // Trả phần đường dẫn + truy vấn, rỗng nếu không hợp lệ.
    inline std::string PathIfTsudevUrl(const std::string& url)
    {
        static const std::string prefix = FORK_TSUDEV_HTTPS_ORIGIN;
        if (url.compare(0, prefix.size(), prefix) != 0)
            return std::string();
        std::string path = url.substr(prefix.size() - 1);
        for (char c : path)
            if (static_cast<unsigned char>(c) <= 0x20 || c == '\\' || c == '#' || static_cast<unsigned char>(c) >= 0x7F)
                return std::string();
        return path;
    }

    struct HttpResult
    {
        bool ok = false;          // có phản hồi HTTP đọc trọn (mọi mã trạng thái)
        DWORD status = 0;
        std::string body;
        std::wstring error;       // mô tả ngắn khi ok == false (ghi nhật ký, không hiện token)
    };

    // method: L"GET" | L"POST". headers: các dòng "Tên: giá trị\r\n" (Authorization, X-Tsu-Installation...).
    inline HttpResult HttpsRequest(const wchar_t* method, const std::string& url, const std::wstring& headers,
                                   const std::string& body, const std::wstring& user_agent = L"TsuTramon")
    {
        HttpResult r;
        const std::string path = PathIfTsudevUrl(url);
        if (path.empty())
        {
            r.error = L"URL không thuộc máy chủ tsudev.com";
            return r;
        }
        HINTERNET session = WinHttpOpen(user_agent.c_str(), WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session)
            session = WinHttpOpen(user_agent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        HINTERNET conn = nullptr, req = nullptr;
        if (session)
        {
            DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
#ifdef WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3
            protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3;
#endif
            if (!WinHttpSetOption(session, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, sizeof(protocols)))
            {
                protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;   // Windows cũ chưa biết cờ TLS 1.3
                WinHttpSetOption(session, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, sizeof(protocols));
            }
            WinHttpSetTimeouts(session, 10000, 10000, 15000, 15000);
            conn = WinHttpConnect(session, kHost, INTERNET_DEFAULT_HTTPS_PORT, 0);
        }
        if (conn)
            req = WinHttpOpenRequest(conn, method, Widen(path).c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (req)
        {
            DWORD policy = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
            WinHttpSetOption(req, WINHTTP_OPTION_REDIRECT_POLICY, &policy, sizeof(policy));
            DWORD no_cookies = WINHTTP_DISABLE_COOKIES;
            WinHttpSetOption(req, WINHTTP_OPTION_DISABLE_FEATURE, &no_cookies, sizeof(no_cookies));
            std::wstring all_headers = headers;
            if (!body.empty())
                all_headers += L"Content-Type: application/x-www-form-urlencoded\r\n";
            all_headers += L"Accept: application/json\r\n";
            BOOL sent = WinHttpSendRequest(req, all_headers.c_str(), static_cast<DWORD>(-1L),
                body.empty() ? WINHTTP_NO_REQUEST_DATA : const_cast<char*>(body.data()),
                static_cast<DWORD>(body.size()), static_cast<DWORD>(body.size()), 0);
            if (sent && WinHttpReceiveResponse(req, nullptr))
            {
                DWORD size = sizeof(r.status);
                WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &r.status, &size, WINHTTP_NO_HEADER_INDEX);
                bool too_big = false;
                for (;;)
                {
                    DWORD avail = 0;
                    if (!WinHttpQueryDataAvailable(req, &avail)) { r.error = L"đọc phản hồi lỗi"; break; }
                    if (avail == 0) { r.ok = true; break; }
                    if (r.body.size() + avail > kMaxResponse) { too_big = true; break; }
                    std::vector<char> buf(avail);
                    DWORD read = 0;
                    if (!WinHttpReadData(req, buf.data(), avail, &read)) { r.error = L"đọc phản hồi lỗi"; break; }
                    r.body.append(buf.data(), read);
                }
                if (too_big)
                {
                    r.ok = false;
                    r.body.clear();
                    r.error = L"phản hồi quá lớn";
                }
            }
            else
                r.error = L"không kết nối được tsudev.com (mã " + std::to_wstring(GetLastError()) + L")";
        }
        else if (r.error.empty())
            r.error = L"không mở được kết nối HTTPS";
        if (req) WinHttpCloseHandle(req);
        if (conn) WinHttpCloseHandle(conn);
        if (session) WinHttpCloseHandle(session);
        return r;
    }

    // ---------------------------------------------------------------------------------------------------------
    // Cổng quay về loopback: nghe 127.0.0.1:<cổng do hệ điều hành chọn>, nhận đúng một callback hợp lệ.
    // ---------------------------------------------------------------------------------------------------------
    class LoopbackReceiver
    {
    public:
        LoopbackReceiver()
        {
            WSADATA wsa;
            m_wsa = WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
            if (!m_wsa) return;
            m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (m_sock == INVALID_SOCKET) return;
            BOOL excl = TRUE;   // không cho tiến trình khác bind chồng cùng cổng
            setsockopt(m_sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<const char*>(&excl), sizeof(excl));
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            addr.sin_port = 0;
            int len = sizeof(addr);
            if (bind(m_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0
                || listen(m_sock, 4) != 0
                || getsockname(m_sock, reinterpret_cast<sockaddr*>(&addr), &len) != 0)
            {
                Close();
                return;
            }
            m_port = ntohs(addr.sin_port);
        }
        ~LoopbackReceiver() { Close(); if (m_wsa) WSACleanup(); }
        LoopbackReceiver(const LoopbackReceiver&) = delete;
        LoopbackReceiver& operator=(const LoopbackReceiver&) = delete;

        bool IsListening() const { return m_sock != INVALID_SOCKET && m_port != 0; }
        unsigned Port() const { return m_port; }

        // Chờ tới khi có callback hợp lệ (code hoặc error) hoặc hết thời gian / *cancel = true.
        // Yêu cầu khác (favicon, đường lạ) nhận 404 và tiếp tục chờ.
        ProAuth::CallbackResult Wait(int timeout_ms, const volatile bool* cancel = nullptr,
                                     const std::string& success_html = DefaultPage(true),
                                     const std::string& failure_html = DefaultPage(false))
        {
            ProAuth::CallbackResult none;
            if (!IsListening()) return none;
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
            while (std::chrono::steady_clock::now() < deadline && !(cancel && *cancel))
            {
                fd_set set;
                FD_ZERO(&set);
                FD_SET(m_sock, &set);
                timeval tv{ 0, 250 * 1000 };
                int sel = select(0, &set, nullptr, nullptr, &tv);
                if (sel <= 0) continue;
                SOCKET c = accept(m_sock, nullptr, nullptr);
                if (c == INVALID_SOCKET) continue;
                DWORD rcv_to = 5000;
                setsockopt(c, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&rcv_to), sizeof(rcv_to));
                std::string head;
                char buf[1024];
                while (head.find("\r\n") == std::string::npos && head.size() < 8192)
                {
                    int n = recv(c, buf, sizeof(buf), 0);
                    if (n <= 0) break;
                    head.append(buf, static_cast<size_t>(n));
                }
                size_t eol = head.find("\r\n");
                ProAuth::CallbackResult r = eol == std::string::npos ? none : ProAuth::ParseCallbackRequestLine(head.substr(0, eol));
                if (r.ok)
                    Respond(c, "200 OK", r.error.empty() ? success_html : failure_html);
                else
                    Respond(c, "404 Not Found", "<!doctype html><meta charset=\"utf-8\"><title>404</title>");
                closesocket(c);
                if (r.ok)
                    return r;
            }
            return none;
        }

        static std::string DefaultPage(bool ok)
        {
            // Trang tĩnh, không script, không tài nguyên ngoài (CSP chặn hết). UTF-8.
            const char* msg = ok ? "\xC4\x90\xC3\xA3 \xC4\x91\xC4\x83ng nh\xE1\xBA\xADp. B\xE1\xBA\xA1n c\xC3\xB3 th\xE1\xBB\x83 \xC4\x91\xC3\xB3ng th\xE1\xBA\xBB n\xC3\xA0y v\xC3\xA0 quay l\xE1\xBA\xA1i Tsu Tramon."
                                 : "\xC4\x90\xC4\x83ng nh\xE1\xBA\xADp ch\xC6\xB0\x61 ho\xC3\xA0n t\xE1\xBA\xA5t. Quay l\xE1\xBA\xA1i Tsu Tramon \xC4\x91\xE1\xBB\x83 th\xE1\xBB\xAD l\xE1\xBA\xA1i.";
            return std::string("<!doctype html><html lang=\"vi\"><meta charset=\"utf-8\"><title>Tsu Tramon</title>"
                "<body style=\"font-family:system-ui,sans-serif;padding:3rem;max-width:36rem;margin:auto\"><p>") + msg + "</p></body></html>";
        }

    private:
        SOCKET m_sock = INVALID_SOCKET;
        unsigned m_port = 0;
        bool m_wsa = false;

        void Close()
        {
            if (m_sock != INVALID_SOCKET) { closesocket(m_sock); m_sock = INVALID_SOCKET; }
        }
        static void Respond(SOCKET c, const char* status, const std::string& html)
        {
            std::string resp = std::string("HTTP/1.1 ") + status + "\r\nContent-Type: text/html; charset=utf-8\r\n"
                "Content-Security-Policy: default-src 'none'; style-src 'unsafe-inline'\r\n"
                "Cache-Control: no-store\r\nReferrer-Policy: no-referrer\r\nConnection: close\r\nContent-Length: "
                + std::to_string(html.size()) + "\r\n\r\n" + html;
            send(c, resp.data(), static_cast<int>(resp.size()), 0);
        }
    };

    // ---------------------------------------------------------------------------------------------------------
    // Refresh token trong Windows Credential Manager (theo người dùng Windows; AUTH 14.2)
    // ---------------------------------------------------------------------------------------------------------
    inline bool SaveSecret(const std::string& secret, const wchar_t* target = kCredentialTarget)
    {
        if (secret.empty() || secret.size() > CRED_MAX_CREDENTIAL_BLOB_SIZE)
            return false;
        CREDENTIALW cred{};
        cred.Type = CRED_TYPE_GENERIC;
        cred.TargetName = const_cast<LPWSTR>(target);
        cred.CredentialBlobSize = static_cast<DWORD>(secret.size());
        cred.CredentialBlob = reinterpret_cast<LPBYTE>(const_cast<char*>(secret.data()));
        cred.Persist = CRED_PERSIST_LOCAL_MACHINE;   // chỉ máy này, không đi theo hồ sơ chuyển vùng
        cred.UserName = const_cast<LPWSTR>(L"tsutramon");
        return CredWriteW(&cred, 0) != FALSE;
    }

    inline bool LoadSecret(std::string& secret, const wchar_t* target = kCredentialTarget)
    {
        secret.clear();
        PCREDENTIALW cred = nullptr;
        if (!CredReadW(target, CRED_TYPE_GENERIC, 0, &cred) || !cred)
            return false;
        secret.assign(reinterpret_cast<const char*>(cred->CredentialBlob), cred->CredentialBlobSize);
        SecureZeroMemory(cred->CredentialBlob, cred->CredentialBlobSize);
        CredFree(cred);
        return !secret.empty();
    }

    // "Đăng xuất và xóa dữ liệu cục bộ" (AUTH 14.2): không có mục cũng coi là đã xoá
    inline bool DeleteSecret(const wchar_t* target = kCredentialTarget)
    {
        return CredDeleteW(target, CRED_TYPE_GENERIC, 0) != FALSE || GetLastError() == ERROR_NOT_FOUND;
    }
}

#endif // _WIN32
