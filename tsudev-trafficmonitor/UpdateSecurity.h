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
    //  2. bắt đầu đúng bằng một tiền tố trang Releases của repo này (FORK_RELEASE_DOWNLOAD_PREFIX hoặc
    //     FORK_RELEASE_DOWNLOAD_PREFIX_ORG sau khi chuyển repo sang tsudev-org; danh sách ngăn cách bởi '|'),
    //  3. không chứa "..", "@", "?", "#", "\" hoặc "%" (chống vượt đường dẫn, chuyển hướng tên miền, tham số lạ),
    //  4. trỏ tới tệp .zip hoặc .exe.
    inline bool IsTrustedDownloadUrl(const std::wstring& url,
        const std::wstring& prefixes = std::wstring(FORK_RELEASE_DOWNLOAD_PREFIX) + L"|" + FORK_RELEASE_DOWNLOAD_PREFIX_ORG)
    {
        if (!IsHttpsUrl(url))
            return false;
        bool prefix_ok = false;
        for (const auto& prefix : SplitTrim(prefixes, L'|'))
        {
            if (url.size() > prefix.size() && url.compare(0, prefix.size(), prefix) == 0)
                prefix_ok = true;
        }
        if (!prefix_ok)
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

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Tải và kiểm SHA-256 gói cập nhật ngay trong ứng dụng (từ 26.11.0905)
    ////////////////////////////////////////////////////////////////////////////////////////////////

    //Kích thước tối đa của gói cập nhật tải về (gói thật khoảng 2-3 MB)
    const unsigned long long MAX_UPDATE_PACKAGE_BYTES = 50ull * 1024 * 1024;

    //Chuỗi SHA-256 dạng hex: đúng 64 ký tự 0-9, a-f, A-F
    inline bool IsSha256Hex(const std::wstring& value)
    {
        if (value.size() != 64)
            return false;
        for (wchar_t ch : value)
        {
            if (!((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f') || (ch >= L'A' && ch <= L'F')))
                return false;
        }
        return true;
    }

    //So sánh hai SHA-256 dạng hex, không phân biệt hoa thường. Chuỗi không hợp lệ (kể cả rỗng) không bao giờ khớp.
    inline bool Sha256Equals(const std::wstring& a, const std::wstring& b)
    {
        if (!IsSha256Hex(a) || !IsSha256Hex(b))
            return false;
        return _wcsicmp(a.c_str(), b.c_str()) == 0;
    }

    //Chuyển byte sang chuỗi hex chữ thường
    inline std::wstring ToHexLower(const unsigned char* data, size_t size)
    {
        static const wchar_t digits[] = L"0123456789abcdef";
        std::wstring result;
        result.reserve(size * 2);
        for (size_t i = 0; i < size; i++)
        {
            result.push_back(digits[data[i] >> 4]);
            result.push_back(digits[data[i] & 0x0f]);
        }
        return result;
    }

    //Tên tệp an toàn để lưu gói cập nhật, lấy từ liên kết tải đã qua IsTrustedDownloadUrl:
    //phần sau dấu '/' cuối cùng, dài 5~128 ký tự, chỉ gồm A-Z a-z 0-9 . _ -, không bắt đầu bằng '.',
    //đuôi .zip (không nhận .exe), không phải tên thiết bị DOS (CON, NUL, COM1...). Rỗng nếu không đạt.
    inline std::wstring SafeFileNameFromUrl(const std::wstring& url)
    {
        if (!IsTrustedDownloadUrl(url))
            return std::wstring();
        size_t slash = url.find_last_of(L'/');
        if (slash == std::wstring::npos)
            return std::wstring();
        std::wstring name = url.substr(slash + 1);
        if (name.size() < 5 || name.size() > 128 || name[0] == L'.')
            return std::wstring();
        for (wchar_t ch : name)
        {
            bool ok = (ch >= L'A' && ch <= L'Z') || (ch >= L'a' && ch <= L'z') || (ch >= L'0' && ch <= L'9')
                || ch == L'.' || ch == L'_' || ch == L'-';
            if (!ok)
                return std::wstring();
        }
        if (_wcsicmp(name.c_str() + name.size() - 4, L".zip") != 0)
            return std::wstring();
        //Tên thiết bị DOS: so phần trước dấu chấm đầu tiên
        std::wstring stem = name.substr(0, name.find(L'.'));
        static const wchar_t* const reserved[] = { L"CON", L"PRN", L"AUX", L"NUL",
            L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6", L"COM7", L"COM8", L"COM9",
            L"LPT1", L"LPT2", L"LPT3", L"LPT4", L"LPT5", L"LPT6", L"LPT7", L"LPT8", L"LPT9" };
        for (const wchar_t* r : reserved)
        {
            if (_wcsicmp(stem.c_str(), r) == 0)
                return std::wstring();
        }
        return name;
    }

    //Chọn tên tệp chưa tồn tại để không ghi đè tệp đang có: "a.zip", rồi "a (2).zip" ... "a (99).zip".
    //exists(tên) do nơi gọi cung cấp (kiểm trên đĩa). Trả về rỗng nếu mọi tên đều đã có.
    template <class ExistsFn>
    inline std::wstring UniqueFileName(const std::wstring& name, ExistsFn exists)
    {
        if (name.empty())
            return std::wstring();
        if (!exists(name))
            return name;
        size_t dot = name.find_last_of(L'.');
        std::wstring stem = (dot == std::wstring::npos || dot == 0) ? name : name.substr(0, dot);
        std::wstring ext = (dot == std::wstring::npos || dot == 0) ? std::wstring() : name.substr(dot);
        for (int i = 2; i <= 99; i++)
        {
            std::wstring candidate = stem + L" (" + std::to_wstring(i) + L")" + ext;
            if (!exists(candidate))
                return candidate;
        }
        return std::wstring();
    }

    //Tính SHA-256 theo từng khối (BCrypt), dùng khi tải gói: băm trong lúc ghi ra đĩa
    class Sha256Stream
    {
    public:
        Sha256Stream()
        {
            m_ok = BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&m_alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0))
                && BCRYPT_SUCCESS(BCryptCreateHash(m_alg, &m_hash, nullptr, 0, nullptr, 0, 0));
        }
        ~Sha256Stream()
        {
            if (m_hash) BCryptDestroyHash(m_hash);
            if (m_alg) BCryptCloseAlgorithmProvider(m_alg, 0);
        }
        Sha256Stream(const Sha256Stream&) = delete;
        Sha256Stream& operator=(const Sha256Stream&) = delete;

        bool IsValid() const { return m_ok && !m_finished; }

        bool Update(const void* data, size_t size)
        {
            if (!IsValid())
                return false;
            const unsigned char* p = static_cast<const unsigned char*>(data);
            while (size > 0)
            {
                ULONG chunk = size > 0x10000000 ? 0x10000000UL : static_cast<ULONG>(size);
                if (!BCRYPT_SUCCESS(BCryptHashData(m_hash, const_cast<PUCHAR>(p), chunk, 0)))
                {
                    m_ok = false;
                    return false;
                }
                p += chunk;
                size -= chunk;
            }
            return true;
        }

        //Kết thúc và trả về SHA-256 dạng hex chữ thường; rỗng nếu lỗi. Chỉ gọi được một lần.
        std::wstring FinishHex()
        {
            if (!IsValid())
                return std::wstring();
            m_finished = true;
            unsigned char digest[32]{};
            if (!BCRYPT_SUCCESS(BCryptFinishHash(m_hash, digest, sizeof(digest), 0)))
                return std::wstring();
            return ToHexLower(digest, sizeof(digest));
        }

    private:
        BCRYPT_ALG_HANDLE m_alg{};
        BCRYPT_HASH_HANDLE m_hash{};
        bool m_ok{ false };
        bool m_finished{ false };
    };

    //SHA-256 của cả khối dữ liệu (hex chữ thường)
    inline std::wstring Sha256Hex(const void* data, size_t size)
    {
        Sha256Stream stream;
        if (!stream.Update(data, size))
            return std::wstring();
        return stream.FinishHex();
    }
}
