#include "stdafx.h"
#include "SkinManager.h"
#include "Common.h"
#include "TrafficMonitor.h"
#include "SettingsHelper.h"

CSkinManager CSkinManager::m_instance;

const std::wstring SKIN_SETTINGS_PREFIX = L"skin_";

void CSkinManager::Init()
{
    if (!m_init)    //Tránh tải lặp lại
    {
        //Lấy tên của tất cả skin
        CCommon::GetFiles((theApp.m_skin_path + L"*").c_str(), [&](const wstring& file_name)
            {
                if (CCommon::IsFolder(theApp.m_skin_path + file_name))
                    m_skins.push_back(file_name);
            });
        if (m_skins.empty())
            m_skins.push_back(L"");

        //Tải thiết lập riêng của tất cả skin
        CSettingsHelper ini;
        std::vector<std::wstring> skin_data_app_names = ini.GetAllAppName(SKIN_SETTINGS_PREFIX);
        for (const std::wstring& skin_name : skin_data_app_names)
        {
            std::wstring app_name = SKIN_SETTINGS_PREFIX + skin_name;
            //Kiểm tra skin có tồn tại không
            if (std::find(m_skins.begin(), m_skins.end(), skin_name) != m_skins.end())
            {
                CSkinFile skin_file;
                if (!skin_file.Load(skin_name))
                    continue;
                //Lấy tất cả mục hiển thị của skin
                std::set<CommonDisplayItem> skin_all_items;
                skin_file.GetSkinDisplayItems(skin_all_items);

                SkinSettingData data;
                //Màu văn bản
                ini.LoadMainWndColors(app_name.c_str(), L"text_color", skin_all_items, data.text_colors, 16384); //Thiết lập màu chữ mặc định tùy theo skin có tồn tại không; khi tệp skin không tồn tại thì màu chữ mặc định là trắng
                data.specify_each_item_color = ini.GetBool(app_name.c_str(), L"specify_each_item_color", false);
                //Phông chữ
                FontInfo default_font{};
                default_font.name = theApp.m_str_table.GetLanguageInfo().default_font_name.c_str();
                default_font.size = 10;
                ini.LoadFontData(app_name.c_str(), data.font, default_font);
                //Văn bản hiển thị
                ini.LoadDisplayStr(app_name.c_str(), data.disp_str, true);
                ini.LoadPluginDisplayStr(app_name.c_str(), data.disp_str, true);

                m_skin_setting_data_map[skin_name] = data;
            }
        }
        m_init = true;
    }
}

CSkinManager& CSkinManager::Instance()
{
    return m_instance;
}

std::wstring CSkinManager::GetSkinName(int index) const
{
    if (index >= 0 && index < static_cast<int>(m_skins.size()))
        return m_skins[index];
    else
        return std::wstring();
}

int CSkinManager::FindSkinIndex(const std::wstring& skin_name) const
{
    std::wstring _skin_name{ skin_name };
    SkinNameNormalize(_skin_name);
    for (size_t i{}; i < m_skins.size(); i++)
    {
        if (m_skins[i] == _skin_name)
            return static_cast<int>(i);
    }
    return 0;
}

bool CSkinManager::GetSkinSettingDataByIndex(int index, SkinSettingData& skin_data)
{
    std::wstring skin_name = GetSkinName(index);
    if (!skin_name.empty())
    {
        auto iter = m_skin_setting_data_map.find(skin_name);
        if (iter != m_skin_setting_data_map.end())
        {
            skin_data = iter->second;
            return true;
        }
    }
    return false;
}

const vector<wstring>& CSkinManager::GetSkinNames() const
{
    return m_skins;
}

int CSkinManager::Size() const
{
    return static_cast<int>(m_skins.size());
}

void CSkinManager::AddSkinSettingData(const std::wstring& skin_name, const SkinSettingData& data)
{
    m_skin_setting_data_map[skin_name] = data;
}

void CSkinManager::Save()
{
    //Lưu cấu hình riêng của tất cả skin
    CSettingsHelper ini;
    for (const auto& data : m_skin_setting_data_map)
    {
        std::wstring app_name = SKIN_SETTINGS_PREFIX + data.first;
        //Màu văn bản
        ini.SaveMainWndColors(app_name.c_str(), L"text_color", data.second.text_colors);
        ini.WriteBool(app_name.c_str(), L"specify_each_item_color", data.second.specify_each_item_color);
        //Phông chữ
        ini.SaveFontData(app_name.c_str(), data.second.font);
        //Văn bản hiển thị
        ini.SaveDisplayStr(app_name.c_str(), data.second.disp_str);
        ini.SavePluginDisplayStr(app_name.c_str(), data.second.disp_str);
    }

    //Xóa cấu hình của các skin không tồn tại
    //Lấy tất cả section cấu hình skin
    auto all_skin_section = ini.GetAllAppName(SKIN_SETTINGS_PREFIX);
    for (const auto& skin_name : all_skin_section)
    {
        //Kiểm tra cấu hình skin có tồn tại không
        auto iter = m_skin_setting_data_map.find(skin_name);
        //Nếu cấu hình skin không tồn tại thì xóa khỏi tệp ini
        if (iter == m_skin_setting_data_map.end())
        {
            ini.RemoveSection(SKIN_SETTINGS_PREFIX + skin_name);
        }
    }

    ini.Save();
}

void CSkinManager::SkinSettingDataFronSkin(SkinSettingData& skin_setting_data, const CSkinFile& skin_file)
{
    //Lấy tất cả mục hiển thị của skin
    std::set<CommonDisplayItem> skin_all_items;
    skin_file.GetSkinDisplayItems(skin_all_items);
    //Lấy màu chữ của skin
    skin_setting_data.specify_each_item_color = skin_file.GetSkinInfo().specify_each_item_color;
    int i{};

    for (const auto& item : skin_all_items)
    {
        skin_setting_data.text_colors[item] = skin_file.GetSkinInfo().TextColor(i);
        i++;
    }
    //Lấy phông chữ của skin
    if (!skin_file.GetSkinInfo().font_info.name.IsEmpty())
    {
        skin_setting_data.font = skin_file.GetSkinInfo().font_info;
    }
    else
    {
        skin_setting_data.font.name = theApp.m_str_table.GetLanguageInfo().default_font_name.c_str();
        skin_setting_data.font.size = 10;
    }
    if (skin_file.GetSkinInfo().font_info.size >= MIN_FONT_SIZE && skin_file.GetSkinInfo().font_info.size <= MAX_FONT_SIZE)
        skin_setting_data.font.size = skin_file.GetSkinInfo().font_info.size;

    //Lấy văn bản hiển thị của mục
    if (!skin_file.GetLayoutInfo().no_label)
    {
        if (!skin_file.GetSkinInfo().display_text.IsInvalid())
        {
            for (const auto& display_item : skin_all_items)
                skin_setting_data.disp_str.Get(display_item) = skin_file.GetSkinInfo().display_text.GetConst(display_item);
        }
        //Lấy văn bản hiển thị mặc định của skin
        else
        {
            //Lấy văn bản hiển thị mặc định của tất cả mục hiển thị
            for (const auto& display_item : skin_all_items)
                skin_setting_data.disp_str.Get(display_item) = display_item.DefaultString(true);
        }
    }
}

void CSkinManager::SkinNameNormalize(std::wstring& skin_name)
{
    //Nếu trước tên có dấu gạch chéo thì bỏ đi
    if (!skin_name.empty() && (skin_name[0] == L'\\' || skin_name[0] == L'/'))
    {
        skin_name = skin_name.substr(1);
    }

}
