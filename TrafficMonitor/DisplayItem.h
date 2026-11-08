#pragma once
#include "PluginInterface.h"

//Mục hiển thị tích hợp sẵn
enum DisplayItem
{
    TDI_UP,
    TDI_DOWN,
    TDI_CPU,
    TDI_MEMORY,
    TDI_GPU_USAGE,
    TDI_CPU_TEMP,
    TDI_GPU_TEMP,
    TDI_HDD_TEMP,
    TDI_MAIN_BOARD_TEMP,
    TDI_HDD_USAGE,
    TDI_TOTAL_SPEED,
    TDI_CPU_FREQ,
    TDI_TODAY_TRAFFIC
};

//Tập hợp tất cả mục hiển thị tích hợp sẵn
const std::set<DisplayItem> AllDisplayItems
{
    TDI_UP, TDI_DOWN, TDI_CPU, TDI_MEMORY, TDI_GPU_USAGE
#ifndef WITHOUT_TEMPERATURE
    , TDI_CPU_TEMP, TDI_GPU_TEMP, TDI_HDD_TEMP, TDI_MAIN_BOARD_TEMP
#endif
    , TDI_HDD_USAGE, TDI_CPU_FREQ, TDI_TOTAL_SPEED, TDI_TODAY_TRAFFIC
};

//Mục hiển thị
class CommonDisplayItem
{
public:
    CommonDisplayItem() {}
    CommonDisplayItem(DisplayItem item);
    CommonDisplayItem(IPluginItem* item);

    bool operator<(const CommonDisplayItem&) const;
    bool operator==(const CommonDisplayItem&) const;

    //Có phải mục plugin hay không
    bool IsPlugin() const;

    //Lấy mục hiển thị tích hợp sẵn
    DisplayItem ItemType() const;

    //Lấy mục hiển thị của plugin
    IPluginItem* PluginItem() const;

    //Lấy tên của mục hiển thị
    CString GetItemName() const;

    //Lấy văn bản hiển thị mặc định của một mục hiển thị
    std::wstring DefaultString(bool is_main_window) const;

    //Lấy tên key lưu văn bản hiển thị của một mục hiển thị trong tệp ini
    const wchar_t* GetItemIniKeyName() const;

    /**
     * @brief   Lấy văn bản giá trị của một mục hiển thị
     * @param   is_main_window Nếu là true thì là cửa sổ chính, ngược lại là cửa sổ thanh tác vụ
     * @return  Văn bản hiển thị
     */
    CString GetItemValueText(bool is_main_window) const;

    /**
     * @brief   Lấy văn bản ví dụ giá trị của một mục hiển thị
     * @param   is_main_window Nếu là true thì là cửa sổ chính, ngược lại là cửa sổ thanh tác vụ
     * @return  Văn bản ví dụ
     */
    CString GetItemValueSampleText(bool is_main_window) const;

    //Có chiếm trọn hai dòng trên thanh tác vụ hay không
    bool IsDoubleLineExclusive() const;

private:
    bool is_plugin{};           //Có phải mục plugin hay không
    DisplayItem item_type{};    //Mục hiển thị tích hợp sẵn
    IPluginItem* plugin_item{}; //Mục hiển thị của plugin
};


class DisplayItemSet
{
public:
    DisplayItemSet(){}
    DisplayItemSet(std::initializer_list<DisplayItem> items);

    void Add(DisplayItem item);
    void Remove(DisplayItem item);
    bool Contains(DisplayItem item) const;

    int ToInt() const;
    void FromInt(int value);
    bool IsEmpty() const;

private:
    std::set<DisplayItem> data;
};
