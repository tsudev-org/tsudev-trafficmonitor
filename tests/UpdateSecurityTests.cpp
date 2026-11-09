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
#include <cwctype>

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

    // Danh sách host khi tự tải gói cập nhật (hẹp hơn FORK_TRUSTED_HOSTS)
    const std::wstring dl = FORK_UPDATE_DOWNLOAD_HOSTS;
    CHECK(IsTrustedHostUrl(L"https://github.com/tsudev-org/tsudev-trafficmonitor/releases/download/V1/x.zip", dl));
    CHECK(IsTrustedHostUrl(L"https://release-assets.githubusercontent.com/github-production-release-asset/1?sp=r&sig=a%2Bb", dl));
    CHECK(IsTrustedHostUrl(L"https://objects.githubusercontent.com/x", dl));
    CHECK(!IsTrustedHostUrl(L"https://raw.githubusercontent.com/tsudev-org/tsudev-trafficmonitor/main/x.zip", dl));
    CHECK(!IsTrustedHostUrl(L"https://trafficmonitor.tsudev.com/x.zip", dl));
    CHECK(!IsTrustedHostUrl(L"https://api.ipify.org/", dl));
    CHECK(!IsTrustedHostUrl(L"http://release-assets.githubusercontent.com/x", dl));

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

static void TestDownloadHelpers()
{
    // IsSha256Hex / Sha256Equals
    const std::wstring h = L"78f292daf7d2c99f99a1c06ab5a03caa61a11192b38b3a4d440e5f7675324198";
    std::wstring upper = h;
    for (auto& ch : upper)
        ch = static_cast<wchar_t>(towupper(ch));
    CHECK(IsSha256Hex(h));
    CHECK(IsSha256Hex(upper));
    CHECK(!IsSha256Hex(L""));
    CHECK(!IsSha256Hex(h.substr(1)));                      // 63 ký tự
    CHECK(!IsSha256Hex(h + L"0"));                         // 65 ký tự
    CHECK(!IsSha256Hex(L"g" + h.substr(1)));
    CHECK(!IsSha256Hex(L" " + h.substr(1)));
    CHECK(Sha256Equals(h, h));
    CHECK(Sha256Equals(h, upper));                         // không phân biệt hoa thường
    std::wstring other = h;
    other[63] = (other[63] == L'8') ? L'9' : L'8';
    CHECK(!Sha256Equals(h, other));
    CHECK(!Sha256Equals(L"", L""));                        // rỗng không bao giờ khớp
    CHECK(!Sha256Equals(h.substr(1), h.substr(1)));        // sai độ dài không bao giờ khớp
    CHECK(!Sha256Equals(L"z" + h.substr(1), L"z" + h.substr(1)));

    // ToHexLower, Sha256Hex, Sha256Stream (vector chuẩn FIPS 180-2)
    const unsigned char bytes[] = { 0x00, 0x0f, 0xa5, 0xff };
    CHECK(ToHexLower(bytes, sizeof(bytes)) == L"000fa5ff");
    CHECK(ToHexLower(bytes, 0).empty());
    CHECK(Sha256Hex("", 0) == L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    CHECK(Sha256Hex("abc", 3) == L"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    const char* msg = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    const std::wstring msg_hash = L"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1";
    CHECK(Sha256Hex(msg, strlen(msg)) == msg_hash);
    {
        Sha256Stream stream;                               // băm theo khối cho cùng kết quả
        CHECK(stream.IsValid());
        CHECK(stream.Update(msg, 10));
        CHECK(stream.Update(msg + 10, 0));
        CHECK(stream.Update(msg + 10, strlen(msg) - 10));
        CHECK(stream.FinishHex() == msg_hash);
        CHECK(!stream.IsValid());                          // chỉ kết thúc được một lần
        CHECK(stream.FinishHex().empty());
        CHECK(!stream.Update("x", 1));
    }
    {
        std::string big(1024 * 1024, 'a');                 // 1 MiB 'a', đối chiếu với băm từng khối 4 KiB
        Sha256Stream stream;
        for (size_t i = 0; i < big.size(); i += 4096)
            stream.Update(big.data() + i, 4096);
        CHECK(stream.FinishHex() == Sha256Hex(big.data(), big.size()));
    }
    CHECK(MAX_UPDATE_PACKAGE_BYTES == 50ull * 1024 * 1024);

    // SafeFileNameFromUrl
    const std::wstring rel = L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/";
    const std::wstring org = L"https://github.com/tsudev-org/tsudev-trafficmonitor/releases/download/";
    CHECK(SafeFileNameFromUrl(rel + L"V26.11.0902-vn/tsudev-trafficmonitor_26.11.0902_x64_Lite.zip") == L"tsudev-trafficmonitor_26.11.0902_x64_Lite.zip");
    CHECK(SafeFileNameFromUrl(org + L"V1-vn/a_b-c.1.ZIP") == L"a_b-c.1.ZIP");
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/setup.exe").empty());           // chỉ lưu .zip
    CHECK(SafeFileNameFromUrl(L"https://github.com/evil/repo/releases/download/V1/x.zip").empty());
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/.zip").empty());
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/.hidden.zip").empty());
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/a$b.zip").empty());
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/a:b.zip").empty());              // luồng dữ liệu NTFS (ADS)
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/a\x0103.zip").empty());          // ký tự ngoài ASCII
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/CON.zip").empty());              // tên thiết bị DOS
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/nul.tar.zip").empty());
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/com1.zip").empty());
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/console.zip") == L"console.zip");
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/" + std::wstring(124, L'a') + L".zip").size() == 128);
    CHECK(SafeFileNameFromUrl(rel + L"V1-vn/" + std::wstring(125, L'a') + L".zip").empty());

    // UniqueFileName: không ghi đè tệp đang có
    std::vector<std::wstring> existing;
    auto exists = [&](const std::wstring& name) {
        for (const auto& e : existing)
            if (_wcsicmp(e.c_str(), name.c_str()) == 0)
                return true;
        return false;
    };
    CHECK(UniqueFileName(L"a.zip", exists) == L"a.zip");
    existing.push_back(L"a.zip");
    CHECK(UniqueFileName(L"a.zip", exists) == L"a (2).zip");
    existing.push_back(L"A (2).ZIP");                      // Windows không phân biệt hoa thường
    CHECK(UniqueFileName(L"a.zip", exists) == L"a (3).zip");
    CHECK(UniqueFileName(L"", exists).empty());
    existing.push_back(L"noext");
    CHECK(UniqueFileName(L"noext", exists) == L"noext (2)");
    CHECK(UniqueFileName(L"x.zip", [](const std::wstring&) { return true; }).empty());   // hết tên
}

int main(int argc, char* argv[])
{
    std::string fixtures = (argc > 1) ? argv[1] : "tests\\fixtures";
    TestVersion();
    TestUrls();
    TestSignature(fixtures);
    TestDownloadHelpers();
    std::cout << "UpdateSecurityTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed == 0 ? 0 : 1;
}
