#pragma once
#include "TaskBarDlg.h"
class CWin11TaskbarDlg :
    public CTaskBarDlg
{
public:

private:
    // Kế thừa qua CTaskBarDlg
    void InitTaskbarWnd() override;
    virtual void AdjustTaskbarWndPos(bool force_adjust) override;
    void ResetTaskbarPos() override;
    virtual HWND GetParentHwnd() override;

    HWND m_hNotify;     //Handle của vùng thông báo trên thanh tác vụ
    HWND m_hStart;      //Handle của nút Start
    CRect m_rcNotify;   //Vùng hình chữ nhật của vùng thông báo trên thanh tác vụ
    CRect m_rcStart;     //Vùng hình chữ nhật của nút Start
    int m_last_notify_width{};
    int m_last_start_pos{};

    // Kế thừa qua CTaskBarDlg
    void CheckTaskbarOnTopOrBottom() override;

};

