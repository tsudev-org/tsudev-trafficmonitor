#pragma once
#include <memory>
#include "OpenHardwareMonitorGlobal.h"
#include <map>
#include <string>

namespace OpenHardwareMonitorApi
{
    class IOpenHardwareMonitor
    {
    public:
        virtual void GetHardwareInfo() = 0;     //Lấy thông tin phần cứng một lần
        virtual float CpuTemperature() = 0;     //Trả về nhiệt độ CPU đã lấy được
        virtual float GpuTemperature() = 0;     //Trả về nhiệt độ GPU đã lấy được
        virtual float HDDTemperature() = 0;     //Trả về nhiệt độ ổ cứng đã lấy được
        virtual float MainboardTemperature() = 0;   //Trả về nhiệt độ bo mạch chủ đã lấy được
        virtual float GpuUsage() = 0;           //Trả về mức sử dụng GPU đã lấy được
        virtual float CpuFreq() = 0;            //Trả về tần số CPU đã lấy được
        virtual float CpuUsage() = 0;           //Trả về mức sử dụng CPU đã lấy được
        virtual const std::map<std::wstring, float>& AllHDDTemperature() = 0;   //Trả về nhiệt độ của tất cả ổ cứng. key của map là tên ổ cứng, value là nhiệt độ ổ cứng
        virtual const std::map<std::wstring, float>& AllCpuTemperature() = 0;   //Trả về nhiệt độ của tất cả CPU (lõi). key của map là tên CPU, value là nhiệt độ
        virtual const std::map<std::wstring, float>& AllHDDUsage() = 0;         //Trả về mức sử dụng của tất cả ổ cứng

        virtual void SetCpuEnable(bool enable) = 0;
        virtual void SetGpuEnable(bool enable) = 0;
        virtual void SetHddEnable(bool enable) = 0;
        virtual void SetMainboardEnable(bool enable) = 0;
    };

    OPENHARDWAREMONITOR_API std::shared_ptr<IOpenHardwareMonitor> CreateInstance();
    OPENHARDWAREMONITOR_API std::wstring GetErrorMessage();
}
