// Kiểm thử tự động cho logic thuần của kênh cập nhật (TrafficMonitor/UpdateSecurity.h).
// Build và chạy trong CI (build.yml, job "Kiểm thử C++"); chạy tay trên Windows:
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /I TrafficMonitor tests\UpdateSecurityTests.cpp /Fe:UpdateSecurityTests.exe
//   UpdateSecurityTests.exe tests\fixtures
// Dữ liệu mẫu tests/fixtures/version_signed.xml(.sig) là manifest thật của bản 26.11.0902 do release.yml ký,
// nên test VerifySignature còn kiểm luôn khóa công khai đang nhúng trong ForkConfig.h.

#include "UpdateSecurity.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace UpdateSecurity;

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

static bool ReadFile(const std::string& path, std::string& out)
{
    std::ifstream f(path, std::ios::binary);
    if (!f)
        return false;
    out.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
    return true;
}

static void TestVersion()
{
    CHECK(IsValidVersionString(L"1.86"));
    CHECK(IsValidVersionString(L"1.86.1"));
    CHECK(IsValidVersionString(L"26.11.0902"));
    CHECK(IsValidVersionString(L"1.2.3.4"));
    CHECK(!IsValidVersionString(L""));
    CHECK(!IsValidVersionString(L"1.2.3.4.5"));     // quá 4 nhóm
    CHECK(!IsValidVersionString(L"1..2"));
    CHECK(!IsValidVersionString(L".1"));
    CHECK(!IsValidVersionString(L"1."));
    CHECK(!IsValidVersionString(L"1.123456"));      // nhóm quá 5 chữ số
    CHECK(!IsValidVersionString(L"1.86-vn"));
    CHECK(!IsValidVersionString(L" 1.86"));
    CHECK(!IsValidVersionString(L"1.86<script>"));

    CHECK(CompareVersion(L"26.11.0902", L"26.11.0901") == 1);
    CHECK(CompareVersion(L"26.11.0901", L"1.91") == 1);   // chuyển khuôn phiên bản vẫn được coi là mới hơn
    CHECK(CompareVersion(L"1.91", L"26.11.0901") == -1);
    CHECK(CompareVersion(L"1.100", L"1.86") == 1);       // so theo số, không theo chuỗi
    CHECK(CompareVersion(L"1.9", L"1.86") == -1);
    CHECK(CompareVersion(L"1.86", L"1.86.0") == 0);      // nhóm thiếu coi là 0
    CHECK(CompareVersion(L"1.86.1", L"1.86") == 1);
    CHECK(CompareVersion(L"26.11.0902", L"26.11.902") == 0);
    CHECK(CompareVersion(L"", L"") == 0);
}

static void TestUrls()
{
    // IsHttpsUrl
    CHECK(IsHttpsUrl(L"https://trafficmonitor.tsudev.com/"));
    CHECK(IsHttpsUrl(L"HTTPS://github.com/x"));
    CHECK(!IsHttpsUrl(L"http://github.com/x"));
    CHECK(!IsHttpsUrl(L"file:///C:/Windows/win.ini"));
    CHECK(!IsHttpsUrl(L"https://"));
    CHECK(!IsHttpsUrl(L"https://github.com/a b"));
    CHECK(!IsHttpsUrl(L"https://github.com/\x7f"));
    CHECK(!IsHttpsUrl(L"https://github.com/\t"));
    CHECK(!IsHttpsUrl(L"https://github.com/" + std::wstring(2100, L'a')));

    // IsTrustedHostUrl (danh sách host trong ForkConfig.h)
    CHECK(IsTrustedHostUrl(L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor"));
    CHECK(IsTrustedHostUrl(L"https://GitHub.com/x"));
    CHECK(IsTrustedHostUrl(L"https://trafficmonitor.tsudev.com"));
    CHECK(IsTrustedHostUrl(L"https://api.ipify.org?format=json"));
    CHECK(!IsTrustedHostUrl(L"https://github.com.evil.example/x"));
    CHECK(!IsTrustedHostUrl(L"https://evil.example/github.com"));
    CHECK(!IsTrustedHostUrl(L"https://github.com@evil.example/x"));
    CHECK(!IsTrustedHostUrl(L"https://user:pass@github.com/x"));
    CHECK(!IsTrustedHostUrl(L"https://github.com:8443/x"));
    CHECK(!IsTrustedHostUrl(L"https://github.com\\@evil.example/"));
    CHECK(!IsTrustedHostUrl(L"https://sub.github.com/x"));
    CHECK(!IsTrustedHostUrl(L"http://github.com/x"));
    CHECK(!IsTrustedHostUrl(L"https:///x"));
    CHECK(IsTrustedHostUrl(L"https://a.example/x", L"b.example| a.example |"));   // tách và cắt khoảng trắng

    // IsTrustedDownloadUrl (tiền tố trang Releases của repo)
    const std::wstring rel = L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/";
    CHECK(IsTrustedDownloadUrl(rel + L"V26.11.0902-vn/tsudev-trafficmonitor_26.11.0902_x64_Lite.zip"));
    CHECK(IsTrustedDownloadUrl(rel + L"V1.0-vn/setup.EXE"));
    CHECK(!IsTrustedDownloadUrl(rel));
    CHECK(!IsTrustedDownloadUrl(rel + L"V1/file.dll"));
    CHECK(!IsTrustedDownloadUrl(rel + L"V1/file.zip.txt"));
    CHECK(!IsTrustedDownloadUrl(rel + L"../../../other/repo/releases/download/x.zip"));
    CHECK(!IsTrustedDownloadUrl(rel + L"V1/x.zip?redirect=https://evil.example/x.zip"));
    CHECK(!IsTrustedDownloadUrl(rel + L"V1/x.zip#.zip"));
    CHECK(!IsTrustedDownloadUrl(rel + L"V1/%2e%2e/x.zip"));
    CHECK(!IsTrustedDownloadUrl(rel + L"V1\\x.zip"));
    CHECK(!IsTrustedDownloadUrl(rel + L"V1/a b.zip"));
    CHECK(!IsTrustedDownloadUrl(L"https://github.com/evil/repo/releases/download/V1/x.zip"));
    CHECK(!IsTrustedDownloadUrl(L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor-evil/releases/download/V1/x.zip"));
    CHECK(!IsTrustedDownloadUrl(L"http://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/V1/x.zip"));

    // Sau khi chuyển repo sang Organization tsudev-org: chấp nhận cả tiền tố cũ và mới, không chấp nhận owner khác
    const std::wstring org = L"https://github.com/tsudev-org/tsudev-trafficmonitor/releases/download/";
    CHECK(IsTrustedDownloadUrl(org + L"V26.11.1001-vn/tsudev-trafficmonitor_26.11.1001_x64_Lite.zip"));
    CHECK(!IsTrustedDownloadUrl(org));
    CHECK(!IsTrustedDownloadUrl(org + L"V1/../x.zip"));
    CHECK(!IsTrustedDownloadUrl(L"https://github.com/tsudev-org/other-repo/releases/download/V1/x.zip"));
    CHECK(!IsTrustedDownloadUrl(L"https://github.com/tsudev-org-evil/tsudev-trafficmonitor/releases/download/V1/x.zip"));
    CHECK(IsTrustedDownloadUrl(rel + L"V1/x.zip", rel));                  // danh sách một tiền tố
    CHECK(!IsTrustedDownloadUrl(org + L"V1/x.zip", rel));
}

static void TestSignature(const std::string& fixtures)
{
    std::string xml, sig;
    bool loaded = ReadFile(fixtures + "\\version_signed.xml", xml) && ReadFile(fixtures + "\\version_signed.xml.sig", sig);
    CHECK(loaded);
    if (!loaded)
        return;

    CHECK(VerifySignature(xml, sig));                         // manifest thật + chữ ký thật
    CHECK(VerifySignature(xml, sig + "\r\n  "));              // khoảng trắng cuối được bỏ qua

    std::string tampered = xml;
    size_t pos = tampered.find("26.11.0902");
    CHECK(pos != std::string::npos);
    if (pos != std::string::npos)
    {
        tampered[pos + 9] = '9';                              // sửa một ký tự phiên bản
        CHECK(!VerifySignature(tampered, sig));
    }
    CHECK(!VerifySignature(xml + " ", sig));                  // thêm một byte

    std::string bad_sig = sig;
    bad_sig[0] = (bad_sig[0] == 'A') ? 'B' : 'A';
    CHECK(!VerifySignature(xml, bad_sig));
    CHECK(!VerifySignature(xml, ""));
    CHECK(!VerifySignature(xml, "khong-phai-base64"));
    CHECK(!VerifySignature(xml, "AAAA"));                     // sai độ dài (3 byte)
    CHECK(!VerifySignature(xml, std::string(300, 'A')));      // quá dài

    // Khóa công khai sai định dạng hoặc khóa khác
    CHECK(!VerifySignature(xml, sig, ""));
    CHECK(!VerifySignature(xml, sig, "zz"));
    std::string other_key = FORK_UPDATE_PUBLIC_KEY_HEX;
    other_key[0] = (other_key[0] == '0') ? '1' : '0';
    CHECK(!VerifySignature(xml, sig, other_key.c_str()));
}

int main(int argc, char* argv[])
{
    std::string fixtures = (argc > 1) ? argv[1] : "tests\\fixtures";
    TestVersion();
    TestUrls();
    TestSignature(fixtures);
    std::cout << "UpdateSecurityTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed == 0 ? 0 : 1;
}
