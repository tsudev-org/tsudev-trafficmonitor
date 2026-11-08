#pragma once
#include "PdhQuery.h"

// Lớp giám sát mức sử dụng GPU
class CPdhGPUUsage : public CPdhQuery
{
public:
    CPdhGPUUsage();
    ~CPdhGPUUsage();

    // Lấy mức sử dụng GPU (0-100)
    // Giá trị trả về: true=thành công, false=thất bại
    // Lưu ý: lần gọi đầu thường trả về false (PDH cần khởi động trước), nên gọi lại sau trên 200ms
    bool GetGpuUsage(/*out*/ int& usage);

};
