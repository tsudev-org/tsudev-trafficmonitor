#include "stdafx.h"
#include "UpdateHelper.h"
#include "SimpleXML.h"
#include "Common.h"
#include "TrafficMonitor.h"
#include <bcrypt.h>
#include <wincrypt.h>
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")


CUpdateHelper::CUpdateHelper()
{
}


CUpdateHelper::~CUpdateHelper()
{
}

void CUpdateHelper::SetUpdateSource(UpdateSource update_source)
{
    m_update_source = update_source;
}

bool CUpdateHelper::CheckForUpdate()
{
    //Bản Việt hóa: chỉ dùng một nguồn cập nhật duy nhất là repo GitHub của TrafficMonitor-VN (xem ForkConfig.h).
    //Lựa chọn nguồn Gitee (nếu có trong cấu hình cũ) được bỏ qua.
    //Từ V1.88-vn: chỉ tin thông tin phiên bản có chữ ký hợp lệ (version_signed.xml + .sig trong bản phát hành mới nhất).
    //Tệp version_utf8.info không chữ ký chỉ còn phục vụ các bản cũ (V1.86-vn, V1.87-vn).
    m_row_data = true;
    m_signature_invalid = false;
    std::string version_info_bytes, signature;
    if (!CCommon::GetURL(FORK_SIGNED_VERSION_URL, version_info_bytes) || !CCommon::GetURL(FORK_SIGNED_VERSION_SIG_URL, signature))
        return false;

    if (!VerifySignature(version_info_bytes, signature))
    {
        if (theApp.m_debug_log)
            CCommon::WriteLog(L"CheckForUpdate: invalid signature of version info, update ignored.", theApp.m_log_path.c_str());
        m_signature_invalid = true;
        m_version.clear();
        return true;    //Kết nối thành công nhưng dữ liệu không đáng tin: GetVersion() rỗng => báo lỗi dữ liệu
    }

    //Bỏ BOM UTF-8 (nếu có) rồi chuyển sang Unicode
    if (version_info_bytes.size() >= 3 && version_info_bytes.compare(0, 3, "\xEF\xBB\xBF") == 0)
        version_info_bytes.erase(0, 3);
    ParseUpdateInfo(CCommon::StrToUnicode(version_info_bytes.c_str(), true));

    return true;
}

bool CUpdateHelper::VerifySignature(const std::string& data, const std::string& signature_base64)
{
    //Giải mã khóa công khai X||Y từ chuỗi hex
    const char* hex = FORK_UPDATE_PUBLIC_KEY_HEX;
    const size_t KEY_LEN = 32;
    if (strlen(hex) != KEY_LEN * 4)
        return false;
    std::vector<BYTE> key_blob(sizeof(BCRYPT_ECCKEY_BLOB) + KEY_LEN * 2);
    auto* header = reinterpret_cast<BCRYPT_ECCKEY_BLOB*>(key_blob.data());
    header->dwMagic = BCRYPT_ECDSA_PUBLIC_P256_MAGIC;
    header->cbKey = static_cast<ULONG>(KEY_LEN);
    for (size_t i = 0; i < KEY_LEN * 2; i++)
    {
        unsigned int value{};
        if (sscanf_s(hex + i * 2, "%2x", &value) != 1)
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

bool CUpdateHelper::IsTrustedDownloadUrl(const std::wstring& url)
{
    //Bảo mật: liên kết tải bản cập nhật phải
    //  1. là HTTPS hợp lệ (không khoảng trắng/ký tự điều khiển),
    //  2. bắt đầu đúng bằng tiền tố trang Releases của repo này (FORK_RELEASE_DOWNLOAD_PREFIX),
    //  3. không chứa "..", "@", "?" hoặc "#" (chống vượt đường dẫn, chuyển hướng tên miền, tham số lạ),
    //  4. trỏ tới tệp .zip hoặc .exe.
    //Nhờ vậy, kể cả khi tệp version_utf8.info bị giả mạo, chương trình cũng không mở liên kết tới nơi khác.
    if (!CCommon::IsHttpsUrl(url))
        return false;
    const std::wstring prefix{ FORK_RELEASE_DOWNLOAD_PREFIX };
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

bool CUpdateHelper::IsValidVersionString(const std::wstring& version)
{
    //Phiên bản hợp lệ: 1~4 nhóm số ngăn cách bởi dấu chấm, mỗi nhóm tối đa 5 chữ số (ví dụ 1.86 hoặc 1.86.1)
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

int CUpdateHelper::CompareVersion(const std::wstring& a, const std::wstring& b)
{
    //So sánh phiên bản theo giá trị số từng nhóm (1.100 > 1.86 > 1.9), thay cho so sánh chuỗi trước đây
    //vốn cho kết quả sai khi số chữ số khác nhau.
    std::vector<std::wstring> pa, pb;
    CCommon::StringSplit(a, L'.', pa);
    CCommon::StringSplit(b, L'.', pb);
    size_t n = (std::max)(pa.size(), pb.size());
    for (size_t i = 0; i < n; i++)
    {
        long va = (i < pa.size()) ? wcstol(pa[i].c_str(), nullptr, 10) : 0;
        long vb = (i < pb.size()) ? wcstol(pb[i].c_str(), nullptr, 10) : 0;
        if (va != vb)
            return va < vb ? -1 : 1;
    }
    return 0;
}

void CUpdateHelper::ParseUpdateInfo(wstring version_info)
{
    CSimpleXML version_xml;
    version_xml.LoadXMLContentDirect(version_info);

    m_version = version_xml.GetNode(L"version");

    //Kiểm tra chuỗi phiên bản: rỗng hoặc sai định dạng nghĩa là tệp thông tin phiên bản không hợp lệ/bị giả mạo
    if (!IsValidVersionString(m_version))
    {
        m_version.clear();
        return;
    }

    const wchar_t* str_source_tag = L"GitHub";
    wstring str_link_tag, str_link_tag_x64, str_link_tag_arm64ec;
#ifdef WITHOUT_TEMPERATURE
    str_link_tag = L"link_without_temperature";
    str_link_tag_x64 = L"link_without_temperature_x64";
    str_link_tag_arm64ec = L"link_without_temperature_arm64ec";
#else
    str_link_tag = L"link";
    str_link_tag_x64 = L"link_x64";
    str_link_tag_arm64ec = L"link_arm64ec";
#endif
    m_link64 = version_xml.GetNode(str_link_tag_x64.c_str(), str_source_tag);
    m_link = version_xml.GetNode(str_link_tag.c_str(), str_source_tag);
    m_link_arm64ec = version_xml.GetNode(str_link_tag_arm64ec.c_str(), str_source_tag);
    //SHA-256 của từng gói (chỉ có trong version_signed.xml); chỉ nhận chuỗi 64 ký tự hex
    auto LoadSha256 = [&](const wchar_t* node) {
        std::wstring value = version_xml.GetNode(node, str_source_tag);
        if (value.size() != 64 || value.find_first_not_of(L"0123456789abcdef") != std::wstring::npos)
            value.clear();
        return value;
    };
    m_sha256 = LoadSha256(L"sha256");
    m_sha256_x64 = LoadSha256(L"sha256_x64");
    m_sha256_arm64ec = LoadSha256(L"sha256_arm64ec");

    //Mọi liên kết tải (nếu có) đều phải đáng tin cậy; chỉ cần một liên kết không hợp lệ là bỏ toàn bộ thông tin cập nhật
    auto IsAcceptable = [](const std::wstring& url) { return url.empty() || IsTrustedDownloadUrl(url); };
    if (!IsAcceptable(m_link) || !IsAcceptable(m_link64) || !IsAcceptable(m_link_arm64ec))
    {
        if (theApp.m_debug_log)
            CCommon::WriteLog(L"CheckForUpdate: untrusted download link in version info, update ignored.", theApp.m_log_path.c_str());
        m_version.clear();
        m_link.clear();
        m_link64.clear();
        m_link_arm64ec.clear();
        return;
    }

    //Nội dung cập nhật: giới hạn độ dài để tránh hộp thoại quá lớn
    auto LoadContents = [&](const wchar_t* node) {
        CString contents = version_xml.GetNode(node, L"update_contents").c_str();
        contents.Replace(L"\\n", L"\r\n");
        if (contents.GetLength() > 4000)
            contents = contents.Left(4000) + L"...";
        return std::wstring(contents.GetString());
    };
    m_contents_vi = LoadContents(L"contents_vi");
    m_contents_en = LoadContents(L"contents_en");
}

const std::wstring& CUpdateHelper::GetVersion() const
{
    return m_version;
}

const std::wstring& CUpdateHelper::GetLink() const
{
    return m_link;
}

const std::wstring& CUpdateHelper::GetLink64() const
{
    return m_link64;
}

const std::wstring& CUpdateHelper::GetLinkArm64ec() const
{
    return m_link_arm64ec;
}

const std::wstring& CUpdateHelper::GetSha256() const
{
    return m_sha256;
}

const std::wstring& CUpdateHelper::GetSha256x64() const
{
    return m_sha256_x64;
}

const std::wstring& CUpdateHelper::GetSha256Arm64ec() const
{
    return m_sha256_arm64ec;
}

bool CUpdateHelper::IsSignatureInvalid() const
{
    return m_signature_invalid;
}

const std::wstring& CUpdateHelper::GetContentsVi() const
{
    return m_contents_vi;
}

const std::wstring& CUpdateHelper::GetContentsEn() const
{
    return m_contents_en;
}

bool CUpdateHelper::IsRowData()
{
    return m_row_data;
}
