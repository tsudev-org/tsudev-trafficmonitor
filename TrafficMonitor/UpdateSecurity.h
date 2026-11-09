#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////
// UpdateSecurity.h - Logic thuần của kênh cập nhật và kiểm URL (không phụ thuộc MFC).
//
// Tách khỏi CUpdateHelper/CCommon để kiểm thử tự động được (tests/UpdateSecurityTests.cpp,
// chạy trong CI build.yml). CUpdateHelper và CCommon gọi lại các hàm ở đây, nên hành vi
// của ứng dụng không đổi. Chỉ dùng Win32 (bcrypt, crypt32) và thư viện chuẩn.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>
#include "ForkConfig.h"

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")

namespace UpdateSecurity
{
    //Tách chuỗi theo ký tự, bỏ khoảng trắng/ký tự điều khiển hai đầu mỗi phần và bỏ phần rỗng
    //(giống CCommon::StringSplit với tham số mặc định)
    inline std::vector<std::wstring> SplitTrim(const std::wstring& str, wchar_t div_ch)
    {
        std::vector<std::wstring> results;
        size_t start = 0;
        while (true)
        {
            size_t pos = str.find(div_ch, start);
            std::wstring part = str.substr(start, pos == std::wstring::npos ? std::wstring::npos : pos - start);
            size_t b = 0, e = part.size();
            while (b < e && part[b] <= 32)
                b++;
            while (e > b && part[e - 1] <= 32)
                e--;
            if (e > b)
                results.push_back(part.substr(b, e - b));
            if (pos == std::wstring::npos)
                break;
            start = pos + 1;
        }
        return results;
    }

    //URL bắt đầu bằng "https://" (không phân biệt hoa thường), dài tối đa 2048, không có khoảng trắng/ký tự điều khiển
    inline bool IsHttpsUrl(const std::wstring& url)
    {
        if (url.size() <= 8 || url.size() > 2048 || _wcsnicmp(url.c_str(), L"https://", 8) != 0)
            return false;
        for (wchar_t ch : url)
        {
            if (ch <= L' ' || ch == 0x7f)
                return false;
        }
        return true;
    }

    //URL HTTPS có host khớp đúng một mục trong danh sách hosts (ngăn cách bởi '|').
    //Từ chối URL có thông tin đăng nhập ("user@host"), cổng ("host:port") hoặc dấu '\'.
    inline bool IsTrustedHostUrl(const std::wstring& url, const std::wstring& hosts = FORK_TRUSTED_HOSTS)
    {
        if (!IsHttpsUrl(url))
            return false;
        size_t start = 8;   //sau "https://"
        size_t end = url.find_first_of(L"/?#", start);
        std::wstring authority = url.substr(start, end == std::wstring::npos ? std::wstring::npos : end - start);
        if (authority.empty() || authority.find_first_of(L"@:\\") != std::wstring::npos)
            return false;
        for (const auto& host : SplitTrim(hosts, L'|'))
        {
            if (_wcsicmp(host.c_str(), authority.c_str()) == 0)
                return true;
        }
        return false;
    }

    //Liên kết tải bản cập nhật phải:
    //  1. là HTTPS hợp lệ (không khoảng trắng/ký tự điều khiển),
    //  2. bắt đầu đúng bằng tiền tố trang Releases của repo này (FORK_RELEASE_DOWNLOAD_PREFIX),
    //  3. không chứa "..", "@", "?", "#", "\" hoặc "%" (chống vượt đường dẫn, chuyển hướng tên miền, tham số lạ),
    //  4. trỏ tới tệp .zip hoặc .exe.
    inline bool IsTrustedDownloadUrl(const std::wstring& url, const std::wstring& prefix = FORK_RELEASE_DOWNLOAD_PREFIX)
    {
        if (!IsHttpsUrl(url))
            return false;
        if (url.size() <= prefix.size() || url.compare(0, prefix.size(), prefix) != 0)
            return false;
        if (url.find(L"..") != std::wstring::npos || url.find_first_of(L"@?#\\%") != std::wstring::npos)
            return false;
        auto ends_with = [&](const wchar_t* ext) {
            size_t len = wcslen(ext);
            return url.size() > len && _wcsicmp(url.c_str() + url.size() - len, ext) == 0;
        };
        return ends_with(L".zip") || ends_with(L".exe");
    }

    //Phiên bản hợp lệ: 1~4 nhóm số ngăn cách bởi dấu chấm, mỗi nhóm tối đa 5 chữ số, tổng dài tối đa 23
    inline bool IsValidVersionString(const std::wstring& version)
    {
        if (version.empty() || version.size() > 23)
            return false;
        int group_len = 0;
        int group_count = 1;
        for (wchar_t ch : version)
        {
            if (ch >= L'0' && ch <= L'9')
            {
                if (++group_len > 5)
                    return false;
            }
            else if (ch == L'.')
            {
                if (group_len == 0 || ++group_count > 4)
                    return false;
                group_len = 0;
            }
            else
            {
                return false;
            }
        }
        return group_len > 0;
    }

    //So sánh phiên bản theo giá trị số từng nhóm (1.100 > 1.86 > 1.9; 26.11.0902 > 26.11.0901 > 1.91):
    //trả về -1 nếu a<b, 0 nếu bằng, 1 nếu a>b. Nhóm thiếu coi là 0.
    inline int CompareVersion(const std::wstring& a, const std::wstring& b)
    {
        std::vector<std::wstring> pa = SplitTrim(a, L'.');
        std::vector<std::wstring> pb = SplitTrim(b, L'.');
        size_t n = (pa.size() > pb.size()) ? pa.size() : pb.size();
        for (size_t i = 0; i < n; i++)
        {
            long va = (i < pa.size()) ? wcstol(pa[i].c_str(), nullptr, 10) : 0;
            long vb = (i < pb.size()) ? wcstol(pb[i].c_str(), nullptr, 10) : 0;
            if (va != vb)
                return va < vb ? -1 : 1;
        }
        return 0;
    }

    //Xác minh chữ ký ECDSA P-256/SHA-256 của data. signature_base64 là base64 của chữ ký thô r||s (64 byte);
    //public_key_hex là X||Y (128 ký tự hex). Mặc định dùng khóa FORK_UPDATE_PUBLIC_KEY_HEX.
    inline bool VerifySignature(const std::string& data, const std::string& signature_base64,
        const char* public_key_hex = FORK_UPDATE_PUBLIC_KEY_HEX)
    {
        const size_t KEY_LEN = 32;
        if (public_key_hex == nullptr || strlen(public_key_hex) != KEY_LEN * 4)
            return false;
        std::vector<BYTE> key_blob(sizeof(BCRYPT_ECCKEY_BLOB) + KEY_LEN * 2);
        auto* header = reinterpret_cast<BCRYPT_ECCKEY_BLOB*>(key_blob.data());
        header->dwMagic = BCRYPT_ECDSA_PUBLIC_P256_MAGIC;
        header->cbKey = static_cast<ULONG>(KEY_LEN);
        for (size_t i = 0; i < KEY_LEN * 2; i++)
        {
            unsigned int value{};
            if (sscanf_s(public_key_hex + i * 2, "%2x", &value) != 1)
                return false;
            key_blob[sizeof(BCRYPT_ECCKEY_BLOB) + i] = static_cast<BYTE>(value);
        }

        //Giải mã chữ ký base64 (bỏ khoảng trắng/xuống dòng ở cuối); chữ ký thô r||s phải đúng 64 byte
        std::string sig_text = signature_base64;
        while (!sig_text.empty() && isspace(static_cast<unsigned char>(sig_text.back())))
            sig_text.pop_back();
        if (sig_text.empty() || sig_text.size() > 256)
            return false;
        BYTE sig[128]{};
        DWORD sig_len = sizeof(sig);
        if (!CryptStringToBinaryA(sig_text.c_str(), static_cast<DWORD>(sig_text.size()), CRYPT_STRING_BASE64, sig, &sig_len, nullptr, nullptr)
            || sig_len != KEY_LEN * 2)
            return false;

        bool verified{ false };
        BCRYPT_ALG_HANDLE hash_alg{}, ecdsa_alg{};
        BCRYPT_HASH_HANDLE hash{};
        BCRYPT_KEY_HANDLE key{};
        BYTE digest[32]{};
        if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hash_alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0))
            && BCRYPT_SUCCESS(BCryptCreateHash(hash_alg, &hash, nullptr, 0, nullptr, 0, 0))
            && BCRYPT_SUCCESS(BCryptHashData(hash, reinterpret_cast<PUCHAR>(const_cast<char*>(data.data())), static_cast<ULONG>(data.size()), 0))
            && BCRYPT_SUCCESS(BCryptFinishHash(hash, digest, sizeof(digest), 0))
            && BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&ecdsa_alg, BCRYPT_ECDSA_P256_ALGORITHM, nullptr, 0))
            && BCRYPT_SUCCESS(BCryptImportKeyPair(ecdsa_alg, nullptr, BCRYPT_ECCPUBLIC_BLOB, &key, key_blob.data(), static_cast<ULONG>(key_blob.size()), 0)))
        {
            verified = BCRYPT_SUCCESS(BCryptVerifySignature(key, nullptr, digest, sizeof(digest), sig, sig_len, 0));
        }
        if (key) BCryptDestroyKey(key);
        if (hash) BCryptDestroyHash(hash);
        if (ecdsa_alg) BCryptCloseAlgorithmProvider(ecdsa_alg, 0);
        if (hash_alg) BCryptCloseAlgorithmProvider(hash_alg, 0);
        return verified;
    }
}
