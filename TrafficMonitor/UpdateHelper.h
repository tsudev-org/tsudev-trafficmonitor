#pragma once
class CUpdateHelper
{
public:
    CUpdateHelper();
    ~CUpdateHelper();

    //Giữ enum để tương thích cấu hình cũ; bản Việt hóa luôn dùng GitHub
    enum class UpdateSource
    {
        GitHubSource,
        GiteeSource
    };

    //Kiểm tra liên kết tải bản cập nhật có thuộc trang Releases của repo này hay không
    static bool IsTrustedDownloadUrl(const std::wstring& url);
    //Kiểm tra định dạng chuỗi phiên bản (vd: 1.86, 1.86.1)
    static bool IsValidVersionString(const std::wstring& version);
    //So sánh phiên bản theo số: trả về -1 nếu a<b, 0 nếu bằng, 1 nếu a>b
    static int CompareVersion(const std::wstring& a, const std::wstring& b);

    void SetUpdateSource(UpdateSource update_source);

    bool CheckForUpdate();

    const std::wstring& GetVersion() const;
    const std::wstring& GetLink() const;
    const std::wstring& GetLink64() const;
    const std::wstring& GetLinkArm64ec() const;
    const std::wstring& GetContentsVi() const;
    const std::wstring& GetContentsEn() const;
    bool IsRowData();

private:
    void ParseUpdateInfo(wstring version_info);

private:
    std::wstring m_version;
    std::wstring m_link;
    std::wstring m_link64;
    std::wstring m_link_arm64ec;
    std::wstring m_contents_vi;
    std::wstring m_contents_en;
    bool m_row_data{ true };
    UpdateSource m_update_source{ UpdateSource::GitHubSource };
};
