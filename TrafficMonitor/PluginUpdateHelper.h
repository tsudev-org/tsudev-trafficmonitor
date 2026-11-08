#pragma once
#include <mutex>

class PluginVersion
{
public:
    PluginVersion();
    PluginVersion(const std::wstring& version);
    PluginVersion(const std::string& version);

    bool operator<(const PluginVersion& another) const;
    bool operator==(const PluginVersion& another) const;

    std::string GetVersionString() const;
    std::wstring GetVersionWString() const;

protected:
    int GetSubVersion(size_t index) const;

private:
    std::vector<int> m_version;
    std::string m_version_str;
};

//Kiểm tra phiên bản mới nhất của plugin.
//CheckForUpdate chạy trên luồng kiểm tra cập nhật, GetPluginLatestVersions được gọi từ luồng giao diện
//(hộp thoại Quản lý plugin), nên m_latest_versions luôn được truy cập dưới khóa m_mutex.
class CPluginUpdateHelper
{
public:
    bool CheckForUpdate();

    //Lấy phiên bản mới nhất của một plugin (trả về bản sao, an toàn khi luồng khác đang cập nhật)
    PluginVersion GetPluginLatestVersions(const std::wstring& file_name) const;

private:
    mutable std::mutex m_mutex;                                 //Bảo vệ m_latest_versions
    std::map<std::wstring, PluginVersion> m_latest_versions;    //Thông tin phiên bản mới nhất của từng plugin lấy được qua mạng
};

