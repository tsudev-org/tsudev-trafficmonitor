// Kiểm thử luồng tài khoản tsudev của Tsu Tramon (tsudev-trafficmonitor/ProAccount.cpp, P9 phần 3). CHỈ Windows.
// Máy chủ tsudev.com GIẢ (tiêm qua ProAccountDeps.http) + "trình duyệt" giả gọi cổng loopback 127.0.0.1 thật;
// kho bí mật trong bộ nhớ; chữ ký giả (64 byte 'S'). KHÔNG gọi mạng ra ngoài, KHÔNG đụng Credential Manager.
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /sdl /I tsudev-trafficmonitor tests\ProAccountTests.cpp tsudev-trafficmonitor\ProAccount.cpp /Fe:ProAccountTests.exe
//
// Phủ: AUTH 14.1 (chưa đăng nhập / ngoại tuyến => 0 lượt gọi mạng), 14.2 (refresh token chỉ trong kho bí mật, đăng xuất
// xoá sạch, thu hồi), 14.3 (X-Tsu-Installation chỉ khi đã đăng nhập), state/iss/nonce, xoay vòng refresh token,
// invalid_grant => về chế độ khách, đệm JWS kiểm lại chữ ký khi khởi động lại.

#include "ProNet.h"
#include "ProAccount.h"
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace ProAuth;

static int g_failed = 0;
static int g_passed = 0;

static void Check(bool cond, const char* expr, int line)
{
    if (cond)
        g_passed++;
    else
    {
        g_failed++;
        std::cerr << "THAT BAI (dong " << line << "): " << expr << "\n";
    }
}
#define CHECK(x) Check((x), #x, __LINE__)

static const int64_t T0 = 1'800'000'000;
static const std::string kSig(64, 'S');
static const char* kInstallation = "AAAAAAAAAAAAAAAAAAAAAA";   // 22 ký tự base64url = 16 byte 0

static bool FakeVerify(const Jwk& k, const std::string&, const std::string& sig) { return k.kid == "k1" && sig == kSig; }

static std::string Jws(const std::string& header, const std::string& payload, const std::string& sig = kSig)
{
    return Base64UrlEncode(header) + "." + Base64UrlEncode(payload) + "." + Base64UrlEncode(sig);
}

static std::string QueryParam(const std::string& url, const std::string& name)
{
    size_t q = url.find('?');
    if (q == std::string::npos) return std::string();
    std::string query = url.substr(q + 1);
    size_t pos = 0;
    while (pos <= query.size())
    {
        size_t amp = query.find('&', pos);
        if (amp == std::string::npos) amp = query.size();
        std::string pair = query.substr(pos, amp - pos);
        size_t eq = pair.find('=');
        if (eq != std::string::npos && pair.substr(0, eq) == name)
        {
            std::string v;
            UrlDecode(pair.substr(eq + 1), v);
            return v;
        }
        pos = amp + 1;
    }
    return std::string();
}

// Gửi một yêu cầu HTTP tới cổng loopback như trình duyệt khi tsudev.com chuyển hướng về
static void BrowserHit(unsigned port, const std::string& target)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.sin_port = htons(static_cast<u_short>(port));
    if (connect(s, reinterpret_cast<sockaddr*>(&a), sizeof(a)) == 0)
    {
        std::string req = "GET " + target + " HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
        send(s, req.data(), static_cast<int>(req.size()), 0);
        char buf[2048];
        while (recv(s, buf, sizeof(buf), 0) > 0) {}
    }
    closesocket(s);
    WSACleanup();
}

enum class BrowserMode { Ok, WrongState, Denied, NoIss, Ignore };

struct Fake
{
    std::mutex mu;
    std::vector<std::string> calls;          // "POST https://tsudev.com/oidc/token" ...
    std::vector<std::wstring> headers;
    std::vector<std::string> bodies;
    std::string nonce;
    BrowserMode browser = BrowserMode::Ok;
    int browser_opens = 0;
    bool network_down = false;
    unsigned refresh_status = 200;
    std::string refresh_body;                // rỗng => phản hồi hợp lệ có rt-2
    std::string ent_sub = "u1";
    int64_t pro_until = T0 + 3 * 86400;
    std::string secret;
    bool has_secret = false;
    std::vector<std::thread> threads;

    ~Fake()
    {
        for (auto& t : threads) if (t.joinable()) t.join();
    }

    size_t CallCount()
    {
        std::lock_guard<std::mutex> lk(mu);
        return calls.size();
    }

    bool Called(const std::string& c)
    {
        std::lock_guard<std::mutex> lk(mu);
        for (auto& x : calls) if (x == c) return true;
        return false;
    }

    ProHttpResult Http(const wchar_t* method, const std::string& url, const std::wstring& h, const std::string& body)
    {
        std::lock_guard<std::mutex> lk(mu);
        calls.push_back(std::string(method[0] == L'P' ? "POST " : "GET ") + url);
        headers.push_back(h);
        bodies.push_back(body);
        ProHttpResult r;
        if (network_down) return r;
        r.ok = true;
        r.status = 200;
        const std::string jwks = "{\"keys\":[{\"kty\":\"EC\",\"crv\":\"P-256\",\"kid\":\"k1\",\"alg\":\"ES256\",\"x\":\""
            + Base64UrlEncode(std::string(32, 'x')) + "\",\"y\":\"" + Base64UrlEncode(std::string(32, 'y')) + "\"}]}";
        if (url == kTokenEndpoint && body.find("grant_type=authorization_code") == 0)
        {
            const std::string id = Jws(R"({"alg":"ES256","kid":"k1"})", "{\"iss\":\"https://tsudev.com/oidc\",\"aud\":\"tsutramon\",\"sub\":\"u1\",\"iat\":"
                + std::to_string(T0) + ",\"exp\":" + std::to_string(T0 + 3600) + ",\"nonce\":\"" + nonce + "\"}");
            r.body = "{\"access_token\":\"at-1\",\"token_type\":\"Bearer\",\"expires_in\":3600,\"id_token\":\"" + id + "\",\"refresh_token\":\"rt-1\"}";
        }
        else if (url == kTokenEndpoint && body.find("grant_type=refresh_token") == 0)
        {
            r.status = refresh_status;
            r.body = refresh_body.empty() ? "{\"access_token\":\"at-2\",\"token_type\":\"Bearer\",\"refresh_token\":\"rt-2\"}" : refresh_body;
        }
        else if (url == kJwksUrl)
            r.body = jwks;
        else if (url == kEntitlementsUrl)
        {
            const std::string p = "{\"iss\":\"https://tsudev.com/oidc\",\"aud\":\"tsutramon\",\"sub\":\"" + ent_sub + "\",\"iat\":"
                + std::to_string(T0) + ",\"exp\":" + std::to_string(T0 + 7 * 86400) + ",\"pro_until\":" + std::to_string(pro_until) + "}";
            r.body = "{\"jws\":\"" + Jws(R"({"alg":"ES256","typ":"tsu-ent+jwt","kid":"k1"})", p) + "\"}";
        }
        else if (url == kRevocationEndpoint)
            r.body.clear();
        else
            r.status = 404;
        return r;
    }

    bool OpenBrowser(const std::string& url)
    {
        std::lock_guard<std::mutex> lk(mu);
        browser_opens++;
        nonce = QueryParam(url, "nonce");
        const std::string redirect = QueryParam(url, "redirect_uri");
        const std::string state = QueryParam(url, "state");
        const unsigned port = static_cast<unsigned>(std::stoul(redirect.substr(std::strlen("http://127.0.0.1:"))));
        const std::string iss = "&iss=" + UrlEncode(kIssuer);
        std::string target;
        switch (browser)
        {
        case BrowserMode::Ok: target = "/callback?code=code-1&state=" + UrlEncode(state) + iss; break;
        case BrowserMode::WrongState: target = "/callback?code=code-1&state=khac" + iss; break;
        case BrowserMode::Denied: target = "/callback?error=access_denied&state=" + UrlEncode(state) + iss; break;
        case BrowserMode::NoIss: target = "/callback?code=code-1&state=" + UrlEncode(state); break;
        case BrowserMode::Ignore: return true;
        }
        threads.emplace_back([port, target] {
            BrowserHit(port, "/favicon.ico");      // yêu cầu lạ: 404, vẫn chờ tiếp
            BrowserHit(port, target);
        });
        return true;
    }

    ProAccountDeps Deps()
    {
        ProAccountDeps d;
        d.http = [this](const wchar_t* m, const std::string& u, const std::wstring& h, const std::string& b) { return Http(m, u, h, b); };
        d.open_browser = [this](const std::string& u) { return OpenBrowser(u); };
        d.verify = FakeVerify;
        d.save_secret = [this](const std::string& s) { std::lock_guard<std::mutex> lk(mu); secret = s; has_secret = true; return true; };
        d.load_secret = [this](std::string& s) { std::lock_guard<std::mutex> lk(mu); s = secret; return has_secret; };
        d.delete_secret = [this] { std::lock_guard<std::mutex> lk(mu); secret.clear(); has_secret = false; return true; };
        d.now = [] { return T0 + 10; };
        d.login_timeout_ms = 3000;
        d.first_refresh_delay_ms = 60 * 60 * 1000;
        d.refresh_interval_ms = 60 * 60 * 1000;
        d.retry_interval_ms = 60 * 60 * 1000;
        return d;
    }
};

template <class P>
static bool WaitFor(const CProAccount& a, P pred, int ms = 5000)
{
    for (int i = 0; i < ms / 10; i++)
    {
        CProAccount::Status s = a.GetStatus();
        if (pred(s)) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return false;
}

static bool Idle(const CProAccount::Status& s) { return s.op == CProAccount::Op::None; }

static std::wstring NewTempDir()
{
    static int n = 0;
    wchar_t tmp[MAX_PATH];
    GetTempPathW(MAX_PATH, tmp);
    std::wstring dir = std::wstring(tmp) + L"tsutramon-proaccount-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(++n) + L"\\";
    CreateDirectoryW(dir.c_str(), nullptr);
    return dir;
}

static bool FileExists(const std::wstring& p) { return GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES; }

static void RemoveDir(const std::wstring& dir)
{
    DeleteFileW((dir + CProAccount::kEntitlementFile).c_str());
    DeleteFileW((dir + CProAccount::kJwksFile).c_str());
    RemoveDirectoryW(dir.c_str());
}

// Chạy trọn lượt đăng nhập thành công, trả về sau khi luồng nền rảnh
static bool SignIn(Fake& f, CProAccount& a)
{
    (void)f;
    const uint64_t rev = a.GetStatus().revision;
    a.StartSignIn();
    return WaitFor(a, [rev](const CProAccount::Status& s) { return s.revision > rev && Idle(s) && (s.has_entitlement || s.last_error != CProAccount::Error::None); });
}

static void TestGuestNoNetwork()
{
    Fake f;
    ProAccountDeps d = f.Deps();
    d.first_refresh_delay_ms = 20;
    d.refresh_interval_ms = 30;
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(d);
        a.Init(dir, kInstallation, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        a.RefreshNow();     // chưa đăng nhập: không có refresh token => không gọi gì
        CHECK(WaitFor(a, [](const CProAccount::Status& s) { return Idle(s) && !s.signed_in; }));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        CHECK(f.CallCount() == 0);
        CHECK(!a.GetStatus().signed_in && !a.GetStatus().has_entitlement);
    }
    RemoveDir(dir);
}

static void TestOfflineBlocksSignIn()
{
    Fake f;
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(f.Deps());
        a.Init(dir, kInstallation, true);
        a.StartSignIn();
        CHECK(WaitFor(a, [](const CProAccount::Status& s) { return s.last_error == CProAccount::Error::Offline; }));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        CHECK(f.browser_opens == 0 && f.CallCount() == 0);
    }
    RemoveDir(dir);
}

static void TestSignInFull()
{
    Fake f;
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(f.Deps());
        a.Init(dir, kInstallation, false);
        CHECK(SignIn(f, a));
        CProAccount::Status s = a.GetStatus();
        CHECK(s.signed_in && s.has_entitlement && s.last_error == CProAccount::Error::None);
        CHECK(s.entitlement.sub == "u1" && s.entitlement.pro_until == T0 + 3 * 86400 && s.last_online_ok == T0 + 10);
        CHECK(f.browser_opens == 1 && f.has_secret && f.secret == "rt-1");
        std::lock_guard<std::mutex> lk(f.mu);
        CHECK(f.calls.size() == 3);
        if (f.calls.size() == 3)
        {
            CHECK(f.calls[0] == std::string("POST ") + kTokenEndpoint);
            CHECK(f.bodies[0].find("code=code-1") != std::string::npos && f.bodies[0].find("code_verifier=") != std::string::npos);
            CHECK(f.headers[0].find(L"X-Tsu-Installation") == std::wstring::npos);
            CHECK(f.calls[1] == std::string("GET ") + kJwksUrl);
            CHECK(f.calls[2] == std::string("GET ") + kEntitlementsUrl);
            CHECK(f.headers[2].find(L"Authorization: Bearer at-1\r\n") != std::wstring::npos);
            CHECK(f.headers[2].find(L"X-Tsu-Installation: AAAAAAAAAAAAAAAAAAAAAA\r\n") != std::wstring::npos);
        }
        CHECK(FileExists(dir + CProAccount::kEntitlementFile) && FileExists(dir + CProAccount::kJwksFile));
        int64_t exp = 0, until = 0;
        CHECK(a.RenewalInput(exp, until) && exp == T0 + 7 * 86400 && until == T0 + 3 * 86400);
    }
    // Khởi động lại: nạp JWS đệm, kiểm lại chữ ký, KHÔNG gọi mạng (lần làm mới đầu còn xa)
    {
        const size_t before = f.CallCount();
        CProAccount b(f.Deps());
        b.Init(dir, kInstallation, false);
        CProAccount::Status s = b.GetStatus();
        CHECK(s.signed_in && s.has_entitlement && s.entitlement.pro_until == T0 + 3 * 86400);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        CHECK(f.CallCount() == before);
    }
    // Tệp đệm bị sửa (chữ ký sai) => không có quyền lợi
    {
        std::ofstream(dir + CProAccount::kEntitlementFile, std::ios::binary | std::ios::trunc)
            << Jws(R"({"alg":"ES256","typ":"tsu-ent+jwt","kid":"k1"})", "{\"iss\":\"https://tsudev.com/oidc\",\"aud\":\"tsutramon\",\"sub\":\"u1\",\"iat\":1800000000,\"exp\":1800600000,\"pro_until\":1900000000}", std::string(64, 'X'));
        CProAccount c(f.Deps());
        c.Init(dir, kInstallation, false);
        CHECK(c.GetStatus().signed_in && !c.GetStatus().has_entitlement);
        int64_t exp = 0, until = 0;
        CHECK(!c.RenewalInput(exp, until));
    }
    RemoveDir(dir);
}

static void TestBadCallbacks()
{
    const struct { BrowserMode mode; CProAccount::Error want; } cases[] = {
        { BrowserMode::WrongState, CProAccount::Error::BadCallback },
        { BrowserMode::NoIss, CProAccount::Error::BadCallback },
        { BrowserMode::Denied, CProAccount::Error::Denied },
    };
    for (const auto& c : cases)
    {
        Fake f;
        f.browser = c.mode;
        const std::wstring dir = NewTempDir();
        {
            CProAccount a(f.Deps());
            a.Init(dir, kInstallation, false);
            CHECK(SignIn(f, a));
            CProAccount::Status s = a.GetStatus();
            CHECK(s.last_error == c.want && !s.signed_in && !s.has_entitlement);
            CHECK(f.CallCount() == 0 && !f.has_secret);     // không đổi mã lấy token
        }
        RemoveDir(dir);
    }
}

static void TestCancelSignIn()
{
    Fake f;
    f.browser = BrowserMode::Ignore;     // trình duyệt không bao giờ quay về
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(f.Deps());
        a.Init(dir, kInstallation, false);
        a.StartSignIn();
        CHECK(WaitFor(a, [](const CProAccount::Status& s) { return s.op == CProAccount::Op::SigningIn; }));
        a.CancelSignIn();
        CHECK(WaitFor(a, [](const CProAccount::Status& s) { return Idle(s) && s.last_error == CProAccount::Error::Cancelled; }, 1500));
        CHECK(f.CallCount() == 0);
    }
    RemoveDir(dir);
}

static void TestWrongEntitlementSubject()
{
    Fake f;
    f.ent_sub = "u2";     // JWS của tài khoản khác
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(f.Deps());
        a.Init(dir, kInstallation, false);
        CHECK(SignIn(f, a));
        CProAccount::Status s = a.GetStatus();
        CHECK(s.last_error == CProAccount::Error::InvalidToken && !s.has_entitlement);
        CHECK(!FileExists(dir + CProAccount::kEntitlementFile));
    }
    RemoveDir(dir);
}

static void TestNetworkDown()
{
    Fake f;
    f.network_down = true;
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(f.Deps());
        a.Init(dir, kInstallation, false);
        CHECK(SignIn(f, a));
        CHECK(a.GetStatus().last_error == CProAccount::Error::Network && !a.GetStatus().signed_in && !f.has_secret);
    }
    RemoveDir(dir);
}

static void TestRefreshRotationAndExpiry()
{
    Fake f;
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(f.Deps());
        a.Init(dir, kInstallation, false);
        CHECK(SignIn(f, a));
        f.pro_until = T0 + 400 * 86400;      // đã gia hạn trên web
        const size_t before = f.CallCount();
        const uint64_t rev = a.GetStatus().revision;
        a.RefreshNow();
        CHECK(WaitFor(a, [rev](const CProAccount::Status& s) { return s.revision > rev && Idle(s); }));
        CHECK(f.secret == "rt-2");
        CHECK(a.GetStatus().entitlement.pro_until == T0 + 400 * 86400);
        {
            std::lock_guard<std::mutex> lk(f.mu);
            CHECK(f.calls.size() == before + 3);
            if (f.calls.size() == before + 3)
            {
                CHECK(f.bodies[before].find("grant_type=refresh_token&refresh_token=rt-1&client_id=tsutramon") == 0);
                CHECK(f.headers[before + 2].find(L"Authorization: Bearer at-2\r\n") != std::wstring::npos);
            }
        }
        // Refresh token bị thu hồi phía máy chủ => về chế độ khách, xoá sạch cục bộ
        f.refresh_status = 400;
        f.refresh_body = R"({"error":"invalid_grant","error_description":"grant request is invalid"})";
        const uint64_t rev2 = a.GetStatus().revision;
        a.RefreshNow();
        CHECK(WaitFor(a, [rev2](const CProAccount::Status& s) { return s.revision > rev2 && Idle(s); }));
        CProAccount::Status s = a.GetStatus();
        CHECK(s.last_error == CProAccount::Error::SessionExpired && !s.signed_in && !s.has_entitlement);
        CHECK(!f.has_secret && !FileExists(dir + CProAccount::kEntitlementFile) && !FileExists(dir + CProAccount::kJwksFile));
    }
    RemoveDir(dir);
}

static void TestSignOut(bool offline_after)
{
    Fake f;
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(f.Deps());
        a.Init(dir, kInstallation, false);
        CHECK(SignIn(f, a));
        if (offline_after)
            a.SetOffline(true);
        const size_t before = f.CallCount();
        a.SignOutAndClear();
        // Cục bộ sạch NGAY khi bấm
        CProAccount::Status s = a.GetStatus();
        CHECK(!s.signed_in && !s.has_entitlement && s.entitlement.pro_until == 0);
        CHECK(!f.has_secret && !FileExists(dir + CProAccount::kEntitlementFile) && !FileExists(dir + CProAccount::kJwksFile));
        CHECK(WaitFor(a, [](const CProAccount::Status& st) { return Idle(st); }));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (offline_after)
            CHECK(f.CallCount() == before);         // ngoại tuyến: không thu hồi qua mạng
        else
        {
            CHECK(f.Called(std::string("POST ") + kRevocationEndpoint));
            std::lock_guard<std::mutex> lk(f.mu);
            CHECK(!f.bodies.empty() && f.bodies.back() == "token=rt-1&token_type_hint=refresh_token&client_id=tsutramon");
        }
        // Sau khi đăng xuất, làm mới không gọi mạng
        const size_t after = f.CallCount();
        a.RefreshNow();
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        CHECK(f.CallCount() == after);
    }
    RemoveDir(dir);
}

static void TestAutoRefresh(bool offline)
{
    Fake f;
    f.has_secret = true;
    f.secret = "rt-1";
    ProAccountDeps d = f.Deps();
    d.first_refresh_delay_ms = 30;
    const std::wstring dir = NewTempDir();
    {
        CProAccount a(d);
        a.Init(dir, kInstallation, offline);
        CHECK(a.GetStatus().signed_in);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        if (offline)
            CHECK(f.CallCount() == 0);              // AUTH 14.1: ngoại tuyến hoàn toàn chặn cả khi đã đăng nhập
        else
        {
            CHECK(WaitFor(a, [](const CProAccount::Status& s) { return s.has_entitlement && Idle(s); }));
            CHECK(f.Called(std::string("POST ") + kTokenEndpoint) && f.secret == "rt-2");
        }
    }
    RemoveDir(dir);
}

int main()
{
    TestGuestNoNetwork();
    TestOfflineBlocksSignIn();
    TestSignInFull();
    TestBadCallbacks();
    TestCancelSignIn();
    TestWrongEntitlementSubject();
    TestNetworkDown();
    TestRefreshRotationAndExpiry();
    TestSignOut(false);
    TestSignOut(true);
    TestAutoRefresh(false);
    TestAutoRefresh(true);
    std::cout << "ProAccountTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed == 0 ? 0 : 1;
}
