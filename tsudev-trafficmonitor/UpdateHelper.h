#pragma once
#include "UpdateDownload.h"

class CUpdateHelper
{
public:
    CUpdateHelper();
    ~CUpdateHelper();

    //Kiểm tra liên kết tải bản cập nhật có thuộc trang Releases của repo này hay không
    static bool IsTrustedDownloadUrl(const std::wstring& url);
    //Kiểm tra định dạng chuỗi phiên bản (vd: 1.86, 1.86.1)
    static bool IsValidVersionString(const std::wstring& version);
    //So sánh phiên bản theo số: trả về -1 nếu a<b, 0 nếu bằng, 1 nếu a>b
    static int CompareVersion(const std::wstring& a, const std::wstring& b);
    //Xác minh chữ ký ECDSA P-256/SHA-256 (base64, dạng thô r||s 64 byte) của dữ liệu bằng khóa FORK_UPDATE_PUBLIC_KEY_HEX
    static bool VerifySignature(const std::string& data, const std::string& signature_base64);

    //Tự tải gói cập nhật vào thư mục Downloads và kiểm SHA-256 (logic ở UpdateDownload.h; xem mô tả ở đó).
    //Gọi ở luồng nền (chặn tới khi xong). progress trả về false để hủy.
    static UpdateDownload::Result DownloadPackage(const std::wstring& url, const std::wstring& expected_sha256,
        std::wstring& saved_path, std::wstring& actual_sha256, UpdateDownload::ProgressFn progress = nullptr, void* context = nullptr);

    bool CheckForUpdate();

    const std::wstring& GetVersion() const;
    const std::wstring& GetLink() const;
    const std::wstring& GetLink64() const;
    const std::wstring& GetLinkArm64ec() const;
    //SHA-256 (hex chữ thường) của gói tải về tương ứng; rỗng nếu thông tin phiên bản không có
    const std::wstring& GetSha256() const;
    const std::wstring& GetSha256x64() const;
    const std::wstring& GetSha256Arm64ec() const;
    //true nếu lần kiểm tra gần nhất thất bại vì chữ ký không hợp lệ (khác với lỗi mạng)
    bool IsSignatureInvalid() const;
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
    std::wstring m_sha256;
    std::wstring m_sha256_x64;
    std::wstring m_sha256_arm64ec;
    bool m_signature_invalid{ false };
    std::wstring m_contents_vi;
    std::wstring m_contents_en;
    bool m_row_data{ true };
};
