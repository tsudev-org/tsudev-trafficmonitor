#include "stdafx.h"
#include <Windows.h>
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <locale>
#include <initializer_list>
#include <string.h>
#include "DrawTextManager.h"
#include "PluginManager.h"
#include "Common.h"
#include "TsudevTrafficMonitor.h"

#define PLUGIN_UNSUPPORT_VERSION 0      //Phiên bản plugin không được hỗ trợ

CPluginManager::CPluginManager()
{
}

CPluginManager::~CPluginManager()
{
    //Gỡ nạp plugin
    for (const auto& m : m_modules)
        FreeLibrary(m.plugin_module);
}

static wstring WcharArrayToWString(const wchar_t* str)
{
    if (str == nullptr)
        return wstring();
    else
        return wstring(str);
}

void CPluginManager::LoadPlugins()
{
    //Nạp plugin từ thư mục plugins
    wstring plugin_dir;
    plugin_dir = CCommon::GetModuleDir() + L"plugins";
    vector<wstring> plugin_files;
    CCommon::GetFiles((plugin_dir + L"\\*.dll").c_str(), plugin_files);		//Lấy tên tệp của tất cả dll trong thư mục Plugins
    for (const auto& file : plugin_files)
    {
        //Thông tin plugin
        m_modules.push_back(PluginInfo());
        PluginInfo& plugin_info{ m_modules.back() };
        //Đường dẫn dll của plugin
        plugin_info.file_path = plugin_dir + file;
        //Tên tệp plugin
        std::wstring file_name{ file };
        if (!file_name.empty() && (file_name[0] == L'\\' || file_name[0] == L'/'))
            file_name = file_name.substr(1);
        //Nếu plugin bị vô hiệu hóa thì không nạp
        if (theApp.m_cfg_data.plugin_disabled.Contains(file_name))
        {
            plugin_info.state = PluginState::PS_DISABLE;
            continue;
        }
        //Nạp dll
        //Bảo mật: nạp plugin bằng đường dẫn tuyệt đối và chỉ tìm DLL phụ thuộc trong thư mục của plugin
        //cùng các thư mục hệ thống an toàn (không tìm trong thư mục hiện hành/PATH) để chống DLL hijacking.
        plugin_info.plugin_module = CCommon::SafeLoadLibrary(plugin_info.file_path.c_str());
        if (plugin_info.plugin_module == NULL)
        {
            plugin_info.state = PluginState::PS_MUDULE_LOAD_FAILED;
            plugin_info.error_code = GetLastError();
            continue;
        }
        // Hook nhóm hàm DrawText trong User32.dll do plugin import
        ReplacePluginDrawTextFunction(plugin_info.plugin_module);
        //Lấy địa chỉ điểm vào của hàm
        pfTMPluginGetInstance TMPluginGetInstance = (pfTMPluginGetInstance)::GetProcAddress(plugin_info.plugin_module, "TMPluginGetInstance");
        if (TMPluginGetInstance == NULL)
        {
            plugin_info.state = PluginState::PS_FUNCTION_GET_FAILED;
            plugin_info.error_code = GetLastError();
            continue;
        }
        //Tạo đối tượng plugin
        plugin_info.plugin = TMPluginGetInstance();
        if (plugin_info.plugin == nullptr)
            continue;
        //Kiểm tra phiên bản plugin
        int version = plugin_info.plugin->GetAPIVersion();
        if (version <= PLUGIN_UNSUPPORT_VERSION)
        {
            plugin_info.state = PluginState::PS_VERSION_NOT_SUPPORT;
            continue;
        }

        //Gọi hàm khởi tạo
        if (version >= 7)
        {
            plugin_info.plugin->OnInitialize(&theApp);
        }

        //Truyền đường dẫn tệp cấu hình cho plugin
        std::wstring config_dir = theApp.m_config_dir;
        config_dir += L"plugins\\";
        if (version >= 2)
        {
            CreateDirectory(config_dir.c_str(), NULL);       //Nếu plugins chưa tồn tại thì tạo mới
            plugin_info.plugin->OnExtenedInfo(ITMPlugin::EI_CONFIG_DIR, config_dir.c_str());
        }

        //Lấy thông tin plugin
        for (int i{}; i < ITMPlugin::TMI_MAX; i++)
        {
            ITMPlugin::PluginInfoIndex index{ static_cast<ITMPlugin::PluginInfoIndex>(i) };
            plugin_info.properties[index] = WcharArrayToWString(plugin_info.plugin->GetInfo(index));
        }

        //Lấy mục hiển thị của plugin
        int index = 0;
        while (true)
        {
            IPluginItem* item = plugin_info.plugin->GetItem(index);
            if (item == nullptr)
                break;
            plugin_info.plugin_items.push_back(item);
            m_plugins.push_back(item);
            m_plguin_item_map[item] = plugin_info.plugin;
            index++;
        }
    }
    // Hook nhóm hàm DrawText trong User32.dll do MFC import
    ReplaceMfcDrawTextFunction();

    //Khởi tạo tất cả mục hiển thị trên thanh tác vụ
    for (const auto& display_item : AllDisplayItems)
    {
        m_all_display_items_with_plugins.insert(display_item);
    }
    for (const auto& display_item : m_plugins)
    {
        m_all_display_items_with_plugins.insert(display_item);
    }
}

const std::vector<IPluginItem*>& CPluginManager::GetPluginItems() const
{
    return m_plugins;
}

const std::vector<CPluginManager::PluginInfo>& CPluginManager::GetPlugins() const
{
    return m_modules;
}

IPluginItem* CPluginManager::GetItemById(const std::wstring& item_id)
{
    for (const auto& item : m_plugins)
    {
        if (item->GetItemId() == item_id)
            return item;
    }
    return nullptr;
}

IPluginItem* CPluginManager::GetItemByIndex(int index)
{
    if (index >= 0 && index < static_cast<int>(m_plugins.size()))
        return m_plugins[index];
    return nullptr;
}

int CPluginManager::GetItemIndex(IPluginItem* item) const
{
    for (auto iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
    {
        if (*iter == item)
            return static_cast<int>(iter - m_plugins.begin());
    }
    return -1;
}

ITMPlugin* CPluginManager::GetPluginByItem(IPluginItem* pItem)
{
    if (pItem == nullptr)
        return nullptr;
    return m_plguin_item_map[pItem];
}

int CPluginManager::GetPluginIndex(ITMPlugin* plugin) const
{
    for (auto iter = m_modules.begin(); iter != m_modules.end(); ++iter)
    {
        if (iter->plugin == plugin)
            return static_cast<int>(iter - m_modules.begin());
    }
    return -1;
}

void CPluginManager::EnumPlugin(std::function<void(ITMPlugin*)> func) const
{
    for (const auto& item : m_modules)
    {
        if (item.plugin != nullptr)
        {
            func(item.plugin);
        }
    }
}

void CPluginManager::EnumPluginItem(std::function<void(IPluginItem*)> func) const
{
    for (const auto& item : m_plugins)
    {
        if (item != nullptr)
        {
            func(item);
        }
    }
}

const std::set<CommonDisplayItem>& CPluginManager::AllDisplayItemsWithPlugins()
{
    return m_all_display_items_with_plugins;
}


int CPluginManager::GetItemWidth(IPluginItem* pItem, CDC* pDC)
{
    int width = 0;
    ITMPlugin* plugin = GetPluginByItem(pItem);
    if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
    {
        width = pItem->GetItemWidthEx(pDC->GetSafeHdc());       //Ưu tiên dùng giao diện GetItemWidthEx để lấy chiều rộng
    }
    if (width == 0)
    {
        width = theApp.DPI(pItem->GetItemWidth());
    }
    return width;
}

std::wstring CPluginManager::PluginInfo::Property(ITMPlugin::PluginInfoIndex index) const
{
    auto iter = properties.find(index);
    if (iter != properties.end())
        return iter->second;
    return wstring();
}

template <class T>
void inline CopyData(void* from, T* to)
{
    ::memcpy(to, from, sizeof(T));
}

auto inline GetCLocale() noexcept
    -> const std::locale&
{
    const static std::locale restlt{"C"};
    return restlt;
};

/**
 * @brief Chuyển tên dll đầu vào thành chữ hoa
 *
 * @param dll_name Tên dll đầu vào
 * @param expected_length Độ dài tên dll mong đợi, không khớp thì không làm gì
 * @return std::string Tên dll viết hoa
 */
std::string UpperDllName(const char* dll_name, std::size_t expected_length)
{
    std::string result{};
    result.resize(expected_length);
    const char* dll_name_last = &dll_name[expected_length];
    std::transform(dll_name, dll_name_last, result.begin(),
                   [](char c) -> char
                   { return std::toupper(c, GetCLocale()); });
    return result;
}

std::string UpperDllName(const char* dll_name)
{
    std::string result{};
    for (std::size_t i = 0; i < 256; ++i)
    {
        auto current_char = dll_name[i];
        if (current_char == '\0')
        {
            result += '\0';
            break;
        }
        result += std::toupper(current_char, GetCLocale());
    }
    return result;
}

class DllName
{
public:
    class FunctionNameImpl
    {
        using Handler = std::function<void(void**, void*)>;
        friend DllName;

    public:
        /**
         * @brief Construct a new Function Name Impl object
         *
         * @param function_name Tên hàm
         * @param on_function_find_handler Phương thức thực thi khi tìm thấy hàm,
         Kiểu là std::function<void(void** pp_found_function_address, void* p_args)>
         */
        FunctionNameImpl(const std::string& function_name, Handler on_function_find_handler)
            : m_function_name{function_name}, m_on_function_find_handler{on_function_find_handler}, m_p_function{}
        {
        }
        ~FunctionNameImpl() = default;

        void OnFunctionFind(void** pp_found_fuction, void* p_args) const
        {
            m_on_function_find_handler(pp_found_fuction, p_args);
        }

        auto NameString() const noexcept
            -> const std::string&
        {
            return m_function_name;
        }

    private:
        void* m_p_function;
        Handler m_on_function_find_handler;
        std::string m_function_name;
    };
    using FunctionName = FunctionNameImpl;

private:
    std::unordered_map<void*, FunctionName> m_function_name_map{};
    std::string m_dll_name;

public:
    DllName(const char* dll_name, std::size_t expected_length, std::initializer_list<FunctionName> function_names_in_dll)
        : m_dll_name{UpperDllName(dll_name, expected_length)}
    {
        auto h_dll = ::LoadLibraryA(dll_name);
        if (h_dll)
        {
            for (auto function_name : function_names_in_dll)
            {
                auto* function_pointer =
                    reinterpret_cast<void*>(::GetProcAddress(h_dll, function_name.NameString().c_str()));

                auto exist_it = m_function_name_map.find(function_pointer);
                if (exist_it == m_function_name_map.end())
                {
                    m_function_name_map.emplace(std::move(std::make_pair(function_pointer, function_name)));
                }
            }
            ::FreeLibrary(h_dll);
        }
    }
    ~DllName() = default;

    auto NameString() const noexcept
        -> const std::string&
    {
        return m_dll_name;
    }

    auto MatchFunctionPointer(void* found_pointer) const noexcept
        -> const FunctionName*
    {
        auto it = m_function_name_map.find(found_pointer);
        if (it != m_function_name_map.end())
        {
            return &it->second;
        }
        return nullptr;
    }
};

using FunctionName = DllName::FunctionNameImpl;

class DllMap
{
private:
    std::unordered_map<std::string, DllName> m_dll_name_map;

public:
    DllMap(std::initializer_list<DllName> dll_names)
    {
        for (auto&& dll_name : dll_names)
        {
            auto exist_dll_name = m_dll_name_map.find(dll_name.NameString());
            if (exist_dll_name == m_dll_name_map.end())
            {
                m_dll_name_map.emplace(std::make_pair(dll_name.NameString(), dll_name));
            }
        }
    }
    ~DllMap() = default;

    auto MatchDllName(const char* any_case_string) const
        -> const DllName*
    {
        std::string capital_dll_name = UpperDllName(any_case_string);
        auto it = m_dll_name_map.find(capital_dll_name);
        if (it != m_dll_name_map.end())
        {
            return &it->second;
        }
        return nullptr;
    }
};

#define ARR_ARRLENGTH(c_array) c_array, GetArrayLength(c_array)

void CPluginManager::ReplacePluginDrawTextFunction(HMODULE plgin_dll_module) noexcept
{
    auto* p_plugin_dll_module = reinterpret_cast<BYTE*>(plgin_dll_module);

    auto p_dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(plgin_dll_module);

    auto p_image_nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(
        p_plugin_dll_module + p_dos_header->e_lfanew);

    auto p_optional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(
        &p_image_nt_headers->OptionalHeader);

    auto import_table_offset =
        p_optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    auto p_current_import_desc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
        p_plugin_dll_module + import_table_offset);

    //Không phân biệt hoa thường, cần nhập 4 hàm của User32.dll trong bảng import của dll
    static const char user32_dll_name[] = "User32.dll";
    static const DllName dll_names{ARR_ARRLENGTH(user32_dll_name),
                                   {{"DrawTextA", User32DrawTextManager::A::GetReplaceOperation()},
                                    {"DrawTextW", User32DrawTextManager::W::GetReplaceOperation()},
                                    {"DrawTextExA", User32DrawTextManager::ExA::GetReplaceOperation()},
                                    {"DrawTextExW", User32DrawTextManager::ExW::GetReplaceOperation()}}};
    static const DllMap dll_map{{dll_names}};

    while (p_current_import_desc->FirstThunk != NULL)
    {
        const auto* p_import_dll_name = reinterpret_cast<char*>(
            p_plugin_dll_module + p_current_import_desc->Name);

        auto* p_matched_dll_name = dll_map.MatchDllName(p_import_dll_name);
        if (p_matched_dll_name != nullptr)
        {
            //Tìm trong bảng IAT
            auto p_current_chunk = reinterpret_cast<PIMAGE_THUNK_DATA>(
                p_plugin_dll_module + p_current_import_desc->FirstThunk);

            while (p_current_chunk->u1.AddressOfData != NULL)
            {
                static_assert(sizeof(p_current_chunk->u1.Function) == sizeof(void*), "Length not match!");
                auto p_target_pointer = reinterpret_cast<void**>(&p_current_chunk->u1.Function);

                auto p_matched_function =
                    p_matched_dll_name->MatchFunctionPointer(*p_target_pointer);
                if (p_matched_function != nullptr)
                {
                    p_matched_function->OnFunctionFind(p_target_pointer, nullptr);
                }
                ++p_current_chunk;
            }
        }
        ++p_current_import_desc;
    }
}

void CPluginManager::ReplaceMfcDrawTextFunction() noexcept
{
    HMODULE mfc_module = ::LoadLibraryA("mfc140u.dll");
    if (mfc_module != NULL)
    {
        ReplacePluginDrawTextFunction(mfc_module);
        ::FreeLibrary(mfc_module);
    }
}
