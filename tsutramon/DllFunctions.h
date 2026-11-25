#pragma once
#include <shellscalingapi.h> // Chứa ::GetDpiForMonitor
#include <d3dcompiler.h> // Chứa D3DCompile
#include <dcomp.h> // Chứa DCompositionCreateDevice
#include <dxgi1_3.h> // Chứa CreateDXGIFactory2
#include <tchar.h>
#include <utility>

template <class FunctionPointer>
class CDllFunction
#ifdef WIN32
{
    // workaround for MSVC 19.34.31933 target x86

private:
    FunctionPointer m_p_function{nullptr};
    HMODULE m_h_dll;

public:
    CDllFunction(LPCTSTR dll_name, LPCSTR function_name) noexcept
    {
        m_h_dll = ::LoadLibrary(dll_name);
        if (m_h_dll != NULL)
        {
            m_p_function = (FunctionPointer)::GetProcAddress(m_h_dll, function_name);
            if (m_p_function == nullptr)
            {
                ::FreeLibrary(m_h_dll);
                m_h_dll = NULL;
            }
        }
    }
    ~CDllFunction() noexcept
    {
        if (m_h_dll != NULL)
        {
            ::FreeLibrary(m_h_dll);
            m_h_dll = NULL;
        }
    }
    template <class... Args>
    auto operator()(Args&&... args) const
    {
        return m_p_function(std::forward<Args>(args)...);
    }
    bool HasValue() const noexcept
    {
        return m_p_function != nullptr;
    }
}
#endif
;
/**
 * @brief Lớp tự động quản lý vòng đời HMODULE và tự động thử nạp hàm chỉ định
 Ví dụ sử dụng:
    Định nghĩa: CDllFunction<decltype(&::GetDpiForMonitor)> GetDpiForMonitor{_T("Shcore.dll"), "GetDpiForMonitor"};
    Sử dụng: (một lớp hoặc namespace nào đó)::GetDpiForMonitor(các tham số cần thiết);
    Tất nhiên, biến GetDpiForMonitor không được để lộ ra namespace toàn cục.
 *
 * @tparam R Giá trị trả về của hàm
 * @tparam Args Tham số của hàm
 */
template <class R, class... Args>
class CDllFunction<R (*)(Args...)>
{
    using FunctionPointer = R (*)(Args...);

private:
    FunctionPointer m_p_function{nullptr};
    HMODULE m_h_dll;

public:
    CDllFunction(LPCTSTR dll_name, LPCSTR function_name) noexcept
    {
        m_h_dll = ::LoadLibrary(dll_name);
        if (m_h_dll != NULL)
        {
            m_p_function = (FunctionPointer)::GetProcAddress(m_h_dll, function_name);
            if (m_p_function == nullptr)
            {
                ::FreeLibrary(m_h_dll);
                m_h_dll = NULL;
            }
        }
    }
    ~CDllFunction() noexcept
    {
        if (m_h_dll != NULL)
        {
            ::FreeLibrary(m_h_dll);
            m_h_dll = NULL;
        }
    }
    R operator()(Args... args) const
    {
        return m_p_function(args...);
    }
    bool HasValue() const noexcept
    {
        return m_p_function != nullptr;
    }
};

typedef HRESULT(WINAPI* _GetDpiForMonitor)(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

class CDllFunctions
{
public:
    CDllFunctions();
    ~CDllFunctions();

public:
    HRESULT GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);
    static const CDllFunction<decltype(&::D3DCompile)> D3DCompile;
    static const CDllFunction<decltype(&::DCompositionCreateDevice)> DCompositionCreateDevice;
    static const CDllFunction<decltype(&::CreateDXGIFactory2)> CreateDXGIFactory2;

private:
    _GetDpiForMonitor m_getDpiForMonitor{};

private:
    HMODULE m_shcore_module{};
};
