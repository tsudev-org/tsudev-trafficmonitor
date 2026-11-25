#pragma once
#include "TaskBarDlg.h"
class CClassicalTaskbarDlg :
    public CTaskBarDlg
{
public:

private:
    // Kế thừa qua CTaskBarDlg
    virtual void AdjustTaskbarWndPos(bool force_adjust) override;
    void InitTaskbarWnd() override;
    void ResetTaskbarPos() override;
    virtual HWND GetParentHwnd() override;

private:
    CRect m_rcMinOri;   //Vùng hình chữ nhật của cửa sổ thu nhỏ ở trạng thái ban đầu
    int m_left_space{};			//Lề trái giữa cửa sổ thu nhỏ và cửa sổ cấp hai
    int m_top_space{};			//Lề trên giữa cửa sổ thu nhỏ và cửa sổ cấp hai (dùng khi thanh tác vụ ở bên trái hoặc bên phải màn hình)
    HWND m_hBar;		//Handle của container cấp hai trên thanh tác vụ
    HWND m_hMin;		//Handle của cửa sổ thu nhỏ
    CRect m_rcBar;		//Vùng hình chữ nhật của cửa sổ thanh tác vụ ở trạng thái ban đầu
    CRect m_rcMin;		//Vùng hình chữ nhật của cửa sổ thu nhỏ

    int m_last_width;	//Chiều rộng lần trước, dùng để phát hiện thay đổi chiều rộng
    int m_last_height;	//Chiều cao lần trước, dùng để phát hiện thay đổi chiều cao (dùng khi thanh tác vụ ở bên trái hoặc bên phải màn hình)

    // Kế thừa qua CTaskBarDlg
    void CheckTaskbarOnTopOrBottom() override;
};

