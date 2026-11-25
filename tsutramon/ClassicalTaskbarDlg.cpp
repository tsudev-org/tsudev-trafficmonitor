#include "stdafx.h"
#include "ClassicalTaskbarDlg.h"

void CClassicalTaskbarDlg::AdjustTaskbarWndPos(bool force_adjust)
{
    ::GetWindowRect(m_hMin, m_rcMin); //Lấy vùng của cửa sổ thu nhỏ
    ::GetWindowRect(m_hBar, m_rcBar); //Lấy vùng của container cấp hai

    if (m_taskbar_on_top_or_bottom)     //Khi thanh tác vụ nằm ở trên cùng hoặc dưới cùng màn hình
    {
        //Thiết lập kích thước cửa sổ
        m_rect.right = m_rect.left + m_window_width;
        m_rect.bottom = m_rect.top + m_window_height;

        if (force_adjust || m_rcMin.Width() != m_last_width)   //Nếu chiều rộng thay đổi thì đặt lại vị trí cửa sổ thanh tác vụ
        {
            m_rcMinOri = m_rcMin;
            m_left_space = m_rcMin.left - m_rcBar.left;
            //Lưu chiều rộng lần trước
            m_last_width = m_rcMin.Width() - m_rect.Width();
            //Khi cửa sổ thanh tác vụ hiển thị ở bên phải
            if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            {
                ::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);    //Thiết lập vị trí của cửa sổ thu nhỏ
                m_rect.MoveToX(m_left_space + m_rcMin.Width() - m_rect.Width() + 2);
            }
            //Khi cửa sổ thanh tác vụ hiển thị ở bên trái
            else
            {
                ::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
                m_rect.MoveToX(m_left_space);
            }

            //Thiết lập vị trí theo chiều dọc của cửa sổ thanh tác vụ
            m_rect.MoveToY((m_rcBar.Height() - m_rect.Height()) / 2);
            if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())
                m_rect.MoveToY(m_rect.top + DPI(1));
            MoveWindow(m_rect);
        }
    }
    else        //Khi thanh tác vụ nằm ở bên trái hoặc bên phải màn hình
    {
        //Thiết lập kích thước cửa sổ
        if (force_adjust || m_rcMin.Height() != m_last_height)     //Nếu chiều cao thay đổi thì đặt lại vị trí cửa sổ thanh tác vụ
        {
            m_rcMinOri = m_rcMin;
            m_top_space = m_rcMin.top - m_rcBar.top;
            //Lưu chiều rộng lần trước
            m_last_height = m_rcMin.Height() - m_rect.Height();

            if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            {
                ::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);    //Thiết lập vị trí của cửa sổ thu nhỏ
                m_rect.MoveToY(m_top_space + m_rcMin.Height() - m_rect.Height() + 2);
            }
            else
            {
                ::MoveWindow(m_hMin, 0, m_top_space + m_rect.Height(), m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);  //Thiết lập vị trí của cửa sổ thu nhỏ
                m_rect.MoveToY(m_top_space);
            }
            m_rect.MoveToX((m_rcMin.Width() - m_window_width) / 2);
            int left_space = DPI(2);
            if (m_rect.left < left_space)
                m_rect.MoveToX(left_space);
            MoveWindow(m_rect);
        }
    }
}

void CClassicalTaskbarDlg::InitTaskbarWnd()
{
    m_hBar = ::FindWindowEx(m_hTaskbar, 0, L"ReBarWindow32", NULL); //Tìm handle của container cấp hai
    if (m_hBar == NULL)
        m_hBar = ::FindWindowEx(m_hTaskbar, nullptr, L"WorkerW", NULL);
    m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskSwWClass", NULL);    //Tìm handle của cửa sổ thu nhỏ
    if (m_hMin == NULL)
        m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskListWClass", NULL);    //Tìm handle của cửa sổ thu nhỏ

    ::GetWindowRect(m_hMin, m_rcMin);   //Lấy vùng của cửa sổ thu nhỏ
    ::GetWindowRect(m_hBar, m_rcBar);   //Lấy vùng của container cấp hai

    m_left_space = m_rcMin.left - m_rcBar.left;
    m_top_space = m_rcMin.top - m_rcBar.top;
}

void CClassicalTaskbarDlg::ResetTaskbarPos()
{
    //Khi đóng chương trình thì khôi phục lại width của cửa sổ thu nhỏ
    if (!m_rcMinOri.IsRectEmpty())
    {
        if (m_taskbar_on_top_or_bottom)
            ::MoveWindow(m_hMin, m_left_space, 0, m_rcMinOri.Width(), m_rcMinOri.Height(), TRUE);
        else

            ::MoveWindow(m_hMin, 0, m_top_space, m_rcMinOri.Width(), m_rcMinOri.Height(), TRUE);
    }
}

HWND CClassicalTaskbarDlg::GetParentHwnd()
{
    return m_hBar;
}

void CClassicalTaskbarDlg::CheckTaskbarOnTopOrBottom()
{
    CRect rect;
    CRect rcMin;
    CRect rcBar;
    if (m_hTaskbar != 0)
    {
        //::GetWindowRect(m_hMin, rcMin); //Lấy vùng của cửa sổ thu nhỏ
        //::GetWindowRect(m_hBar, rcBar); //Lấy vùng của container cấp hai
        //if (m_left_space == 0)
        //    m_left_space = rcMin.left - rcBar.left;
        //if (m_top_space == 0)
        //    m_top_space = rcMin.top - rcBar.top;

        ::GetWindowRect(m_hTaskbar, rect);          //Lấy vùng hình chữ nhật của thanh tác vụ
        m_taskbar_on_top_or_bottom = (rect.Width() >= rect.Height());     //Nếu chiều rộng thanh tác vụ lớn hơn chiều cao thì thanh tác vụ nằm ở trên cùng hoặc dưới cùng màn hình
    }
    else
    {
        m_taskbar_on_top_or_bottom = true;
    }
}

