#pragma once
#include <vector>
class CTaskbarHelper
{
public:
    //Lấy thanh tác vụ trên tất cả màn hình phụ và đảm bảo theo thứ tự màn hình
    static void GetAllSecondaryDisplayTaskbar(std::vector<HWND>& secondary_taskbars);

    //Lấy số lượng màn hình
    static int GetDisplayNum();

    //Lấy số lượng thanh tác vụ trên màn hình phụ
    static int GetSecondaryTaskbarNum();
};

