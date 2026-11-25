// ComboBox2.cpp: tệp triển khai
//

#include "stdafx.h"
#include "ComboBox2.h"


// CComboBox2

IMPLEMENT_DYNAMIC(CComboBox2, CComboBox)

CComboBox2::CComboBox2()
{

}

CComboBox2::~CComboBox2()
{
}


void CComboBox2::SetMouseWheelEnable(bool enable)
{
    m_mouse_wheel_enable = enable;
}

BEGIN_MESSAGE_MAP(CComboBox2, CComboBox)
END_MESSAGE_MAP()



// Trình xử lý thông điệp của CComboBox2




BOOL CComboBox2::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    //Nếu m_mouse_wheel_enable là false thì không phản hồi thông điệp con lăn chuột
    if (pMsg->message == WM_MOUSEWHEEL && !m_mouse_wheel_enable)
    {
        //Gửi thông điệp con lăn chuột tới cửa sổ cha
        CWnd* pParent = GetParent();
        pParent->SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
        return true;
    }

    return CComboBox::PreTranslateMessage(pMsg);
}
