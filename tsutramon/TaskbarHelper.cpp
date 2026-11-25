#include "stdafx.h"
#include "TaskbarHelper.h"
#include <algorithm>

// Lưu thông tin màn hình
struct MonitorInfo
{
    HMONITOR hMonitor;
    CRect rect;
};

// Lưu thông tin thanh tác vụ
struct TaskbarInfo
{
    HWND hwnd;
    CRect rect;
};

std::vector<MonitorInfo> monitors;
std::vector<TaskbarInfo> taskbars;

// Hàm callback liệt kê màn hình
static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MonitorInfo info;
    info.hMonitor = hMonitor;
    info.rect = *lprcMonitor;
    if (!info.rect.IsRectEmpty())
        monitors.push_back(info);
    else
        ASSERT(FALSE);
    return TRUE;
}

// Hàm callback liệt kê cửa sổ
static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    TCHAR className[256];
    GetClassName(hwnd, className, 256);

    // Kiểm tra có phải thanh tác vụ của màn hình phụ không
    if (_tcscmp(className, _T("Shell_SecondaryTrayWnd")) == 0)
    {
        TaskbarInfo info;
        info.hwnd = hwnd;
        GetWindowRect(hwnd, &info.rect);
        taskbars.push_back(info);
    }

    return TRUE;
}

// Hàm so sánh: sắp xếp theo tọa độ góc trên bên trái của màn hình
static bool CompareTaskbarByMonitorOrder(const TaskbarInfo& a, const TaskbarInfo& b)
{
    // Duyệt tất cả màn hình
    for (const auto& monitor : monitors)
    {
        // Kiểm tra thanh tác vụ a có nằm trong màn hình hiện tại không
        bool aInMonitor = (a.rect.left >= monitor.rect.left && a.rect.top >= monitor.rect.top);
        // Kiểm tra thanh tác vụ b có nằm trong màn hình hiện tại không
        bool bInMonitor = (b.rect.left >= monitor.rect.left && b.rect.top >= monitor.rect.top);

        // Nếu a nằm trong màn hình hiện tại còn b thì không, a phải đứng trước b
        if (aInMonitor && !bInMonitor)
            return true;
        // Nếu b nằm trong màn hình hiện tại còn a thì không, b phải đứng trước a
        if (bInMonitor && !aInMonitor)
            return false;
        // Nếu cả a và b đều nằm trong màn hình hiện tại thì sắp xếp theo vị trí thanh tác vụ
        if (aInMonitor && bInMonitor)
        {
            if (a.rect.left != b.rect.left)
                return a.rect.left < b.rect.left;
            return a.rect.top < b.rect.top;
        }
    }
    // Mặc định thì a và b bằng nhau
    return false;
}

void CTaskbarHelper::GetAllSecondaryDisplayTaskbar(std::vector<HWND>& secondary_taskbars)
{
    monitors.clear();
    taskbars.clear();
    secondary_taskbars.clear();

    // Lấy thông tin tất cả màn hình
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

    // Lấy handle của tất cả thanh tác vụ
    EnumWindows(EnumWindowsProc, 0);

    // Sắp xếp thanh tác vụ theo thứ tự màn hình
    std::sort(taskbars.begin(), taskbars.end(), CompareTaskbarByMonitorOrder);

    //Lưu handle thanh tác vụ
    for (const auto& taskbar : taskbars)
    {
        secondary_taskbars.push_back(taskbar.hwnd);
    }
}

int CTaskbarHelper::GetDisplayNum()
{
    monitors.clear();
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
    return static_cast<int>(monitors.size());
}

int CTaskbarHelper::GetSecondaryTaskbarNum()
{
    taskbars.clear();
    EnumWindows(EnumWindowsProc, 0);
    return static_cast<int>(taskbars.size());
}
