#include "stdafx.h"
#include "PluginUpdateHelper.h"
#include "Common.h"
#include "TrafficMonitor.h"
#include "TinyXml2Helper.h"

static void VersionFromString(const std::string& version_str, std::vector<int>& versions)
{
    versions.clear();
    //Tách chuỗi
    std::vector<std::string> vec_version;
    CCommon::StringSplit(version_str, '.', vec_version);
    //Chuyển thành số nguyên để lưu
    for (const auto& str : vec_version)
    {
        //Bỏ các ký tự không phải chữ số
        std::string str_ver;
        for (auto ch : str)
        {
            if (ch >= '0' && ch <= '9')
                str_ver.push_back(ch);
        }

        versions.push_back(atoi(str_ver.c_str()));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
PluginVersion::PluginVersion()
{
}

PluginVersion::PluginVersion(const std::wstring& version)
{
    m_version_str = CCommon::AsciiToStr(version);
    VersionFromString(m_version_str, m_version);
}

PluginVersion::PluginVersion(const std::string& version)
{
    m_version_str = version;
    VersionFromString(m_version_str, m_version);
}

bool PluginVersion::operator<(const PluginVersion& another) const
{
    for (size_t i{}; i < m_version.size() || i < another.m_version.size(); i++)
    {
        const int lhs = GetSubVersion(i);
        const int rhs = another.GetSubVersion(i);

        if (lhs < rhs)
            return true;

        if (lhs > rhs)
            return false;
    }
    return false;
}

bool PluginVersion::operator==(const PluginVersion& another) const
{
    for (size_t i{}; i < m_version.size() || i < another.m_version.size(); i++)
    {
        if (GetSubVersion(i) != another.GetSubVersion(i))
            return false;
    }
    return true;
}

std::string PluginVersion::GetVersionString() const
{
    return m_version_str;
}

std::wstring PluginVersion::GetVersionWString() const
{
    return CCommon::AsciiToUnicode(m_version_str);
}

int PluginVersion::GetSubVersion(size_t index) const
{
    if (index < m_version.size())
        return m_version[index];
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
bool CPluginUpdateHelper::CheckForUpdate()
{
    std::wstring url;
    //Bản Việt hóa: phiên bản plugin của dự án gốc được tra từ nguồn phát hành của chúng (HTTPS), xem ForkConfig.h
    url = UPSTREAM_PLUGINS_VERSION_URL;

    //Lấy nội dung url
    std::string version_info;
    if (CCommon::GetURL(url, version_info))
    {
        //Phân tích xml vào bảng tạm (không giữ khóa trong lúc tải và phân tích)
        std::map<std::wstring, PluginVersion> latest_versions;
        tinyxml2::XMLDocument doc;
        if (doc.Parse(version_info.c_str(), version_info.size()) != tinyxml2::XML_SUCCESS)
            return false;
        //Duyệt các phiên bản plugin
        CTinyXml2Helper::IterateChildNode(doc.RootElement(), [&](tinyxml2::XMLElement* ele) {
            std::wstring file_name = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(ele, "file_name"));
            std::string plugin_version(CTinyXml2Helper::ElementAttribute(ele, "version"));
            if (!file_name.empty())
                latest_versions.emplace(file_name, PluginVersion(plugin_version));
        });
        //Thay toàn bộ bảng dưới khóa: luồng giao diện chỉ thấy bảng cũ hoặc bảng mới hoàn chỉnh
        std::lock_guard<std::mutex> lock(m_mutex);
        m_latest_versions.swap(latest_versions);
        return true;
    }
    return false;
}

PluginVersion CPluginUpdateHelper::GetPluginLatestVersions(const std::wstring& file_name) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto iter = m_latest_versions.find(file_name);
    if (iter != m_latest_versions.end())
    {
        return iter->second;
    }
    return PluginVersion();
}
