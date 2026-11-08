#include "stdafx.h"
#include "GpuUsage.h"

///////////////////////////////////////////////////////////////////////////////////////////
// Triển khai CPdhGPUUsage
///////////////////////////////////////////////////////////////////////////////////////////
CPdhGPUUsage::CPdhGPUUsage()
: CPdhQuery(_T("\\GPU Engine(*)\\Utilization Percentage"))
{
}

CPdhGPUUsage::~CPdhGPUUsage()
{
}

bool CPdhGPUUsage::GetGpuUsage(int& usage)
{
    if (isInitialized)
    {
        std::vector<CounterValueItem> valueItems;
        if (QueryValues(valueItems))
        {
            if (!valueItems.empty())
            {
                //Lấy mức sử dụng của mọi loại
                std::map<std::wstring, double> gpu_usage_map;
                for (const auto& item : valueItems)
                {
                    std::wstring item_name = item.name;
                    size_t index = item.name.rfind(L'_');
                    if (index != std::wstring::npos)
                        item_name = item.name.substr(index + 1);
                    gpu_usage_map[item_name] += item.value;
                }
                //Lấy giá trị lớn nhất trong mọi loại làm mức sử dụng tổng (giống cách xử lý của Task Manager trên Windows)
                double max_value = 0;
                for (const auto& item : gpu_usage_map)
                {
                    if (item.second > max_value)
                        max_value = item.second;
                }

                usage = static_cast<int>(max_value + 0.5);  // Làm tròn
                usage = min(max(usage, 0), 100);        // Giới hạn trong khoảng 0-100
                return true;
            }
        }
    }

    return false;
}
