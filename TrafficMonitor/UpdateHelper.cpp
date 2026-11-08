#include "stdafx.h"
#include "UpdateHelper.h"
#include "SimpleXML.h"
#include "Common.h"
#include "TrafficMonitor.h"


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
    wstring version_info;
    m_row_data = true;
    if (!CCommon::GetURL(FORK_VERSION_INFO_RAW_URL, version_info, true))     //Lấy thông tin phiên bản
        return false;

    ParseUpdateInfo(version_info);

    return true;
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
