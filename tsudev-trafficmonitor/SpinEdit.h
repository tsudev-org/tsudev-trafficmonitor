#pragma once
// Đây là điều khiển Edit có nút tăng giảm, gắn thêm nút tăng giảm ở bên phải CEdit, hãy chừa chỗ
// CSpinEdit

//Thông điệp nhấn nút tăng giảm. WPARAM: con trỏ CSpinButtonCtrl, LPARAM: LPNMUPDOWN
#define WM_SPIN_EDIT_POS_CHANGED (WM_USER + 1010)

class CSpinEdit : public CEdit
{
    DECLARE_DYNAMIC(CSpinEdit)

public:
    CSpinEdit();
    virtual ~CSpinEdit();

    void SetRange(short lower, short upper, short step = 1);        // Thiết lập phạm vi giá trị trong ô văn bản
    void SetValue(int value);                       //Thiết lập giá trị trong ô văn bản
    int GetValue();                                 //Lấy giá trị trong ô văn bản
    void SetMouseWheelEnable(bool enable);          //Thiết lập có cho phép phản hồi con lăn chuột hay không

protected:
    CSpinButtonCtrl m_spin;                         //Điều khiển nút tăng giảm
    int m_step{ 1 };
    int m_spin_width{};
    bool m_mouse_wheel_enable{ true };

    DECLARE_MESSAGE_MAP()
    virtual void PreSubclassWindow();
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
public:
    void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnEnable(BOOL bEnable);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


