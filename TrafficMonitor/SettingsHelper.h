#pragma once
#include "IniHelper.h"
#include "CommonData.h"

class CSettingsHelper : public CIniHelper
{
public:
    CSettingsHelper();
    CSettingsHelper(const std::wstring& file_path);

    void SaveFontData(const wchar_t* AppName, const FontInfo& font);
    void LoadFontData(const wchar_t* AppName, FontInfo& font, const FontInfo& default_font) const;

    /**
     * @brief  Đọc thiết lập màu của cửa sổ chính
     * @param AppName[in] Section của tệp ini
     * @param KeyName[in] Key của tệp ini
     * @param text_colors[out] Lưu các màu đọc được
     * @param default_color[in] Màu mặc định
     */
    void LoadMainWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::set<CommonDisplayItem>& all_items, std::map<CommonDisplayItem, COLORREF>& text_colors, COLORREF default_color);
    
    /**
     * @brief  Lưu thiết lập màu của cửa sổ chính
     * @param AppName Section của tệp ini
     * @param KeyName Key của tệp ini
     * @param text_colors Các màu cần lưu
     */
    void SaveMainWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::map<CommonDisplayItem, COLORREF>& text_colors);

    void LoadTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, std::map<CommonDisplayItem, TaskbarItemColor>& text_colors, const wchar_t* default_str);
    void LoadTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, std::map<CommonDisplayItem, TaskbarItemColor>& text_colors, COLORREF default_color);
    void SaveTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::map<CommonDisplayItem, TaskbarItemColor>& text_colors);

    void LoadDisplayStr(const wchar_t* AppName, DispStrings& disp_str, bool is_main_window) const;
    void SaveDisplayStr(const wchar_t* AppName, const DispStrings& disp_str);

    void LoadPluginDisplayStr(const wchar_t* AppName, DispStrings& disp_str, bool is_main_window);
    void SavePluginDisplayStr(const wchar_t* AppName, const DispStrings& disp_str);

};

