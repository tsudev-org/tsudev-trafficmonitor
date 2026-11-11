//Đây là lớp hộp thoại con dùng trong thẻ Tab
#pragma once
#include "BaseDialog.h"

class CTabDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CTabDlg)
public:
    CTabDlg(UINT nIDTemplate, CWnd *pParent = NULL);
    ~CTabDlg();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
    CWnd* GetParentWindow();
    void SetScrollbarInfo(int nPage, int nMax);
    virtual void OnTabEntered() {}      //Được gọi khi thẻ tab chuyển sang cửa sổ hiện tại
    virtual void OnOK();
    void SetScrollEnable(bool enable) { m_scroll_enable = enable; }
    virtual void SetControlMouseWheelEnable(bool enable) {}     //Ghi đè hàm này trong lớp dẫn xuất để thiết lập điều khiển có được phản hồi con lăn chuột hay không
    virtual void OnSettingsApplied() {}   //Được gọi sau khi nhấn nút “Áp dụng” và đã áp dụng xong thiết lập

    bool IsScrollBarVisible();    //Thanh cuộn có hiển thị hay không

private:
    void ScrollWindowSimple(int step, bool absolute = false);

protected:
    void EnableDlgCtrl(UINT id, bool enable);
    void ShowDlgCtrl(UINT id, bool show);

protected:
    CWnd* m_pParent;
    bool m_scroll_enable{ false };
    int m_last_pos{};

protected:
    // Phương thức ảo của CWnd, khai báo trong afxwin.h, triển khai trong wincore.cpp, được CWnd gọi khi xử lý thông điệp WM_SIZE
    // Bên trong, sau một số kiểm tra, dùng con trỏ CMFCDynamicLayout mà CWnd nắm giữ để thực hiện m_pDynamicLayout->Adjust()
    virtual void ResizeDynamicLayout() override final;  // Ghi đè ở đây để thay đổi thời điểm gọi, từ mặc định trước OnSize của cửa sổ thành sau OnSize của cửa sổ cha

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

