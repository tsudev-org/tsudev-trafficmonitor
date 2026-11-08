#pragma once
#include "PdhQuery.h"

class CPdhDiskUsage : public CPdhQuery
{
public:
    CPdhDiskUsage();
    ~CPdhDiskUsage();

    // Lấy mức sử dụng của ổ đĩa vật lý chỉ định (0-100)
    // diskIndex: 0=ổ vật lý thứ nhất, 1=ổ thứ hai...
    // Giá trị trả về: true=thành công, false=thất bại (lần gọi đầu thường thất bại, cần đợi trên 200ms rồi thử lại)
    bool GetDiskUsage(int diskIndex, /*out*/ int& usage);

    // Lấy danh sách tên ổ đĩa vật lý
    const std::vector<CString>& GetDiskNames() const { return m_diskNames; }

    //Tìm số thứ tự của một ổ cứng theo tên
    int FindDiskIndex(const std::wstring diskName);

    // Kiểm tra giám sát ổ đĩa có khả dụng không
    bool IsAvailable() const { return m_isAvailable; }

private:
    // Trích tên ổ đĩa từ instance của bộ đếm
    void ExtractDiskNames();

    // Xử lý thời gian rảnh
    int CalculateUtilization(double idleTime) const;

private:
    bool m_isAvailable{ false };
    std::vector<CString> m_diskNames;  // Danh sách tên ổ đĩa, theo thứ tự PDH trả về
};