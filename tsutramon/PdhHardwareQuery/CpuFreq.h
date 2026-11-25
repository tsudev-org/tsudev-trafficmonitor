#pragma once
#include <Pdh.h>
#include <PdhMsg.h>
#include "PdhQuery.h"

class CPdhCpuFreq : public CPdhQuery
{
public:
    CPdhCpuFreq();

    // Gọi hàm này để lấy tần số CPU.
    // Lưu ý: từ lần gọi thứ hai trở đi mới lấy được giá trị; khoảng cách giữa hai lần gọi không nên quá ngắn, tốt nhất trên 200 mili giây
    bool GetCpuFreq(float& freq);

private:
    float max_cpu_freq = 0;
};
