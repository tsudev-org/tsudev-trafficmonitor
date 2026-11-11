#include "stdafx.h"
#include "CommonData.h"
#include "Common.h"
#include "CalendarHelper.h"
#include "TsudevTrafficMonitor.h"
#include "WindowsSettingHelper.h"

///////////////////////////////////////////////////////////////////////////////////
int Date::week() const
{
    //Tính hôm nay là ngày thứ mấy trong năm
    int days{};
    for (int i{ 1 }; i < month; i++)
    {
        days += CCalendarHelper::DaysInMonth(year, i);
    }
    days += day;
    //Tính ngày 1/1 của năm này là thứ mấy
    int week_day = CCalendarHelper::CaculateWeekDay(year, 1, 1);
    if (theApp.m_cfg_data.m_sunday_first)
    {
        days += (week_day - 1);
    }
    else
    {
        days += (week_day - 2);
    }
    return days / 7 + 1;
}

bool Date::DateGreater(const Date& a, const Date& b)
{
    if (a.year != b.year)
        return a.year > b.year;
    else if (a.month != b.month)
        return a.month > b.month;
    else if (a.day != b.day)
        return a.day > b.day;
    else
        return false;
}

bool Date::DateEqual(const Date& a, const Date& b)
{
    return a.year == b.year && a.month == b.month && a.day == b.day;
}


///////////////////////////////////////////////////////////////////////////////////
//HistoryTraffic
unsigned __int64 HistoryTraffic::kBytes() const
{
    return up_kBytes + down_kBytes;
}


///////////////////////////////////////////////////////////////////////////////////
wstring& DispStrings::Get(CommonDisplayItem item)
{
    return map_str[item];
}

const wstring& DispStrings::GetConst(CommonDisplayItem item) const
{
    auto iter = map_str.find(item);
    if (iter != map_str.end())
        return iter->second;
    static wstring empty_str;
    return empty_str;
}

const std::map<CommonDisplayItem, wstring>& DispStrings::GetAllItems() const
{
    return map_str;
}

bool DispStrings::operator==(const DispStrings& disp_str) const
{
    return map_str == disp_str.map_str;
}

bool DispStrings::IsInvalid() const
{
    return map_str.empty();
}

void DispStrings::Load(const std::wstring& plugin_id, const std::wstring& disp_str)
{
    auto plugin = theApp.m_plugins.GetItemById(plugin_id);
    if (plugin != nullptr)
    {
        map_str[plugin] = disp_str;
    }
}

///////////////////////////////////////////////////////////////////////////////////
bool FontInfo::operator==(const FontInfo& a) const
{
    return name == a.name && size == a.size && bold == a.bold && italic == a.italic
        && underline == a.underline && strike_out == a.strike_out;
}

///////////////////////////////////////////////////////////////////////////////////
bool StringSet::Contains(const std::wstring& str) const
{
    return string_set.count(str) != 0;
}

void StringSet::SetStrContained(const std::wstring& str, bool contained)
{
    if (contained)
        string_set.insert(str);
    else
        string_set.erase(str);
}

void StringSet::FromString(const std::wstring& str)
{
    std::vector<std::wstring> item_vect;
    CCommon::StringSplit(str, L',', item_vect);
    string_set.clear();
    for (const auto& i : item_vect)
        string_set.insert(i);
}

std::wstring StringSet::ToString() const
{
    std::vector<std::wstring> item_vect;
    for (const auto& i : string_set)
        item_vect.push_back(i);
    std::wstring item_str;
    for (const auto& i : item_vect)
    {
        item_str += i;
        item_str += L',';
    }
    if (!item_str.empty())
        item_str.pop_back();
    return item_str;
}

void StringSet::FromVector(const std::vector<std::wstring>& vec)
{
    string_set.clear();
    for (const auto& str : vec)
        string_set.insert(str);
}

std::vector<std::wstring> StringSet::ToVector() const
{
    std::vector<std::wstring> vec;
    for (const auto& str : string_set)
        vec.push_back(str);
    return vec;
}

std::set<std::wstring>& StringSet::data()
{
    return string_set;
}

///////////////////////////////////////////////////////////////////////////////////
bool SkinSettingData::IsEmpty() const
{
    return font.name.IsEmpty() && disp_str.GetAllItems().empty() && text_colors.empty();
}

bool SkinSettingData::operator==(const SkinSettingData& a) const
{
    return font == a.font && disp_str == a.disp_str && text_colors == a.text_colors && specify_each_item_color == a.specify_each_item_color;
}

///////////////////////////////////////////////////////////////////////////////////
void MainWndSettingData::FormSkinSettingData(const SkinSettingData& sking_setting_data)
{
    font = sking_setting_data.font;
    disp_str = sking_setting_data.disp_str;
    text_colors = sking_setting_data.text_colors;
    specify_each_item_color = sking_setting_data.specify_each_item_color;
}

SkinSettingData MainWndSettingData::ToSkinSettingData() const
{
    SkinSettingData sking_setting_data;
    sking_setting_data.font = font;
    sking_setting_data.disp_str = disp_str;
    sking_setting_data.text_colors = text_colors;
    sking_setting_data.specify_each_item_color = specify_each_item_color;
    return sking_setting_data;
}

///////////////////////////////////////////////////////////////////////////////////
bool TaskBarSettingData::IsTaskbarTransparent() const
{
    if (CWindowsSettingHelper::IsWindows10LightTheme() || theApp.m_win_version.IsWindows8Or8point1() || theApp.IsWindows11Taskbar())
        return (transparent_color == back_color);
    else
        return transparent_color == 0;
}

void TaskBarSettingData::SetTaskabrTransparent(bool transparent)
{
    if (transparent)
    {
        if (CWindowsSettingHelper::IsWindows10LightTheme() || theApp.m_win_version.IsWindows8Or8point1() || theApp.IsWindows11Taskbar())
        {
            //Ở chế độ sáng, để cửa sổ thanh tác vụ trong suốt chỉ cần đặt màu trong suốt giống màu nền
            CCommon::TransparentColorConvert(back_color);
            transparent_color = back_color;
        }
        else
        {
            //Ở chế độ tối, để nền trong suốt thì đặt màu trong suốt là màu đen
            transparent_color = 0;
        }
    }
    else
    {
        //Để cửa sổ thanh tác vụ không trong suốt chỉ cần đặt màu trong suốt khác màu nền
        if (back_color != TASKBAR_TRANSPARENT_COLOR1)
            transparent_color = TASKBAR_TRANSPARENT_COLOR1;
        else
            transparent_color = TASKBAR_TRANSPARENT_COLOR2;
    }
}

void TaskBarSettingData::ValidItemSpace()
{
    if (item_space < 0)
        item_space = 0;
    if (item_space > 32)
        item_space = 32;
}

void TaskBarSettingData::ValidVerticalMargin()
{
    if (vertical_margin < -10)
        vertical_margin = -10;
    if (vertical_margin > 10)
        vertical_margin = 10;
}

void TaskBarSettingData::ValidWindowOffsetTop()
{
    if (window_offset_top < -20)
        window_offset_top = -20;
    if (window_offset_top > 20)
        window_offset_top = 20;
}

void TaskBarSettingData::ValidWindowOffsetLeft()
{
    if (window_offset_left < -800)
        window_offset_top = -800;
    if (window_offset_top > 800)
        window_offset_top = 800;
}

unsigned __int64 TaskBarSettingData::GetNetspeedFigureMaxValueInBytes() const
{
    if (netspeed_figure_max_value_unit == 0)        //Đơn vị là KB
        return static_cast<unsigned __int64>(netspeed_figure_max_value) * 1024;
    else
        return static_cast<unsigned __int64>(netspeed_figure_max_value) * 1024 * 1024;
}

COLORREF TaskBarSettingData::GetUsageGraphColor() const
{
    if (graph_color_following_system)
    {
        COLORREF theme_color = theApp.GetThemeColor();
        //Chuyển sang HLS
        double h, l, s;
        CDrawingManager::RGBtoHSL(theme_color, &h, &s, &l);
        //Chỉ định độ sáng theo chế độ sáng/tối hiện tại của hệ thống
        if (theApp.m_last_light_mode)
        {
            //Thanh tác vụ màu sáng, đặt độ sáng là 0.7
            l = 0.7;
        }
        else
        {
            //Thanh tác vụ màu tối, đặt độ sáng là 0.4
            l = 0.4;
        }
        //Chuyển ngược về RGB
        COLORREF graph_color = CDrawingManager::HLStoRGB_ONE(h, l, s);
        return graph_color;
    }
    else
    {
        return status_bar_color;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool LanguageInfo::operator==(const LanguageInfo& another) const
{
    if (isEmpty() && another.isEmpty())
        return true;
    return bcp_47 == another.bcp_47 && display_name == another.display_name && translator == another.translator;
}


wstring LanguageInfo::toConfigString() const
{
    return bcp_47 + L"|" + display_name + L"|" + translator;
}

void LanguageInfo::fromConfigString(const wstring& config_str)
{
    std::vector<wstring> parts;
    CCommon::StringSplit(config_str, L'|', parts, false);
    if (parts.size() >= 1)
    {
        bcp_47 = parts[0];
    }
    if (parts.size() >= 2)
    {
        display_name = parts[1];
    }
    if (parts.size() >= 3)
    {
        translator = parts[2];
    }
    language_id = LANGIDFROMLCID(LocaleNameToLCID(bcp_47.c_str(), 0));
}

bool LanguageInfo::isEmpty() const
{
    return bcp_47 == L"0" || bcp_47.empty();
}
