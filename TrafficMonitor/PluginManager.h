#pragma once
#include "PluginInterface.h"
#include <map>
#include "TaskbarItemOrderHelper.h"
#include <functional>

typedef ITMPlugin* (*pfTMPluginGetInstance)();

//Dùng để nạp và quản lý plugin
class CPluginManager
{
public:
    //Trạng thái của plugin
    enum class PluginState
    {
        PS_SUCCEED,             //Nạp thành công
        PS_MUDULE_LOAD_FAILED,  //Nạp dll thất bại
        PS_FUNCTION_GET_FAILED, //Lấy hàm của plugin thất bại
        PS_VERSION_NOT_SUPPORT, //Phiên bản plugin không được hỗ trợ
        PS_DISABLE              //Đã vô hiệu hóa
    };

    //Thông tin plugin
    struct PluginInfo
    {
        wstring file_path;      //Đường dẫn tệp
        HMODULE plugin_module{};  //dll module
        ITMPlugin* plugin{};      //Đối tượng plugin
        std::vector<IPluginItem*> plugin_items; //Tất cả mục hiển thị do plugin cung cấp
        PluginState state{};    //Trạng thái của plugin
        DWORD error_code{};     //Mã lỗi (giá trị trả về của GetLastError)
        std::map<ITMPlugin::PluginInfoIndex, std::wstring> properties;    //Thuộc tính plugin
        std::wstring Property(ITMPlugin::PluginInfoIndex) const;
    };

    CPluginManager();
    ~CPluginManager();
    void LoadPlugins();

    const std::vector<IPluginItem*>& GetPluginItems() const;
    const std::vector<PluginInfo>& GetPlugins() const;
    IPluginItem* GetItemById(const std::wstring& item_id);
    IPluginItem* GetItemByIndex(int index);
    int GetItemIndex(IPluginItem* item) const;
    ITMPlugin* GetPluginByItem(IPluginItem* pItem);
    int GetPluginIndex(ITMPlugin* plugin) const;

    //Duyệt tất cả plugin
    //func: tham số là đối tượng ITMPlugin được duyệt tới
    void EnumPlugin(std::function<void(ITMPlugin*)> func) const;

    //Duyệt tất cả mục plugin
    //func: tham số là đối tượng IPluginItem được duyệt tới
    void EnumPluginItem(std::function<void(IPluginItem*)> func) const;

    const std::set<CommonDisplayItem>& AllDisplayItemsWithPlugins();

    int GetItemWidth(IPluginItem* pItem, CDC* pDC);

private:
    static void ReplacePluginDrawTextFunction(HMODULE plgin_dll_module) noexcept;
    static void ReplaceMfcDrawTextFunction() noexcept;

private:
    std::vector<IPluginItem*> m_plugins;
    std::vector<PluginInfo> m_modules;
    std::set<CommonDisplayItem> m_all_display_items_with_plugins;   //Tất cả mục hiển thị trên thanh tác vụ, bao gồm cả của plugin
    std::map<IPluginItem*, ITMPlugin*> m_plguin_item_map;          //Map dùng để tìm plugin tương ứng theo mục plugin
};
