#pragma once
#include "DisplayItem.h"

class CTaskbarItemOrderHelper
{
public:
    CTaskbarItemOrderHelper(bool displayed_only = false);

    void Init();

    //Lấy từng mục hiển thị của cửa sổ thanh tác vụ theo thứ tự đã thiết lập
    std::vector<CommonDisplayItem> GetAllDisplayItemsWithOrder() const;

    void FromString(const std::wstring& str);
    std::wstring ToString() const;
    void SetOrder(const vector<int>& item_order);
    const vector<int>& GetItemOrderConst() const;
    vector<int>& GetItemOrder();

    static CString GetItemDisplayName(CommonDisplayItem item);
    static bool IsItemDisplayed(CommonDisplayItem item);

private:
    //Chuẩn hóa các mục trong m_item_order: nếu có số thứ tự vượt quá số mục hiển thị thì xóa đi và thêm số thứ tự của các mục còn thiếu vào phía sau
    void NormalizeItemOrder();

private:
    vector<int> m_item_order;   //Lưu thứ tự của từng mục
    vector<CommonDisplayItem> m_all_item_in_default_order;    //Tất cả mục hiển thị được lưu theo thứ tự mặc định
    bool m_displayed_only;
};
