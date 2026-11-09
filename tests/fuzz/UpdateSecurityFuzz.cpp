// Fuzzing (libFuzzer + AddressSanitizer của MSVC) cho logic thuần của kênh cập nhật (TrafficMonitor/UpdateSecurity.h).
// Ngoài việc tìm lỗi bộ nhớ (ASan), harness kiểm các BẤT BIẾN BẢO MẬT: hễ một URL được tin thì nó phải thỏa mọi
// điều kiện của bộ lọc; vi phạm => abort() để libFuzzer báo lỗi kèm đầu vào tái hiện.
// CI (build.yml, job "Kiểm thử C++"):
//   cl /std:c++17 /EHsc /utf-8 /Zi /fsanitize=address /fsanitize=fuzzer /I TrafficMonitor tests\fuzz\UpdateSecurityFuzz.cpp
//   UpdateSecurityFuzz.exe -max_total_time=120 tests\fuzz\corpus

#include "UpdateSecurity.h"
#include <cstdint>
#include <cstdlib>

using namespace UpdateSecurity;

static void Require(bool cond)
{
    if (!cond)
        abort();
}

static bool StartsWithI(const std::wstring& s, const wchar_t* prefix)
{
    size_t n = wcslen(prefix);
    return s.size() >= n && _wcsnicmp(s.c_str(), prefix, n) == 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Byte đầu chọn cách hiểu dữ liệu: chuỗi ASCII mở rộng hoặc UTF-16 thô (kể cả ký tự điều khiển, NUL)
    std::wstring w;
    if (size > 0 && (data[0] & 1))
    {
        for (size_t i = 1; i + 1 < size; i += 2)
            w.push_back(static_cast<wchar_t>(data[i] | (data[i + 1] << 8)));
    }
    else
    {
        for (size_t i = 1; i < size; i++)
            w.push_back(static_cast<wchar_t>(data[i]));
    }

    // Ghép với tiền tố hợp lệ để fuzzer đi sâu vào phần sau tiền tố
    const std::wstring rel_old = FORK_RELEASE_DOWNLOAD_PREFIX;
    const std::wstring rel_org = FORK_RELEASE_DOWNLOAD_PREFIX_ORG;
    const std::wstring candidates[] = { w, rel_old + w, rel_org + w, L"https://" + w, L"https://github.com/" + w };

    for (const auto& url : candidates)
    {
        if (IsTrustedDownloadUrl(url))
        {
            Require(IsHttpsUrl(url));
            Require(StartsWithI(url, L"https://"));
            Require(url.compare(0, rel_old.size(), rel_old) == 0 || url.compare(0, rel_org.size(), rel_org) == 0);
            Require(url.find(L"..") == std::wstring::npos);
            Require(url.find_first_of(L"@?#\\% \t\r\n") == std::wstring::npos);
            for (wchar_t ch : url)
                Require(ch > L' ' && ch != 0x7f);
            Require(url.size() > 4);
            std::wstring ext = url.substr(url.size() - 4);
            Require(_wcsicmp(ext.c_str(), L".zip") == 0 || _wcsicmp(ext.c_str(), L".exe") == 0);
        }
        if (IsTrustedHostUrl(url))
        {
            Require(IsHttpsUrl(url));
            size_t end = url.find_first_of(L"/?#", 8);
            std::wstring host = url.substr(8, end == std::wstring::npos ? std::wstring::npos : end - 8);
            Require(!host.empty() && host.find_first_of(L"@:\\") == std::wstring::npos);
            bool listed = false;
            for (const auto& h : SplitTrim(FORK_TRUSTED_HOSTS, L'|'))
                listed = listed || _wcsicmp(h.c_str(), host.c_str()) == 0;
            Require(listed);
        }
    }

    // Phiên bản: hợp lệ => chỉ gồm số và dấu chấm; CompareVersion phản xứng và phản xạ
    if (IsValidVersionString(w))
    {
        for (wchar_t ch : w)
            Require((ch >= L'0' && ch <= L'9') || ch == L'.');
    }
    size_t half = w.size() / 2;
    std::wstring a = w.substr(0, half), b = w.substr(half);
    Require(CompareVersion(a, a) == 0);
    Require(CompareVersion(a, b) == -CompareVersion(b, a));

    // Chữ ký: dữ liệu ngẫu nhiên không bao giờ được xác minh là hợp lệ với khóa thật
    std::string raw(reinterpret_cast<const char*>(data), size);
    size_t cut = size / 2;
    Require(!VerifySignature(raw.substr(0, cut), raw.substr(cut)));
    return 0;
}
