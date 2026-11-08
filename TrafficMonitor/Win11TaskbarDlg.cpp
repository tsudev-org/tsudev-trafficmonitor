#include "stdafx.h"
#include "Win11TaskbarDlg.h"
#include "WindowsSettingHelper.h"

void CWin11TaskbarDlg::AdjustTaskbarWndPos(bool force_adjust)
{
    ::GetWindowRect(m_hNotify, m_rcNotify);
    ::GetWindowRect(m_hStart, m_rcStart);
    m_rcStart.MoveToXY(m_rcStart.left - m_rcTaskbar.left, m_rcStart.top - m_rcTaskbar.top);

    //Thiết lập kích thước cửa sổ
    m_rect.right = m_rect.left + m_window_width;
    m_rect.bottom = m_rect.top + m_window_height;
    if (force_adjust || m_rcNotify.Width() != m_last_notify_width || m_rcStart.left != m_last_start_pos)   //Nếu chiều rộng cửa sổ thu nhỏ thay đổi thì đặt lại vị trí cửa sổ thanh tác vụ
    {
        m_last_notify_width = m_rcNotify.Width();
        m_last_start_pos = m_rcStart.left;
        //Khi cửa sổ thanh tác vụ hiển thị ở bên phải, hoặc khi thanh tác vụ căn trái trên Windows 11
        //(Trên Windows 11, nếu thanh tác vụ căn trái thì dù trong “Thiết lập cửa sổ thanh tác vụ” đã chọn hiển thị bên trái, cửa sổ vẫn hiển thị bên phải)
        if (!theApp.m_taskbar_data.tbar_wnd_on_left || !CWindowsSettingHelper::IsTaskbarCenterAlign())
        {
            ////Trường hợp nằm gần biểu tượng trên thanh tác vụ
            //if (theApp.m_taskbar_data.tbar_wnd_snap && IsTaskbarCloseToIconEnable(theApp.m_taskbar_data.tbar_wnd_on_left))
            //{
            //    m_rect.MoveToX(m_rcMin.right + 2);
            //}
            ////Trường hợp nằm gần vùng thông báo
            //else
            //{
            //Vị trí ngang của cửa sổ vùng thông báo
            int notify_x_pos = m_rcNotify.left;
            //Trường hợp không lấy được vị trí vùng thông báo
            if (notify_x_pos == 0)
            {
                //Màn hình phụ trên Win11 không có cửa sổ vùng thông báo, ở đây dùng giá trị cố định (vùng đồng hồ hệ thống rộng 88 pixel)
                if (m_is_secondary_display)
                    notify_x_pos = m_rcTaskbar.Width() - DPI(88);
                //Nếu không phải màn hình phụ nhưng vẫn không lấy được vị trí vùng thông báo thì dùng giá trị taskbar_right_space_win11 trong tệp cấu hình
                else
                    notify_x_pos = m_rcTaskbar.Width() - DPI(theApp.m_taskbar_data.taskbar_right_space_win11);
            }
            //Nếu đang hiển thị tiện ích (widget) và thanh tác vụ căn trái thì chừa chỗ cho tiện ích
            if (theApp.m_taskbar_data.avoid_overlap_with_widgets && CWindowsSettingHelper::IsTaskbarWidgetsBtnShown() && !CWindowsSettingHelper::IsTaskbarCenterAlign())
                m_rect.MoveToX(notify_x_pos - m_rect.Width() + 2 - DPI(theApp.m_taskbar_data.taskbar_left_space_win11));
            else
                m_rect.MoveToX(notify_x_pos - m_rect.Width() + 2);
            //}
        }
        //Khi cửa sổ thanh tác vụ hiển thị ở bên trái
        else
        {
            //Gần nút “Start”
            if (theApp.m_taskbar_data.tbar_wnd_snap)
            {
                m_rect.MoveToX(m_rcStart.left - m_rect.Width() - 2);
            }
            //Sát bên trái nhất
            else
            {
                if (CWindowsSettingHelper::IsTaskbarWidgetsBtnShown())
                    m_rect.MoveToX(2 + DPI(theApp.m_taskbar_data.taskbar_left_space_win11));
                else
                    m_rect.MoveToX(2);
            }
        }
        //Độ lệch ngang
        m_rect.MoveToX(m_rect.left + DPI(theApp.m_taskbar_data.window_offset_left));
        ////Đảm bảo theo chiều ngang không vượt ra ngoài biên màn hình
        //if (m_rect.left < 0)
        //    m_rect.MoveToX(0);
        //if (m_rcTaskbar.Width() > m_rect.Width() && m_rect.right > m_rcTaskbar.Width())
        //    m_rect.MoveToX(m_rcTaskbar.Width() - m_rect.Width());

        //Thiết lập vị trí theo chiều dọc của cửa sổ thanh tác vụ
        //Ghi chú: ở đây cộng thêm (m_rcTaskbar.Height() - rcStart.Height()) để sửa lỗi vị trí cửa sổ thanh tác vụ không đúng trên thiết bị cảm ứng từ Windows 11 build 22621 trở đi.
        //Trong trường hợp này chiều cao của m_rcTaskbar sẽ lớn hơn chiều cao của m_rcBar; bình thường thì hai chiều cao này bằng nhau
        //Nhưng khi thanh tác vụ không có biểu tượng nào, chiều cao của m_rcBar sẽ thành 0, nên dùng rcStart thay thế
        m_rect.MoveToY((m_rcStart.Height() - m_rect.Height()) / 2 + (m_rcTaskbar.Height() - m_rcStart.Height()) + DPI(theApp.m_taskbar_data.window_offset_top));
        ////Đảm bảo theo chiều dọc không vượt ra ngoài biên màn hình
        //if (m_rect.top < 0)
        //    m_rect.MoveToY(0);
        //if (m_rcTaskbar.Height() > m_rect.Height() && m_rect.bottom > m_rcTaskbar.Height())
        //    m_rect.MoveToY(m_rcTaskbar.Height() - m_rect.Height());

        MoveWindow(m_rect);
    }
}

void CWin11TaskbarDlg::InitTaskbarWnd()
{
    m_hNotify = ::FindWindowEx(m_hTaskbar, 0, L"TrayNotifyWnd", NULL);
    m_hStart = ::FindWindowEx(m_hTaskbar, nullptr, L"Start", NULL);
    ::GetWindowRect(m_hNotify, m_rcNotify);
}

void CWin11TaskbarDlg::ResetTaskbarPos()
{
}

HWND CWin11TaskbarDlg::GetParentHwnd()
{
    return m_hTaskbar;
}

void CWin11TaskbarDlg::CheckTaskbarOnTopOrBottom()
{
    m_taskbar_on_top_or_bottom = true;
}
