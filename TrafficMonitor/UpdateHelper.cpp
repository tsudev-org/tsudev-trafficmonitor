#include "stdafx.h"
#include "UpdateHelper.h"
#include "SimpleXML.h"
#include "Common.h"
#include "TrafficMonitor.h"
#include "UpdateSecurity.h"


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

//Logic thuần nằm ở UpdateSecurity.h (có kiểm thử tự động: tests/UpdateSecurityTests.cpp)
bool CUpdateHelper::VerifySignature(const std::string& data, const std::string& signature_base64)
{
    return UpdateSecurity::VerifySignature(data, signature_base64);
}

UpdateDownload::Result CUpdateHelper::DownloadPackage(const std::wstring& url, const std::wstring& expected_sha256,
    std::wstring& saved_path, std::wstring& actual_sha256, UpdateDownload::ProgressFn progress, void* context)
{
    //Kiểm lại liên kết ngay trước khi tải (phòng thủ nhiều lớp)
    if (!IsTrustedDownloadUrl(url))
        return UpdateDownload::Result::Unsupported;
    UpdateDownload::Options options;
    options.user_agent = std::wstring(L"TrafficMonitor-VN/") + VERSION;
    options.progress = progress;
    options.context = context;
    std::wstring log;
    options.log = &log;
    UpdateDownload::Result result = UpdateDownload::DownloadPackage(url, expected_sha256, UpdateDownload::GetDownloadsFolder(),
        options, saved_path, actual_sha256);
    if (theApp.m_debug_log)
    {
        std::wstring info = L"DownloadPackage: result ";
        info += UpdateDownload::ResultName(result);
        if (!log.empty())
            info += L"; " + log;
        CCommon::WriteLog(info.c_str(), theApp.m_log_path.c_str());
    }
    return result;
}

bool CUpdateHelper::IsTrustedDownloadUrl(const std::wstring& url)
{
    //Kể cả khi thông tin phiên bản bị giả mạo, chương trình cũng không mở liên kết ngoài trang Releases của repo này
    return UpdateSecurity::IsTrustedDownloadUrl(url);
}

bool CUpdateHelper::IsValidVersionString(const std::wstring& version)
{
    return UpdateSecurity::IsValidVersionString(version);
}

int CUpdateHelper::CompareVersion(const std::wstring& a, const std::wstring& b)
{
    return UpdateSecurity::CompareVersion(a, b);
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
