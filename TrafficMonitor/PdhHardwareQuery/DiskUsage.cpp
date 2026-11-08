#include "stdafx.h"
#include "DiskUsage.h"
#include <algorithm>
#include <cmath>

CPdhDiskUsage::CPdhDiskUsage()
    : CPdhQuery(_T("\\PhysicalDisk(*)\\% Idle Time"))
{
    m_isAvailable = Initialize();
    if (m_isAvailable)
    {
        // Khởi động trước: kích hoạt khởi tạo nội bộ của PDH (lần QueryValues đầu tiên sẽ điền danh sách instance)
        std::vector<CounterValueItem> dummy;
        QueryValues(dummy);
        ExtractDiskNames();
    }
}

CPdhDiskUsage::~CPdhDiskUsage()
{
}

void CPdhDiskUsage::ExtractDiskNames()
{
    m_diskNames.clear();
    std::vector<CounterValueItem> values;

    if (!QueryValues(values))
        return;

    for (const auto& item : values)
    {
        CString name(item.name.c_str());
        m_diskNames.push_back(name);
    }
}

int CPdhDiskUsage::CalculateUtilization(double idleTime) const
{
    // Xử lý quan trọng: thời gian rảnh của ổ đĩa nhiều hàng đợi như NVMe/RAID có thể >100%
    // Ví dụ: thời gian rảnh của ổ đĩa 4 hàng đợi=400% → rảnh thực tế=100% → mức sử dụng=0%
    if (idleTime > 100.0)
        idleTime = 100.0;

    // Đảm bảo nằm trong khoảng 0-100
    idleTime = (std::max)(0.0, (std::min)(100.0, idleTime));

    // Mức sử dụng = 100% - thời gian rảnh
    double utilization = 100.0 - idleTime;
    return static_cast<int>(utilization + 0.5); // Làm tròn
}

bool CPdhDiskUsage::GetDiskUsage(int diskIndex, int& usage)
{
    usage = 0;
    if (!m_isAvailable)
        return false;

    std::vector<CounterValueItem> values;
    if (!QueryValues(values) || values.empty())
        return false;

    if (diskIndex >= 0 && diskIndex < static_cast<int>(values.size()))
    {
        usage = CalculateUtilization(values[diskIndex].value);
        return true;
    }
    return false;

}

int CPdhDiskUsage::FindDiskIndex(const std::wstring diskName)
{
    int disk_index = -1;
    for (int i = 0; i < static_cast<int>(m_diskNames.size()); i++)
    {
        if (diskName == m_diskNames[i].GetString())
        {
            disk_index = i;
            break;
        }
    }
    return disk_index;
}
