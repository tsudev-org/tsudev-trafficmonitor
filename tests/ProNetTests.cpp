// Kiểm thử tầng nền tảng của đăng nhập tsudev (tsutramon/ProNet.h, P9 phần 2). CHỈ Windows.
// KHÔNG gọi mạng ra ngoài: chỉ 127.0.0.1 và Credential Manager của tài khoản chạy test (mục thử tự xoá).
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /sdl /I tsutramon tests\ProNetTests.cpp /Fe:ProNetTests.exe

#include "ProNet.h"
#include <iostream>
#include <thread>

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

static void TestUrlGuard()
{
    using ProNet::PathIfTsudevUrl;
    CHECK(PathIfTsudevUrl("https://tsudev.com/oidc/token") == "/oidc/token");
    CHECK(PathIfTsudevUrl("https://tsudev.com/api/me/entitlements") == "/api/me/entitlements");
    CHECK(PathIfTsudevUrl("https://tsudev.com/pro?app=tsutramon") == "/pro?app=tsutramon");
    CHECK(PathIfTsudevUrl("http://tsudev.com/oidc/token").empty());
    CHECK(PathIfTsudevUrl("https://tsudev.com.evil.example/oidc/token").empty());
    CHECK(PathIfTsudevUrl("https://tsudev.com@evil.example/").empty());
    CHECK(PathIfTsudevUrl("https://tsudev.com:8443/oidc/token").empty());
    CHECK(PathIfTsudevUrl("https://evil.example/https://tsudev.com/").empty());
    CHECK(PathIfTsudevUrl("https://tsudev.com/a b").empty());
    CHECK(PathIfTsudevUrl("https://tsudev.com/a\\b").empty());
    CHECK(PathIfTsudevUrl("https://tsudev.com/a#x").empty());
    CHECK(PathIfTsudevUrl("https://TSUDEV.com/").empty());
    // Đích không hợp lệ => từ chối trước khi mở kết nối (không gọi mạng)
    auto r = ProNet::HttpsRequest(L"GET", "https://evil.example/oidc/jwks", L"", "");
    CHECK(!r.ok && r.status == 0 && !r.error.empty());
}

// Gửi một yêu cầu HTTP tới cổng loopback, trả dòng trạng thái phản hồi
static std::string SendLocal(unsigned port, const std::string& request)
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return std::string();
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = htons(static_cast<u_short>(port));
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);
    std::string status;
    if (connect(s, reinterpret_cast<sockaddr*>(&a), sizeof(a)) == 0)
    {
        send(s, request.data(), static_cast<int>(request.size()), 0);
        char buf[512];
        int n = recv(s, buf, sizeof(buf) - 1, 0);
        if (n > 0)
        {
            std::string resp(buf, static_cast<size_t>(n));
            status = resp.substr(0, resp.find("\r\n"));
        }
    }
    closesocket(s);
    return status;
}

static void TestLoopback()
{
    ProNet::LoopbackReceiver rx;
    CHECK(rx.IsListening());
    CHECK(rx.Port() > 0);
    if (!rx.IsListening())
        return;
    const unsigned port = rx.Port();
    CHECK(ProAuth::RedirectUri(port) == "http://127.0.0.1:" + std::to_string(port) + "/callback");

    std::string s1, s2;
    std::thread client([&] {
        s1 = SendLocal(port, "GET /favicon.ico HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");
        s2 = SendLocal(port, "GET /callback?code=c1&state=st&iss=https%3A%2F%2Ftsudev.com%2Foidc HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");
    });
    ProAuth::CallbackResult r = rx.Wait(10000);
    client.join();
    CHECK(s1 == "HTTP/1.1 404 Not Found");
    CHECK(s2 == "HTTP/1.1 200 OK");
    CHECK(r.ok && r.code == "c1" && r.state == "st" && r.IssuerOk());

    // Hết thời gian mà không ai gọi => kết quả rỗng
    ProNet::LoopbackReceiver rx2;
    auto t0 = std::chrono::steady_clock::now();
    CHECK(!rx2.Wait(600).ok);
    CHECK(std::chrono::steady_clock::now() - t0 >= std::chrono::milliseconds(500));
    // Huỷ
    volatile bool cancel = true;
    CHECK(!rx2.Wait(10000, &cancel).ok);
}

static void TestCredential()
{
    std::wstring target = L"tsudev/tsutramon/test-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64());
    std::string got;
    CHECK(!ProNet::LoadSecret(got, target.c_str()));
    CHECK(ProNet::DeleteSecret(target.c_str()));            // chưa có => coi như đã xoá
    CHECK(ProNet::SaveSecret("rt-\x01\xFFgiá trị", target.c_str()));
    CHECK(ProNet::LoadSecret(got, target.c_str()) && got == "rt-\x01\xFFgiá trị");
    CHECK(ProNet::SaveSecret("rt-2", target.c_str()));       // ghi đè
    CHECK(ProNet::LoadSecret(got, target.c_str()) && got == "rt-2");
    CHECK(ProNet::DeleteSecret(target.c_str()));
    CHECK(!ProNet::LoadSecret(got, target.c_str()) && got.empty());
    CHECK(!ProNet::SaveSecret("", target.c_str()));
    CHECK(!ProNet::SaveSecret(std::string(CRED_MAX_CREDENTIAL_BLOB_SIZE + 1, 'a'), target.c_str()));
}

int main()
{
    TestUrlGuard();
    TestLoopback();
    TestCredential();
    std::cout << "ProNetTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed ? 1 : 0;
}
