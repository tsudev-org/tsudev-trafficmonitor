// TaskBarDlg.cpp : tệp triển khai
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarDlg.h"
#include "afxdialogex.h"
#include "TrafficMonitorDlg.h"
#include "WindowsSettingHelper.h"
#include "WIC.h"
#include "Nullable.hpp"
#include "DrawCommonFactory.h"
#include "WindowsWebExperienceDetector.h"
#include "TaskbarHelper.h"

#ifdef DEBUG
// Bắt thông tin gỡ lỗi DX
#include "dxgi1_3.h"
#include "DXProgrammableCapture.h"
#endif

// Hộp thoại CTaskBarDlg

IMPLEMENT_DYNAMIC(CTaskBarDlg, CDialogEx)

CTaskBarDlg::CTaskBarDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_TASK_BAR_DIALOG, pParent)
{
}

CTaskBarDlg::~CTaskBarDlg()
{
    for (auto iter = m_map_history_data.begin(); iter != m_map_history_data.end(); ++iter)
    {
        iter->second.clear();
    }
}

void CTaskBarDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTaskBarDlg, CDialogEx)
    ON_WM_RBUTTONUP()
    ON_WM_INITMENU()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_WM_CLOSE()
    ON_WM_LBUTTONUP()
    ON_MESSAGE(WM_EXITMENULOOP, &CTaskBarDlg::OnExitmenuloop)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CTaskBarDlg::OnTabletQuerysystemgesturestatus)
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// Trình xử lý thông điệp của CTaskBarDlg

void CTaskBarDlg::ShowInfo(CDC* pDC)
{
    HWND current_hwnd = this->GetSafeHwnd();
    if (current_hwnd == NULL || !IsWindow(this->GetSafeHwnd()))
        return;
    if (m_rect.IsRectEmpty() || m_rect.IsRectNull())
        return;
    if (m_supported_render_enums.IsD2D1Enabled())
    {
        auto last_update_layered_window_error =
            theApp.m_taskbar_data.update_layered_window_error_code;
        if (last_update_layered_window_error)
        {
            m_supported_render_enums.DisableD2D1();
            if (!m_supported_render_enums.IsD2D1WithDCompositionEnabled())
            {
                m_supported_render_enums.EnableDefaultOnly();
                LogWin32ApiErrorMessage(last_update_layered_window_error);
                CString error_info{};
                error_info.Format(
                    _T("Call UpdateLayeredWindowIndirect failed. Use GDI render instead. Error code = %ld."),
                    last_update_layered_window_error);
                CCommon::WriteLog(error_info, theApp.m_log_path.c_str());
                // Tắt D2D
                theApp.m_taskbar_data.disable_d2d = true;
                // Hiển thị thông tin lỗi
                ::MessageBox(NULL, CCommon::LoadText(IDS_UPDATE_TASKBARDLG_FAILED_TIP), NULL, MB_OK | MB_ICONWARNING);
            }
        }
    }
    auto render_type = m_supported_render_enums.GetAutoFitEnum();
    CRect draw_rect{m_rect}; // Vùng hình chữ nhật để vẽ
    draw_rect.MoveToXY(0, 0);

#ifdef DEBUG
    Microsoft::WRL::ComPtr<IDXGraphicsAnalysis> p_dxgi_analysis{};
#endif
    // Khởi tạo bộ nhớ stack của DrawBuffer và DrawCommon
    AllInvolvedDrawCommonObjectsStorage all_involved_draw_common_objects{};
    IDrawCommon* draw_common_interface{nullptr};
    std::tie(std::ignore, draw_common_interface) =
        GetInterfaceFromAllInvolvedDrawCommonObjects(
            all_involved_draw_common_objects,
            render_type,
            {{DrawCommonHelper::RenderType::DEFAULT,
              [&](IDrawBuffer* p_draw_buffer_interface, IDrawCommon* p_draw_common_interface)
              {
                  auto p_draw_buffer = static_cast<CDrawDoubleBuffer*>(p_draw_buffer_interface);
                  auto p_draw_common = static_cast<CDrawCommon*>(p_draw_common_interface);

                  // Phải được khởi tạo trước đối tượng vẽ
                  EmplaceAt(p_draw_buffer, pDC, draw_rect);
                  // Khởi tạo đối tượng vẽ tại đây
                  EmplaceAt(p_draw_common);
                  p_draw_common->Create(p_draw_buffer->GetMemDC(), nullptr);
                  p_draw_common->FillRect(draw_rect, theApp.m_taskbar_data.back_color); // Tô màu nền
                  p_draw_common->SetFont(&m_font);
                  p_draw_common->SetBackColor(theApp.m_taskbar_data.back_color);
              }},
             {DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION,
              [&](IDrawBuffer* p_draw_buffer_interface, IDrawCommon* p_draw_common_interface)
              {
                  auto p_draw_buffer = static_cast<CTaskBarDlgDrawBufferUseDComposition*>(p_draw_buffer_interface);
                  auto p_draw_common = static_cast<CTaskBarDlgDrawCommon*>(p_draw_common_interface);

                  auto& ref_d2d1_app_support = theApp.m_d2d_taskbar_draw_common_support.Get();
                  m_d2d1_device_context_support.Get().SetWorkingDevice(
                      ref_d2d1_app_support.GetDCompositionDevice(),
                      ref_d2d1_app_support.GetD3D10Device1(),
                      m_hWnd);
                  // Ở đây ngược với phía trên: khởi tạo DrawCommon trước rồi mới khởi tạo Buffer
                  // Khởi tạo đối tượng vẽ tại đây
                  EmplaceAt(p_draw_common);
                  D2D1_SIZE_U d2d_size;
                  d2d_size.width = draw_rect.Width();
                  d2d_size.height = draw_rect.Height();
                  p_draw_common->Create(
                      this->m_taskbar_draw_common_window_support.Get(),
                      this->m_d2d1_device_context_support.Get(),
                      d2d_size);
                  // Chỉ bật bộ kết xuất này khi trong suốt và UpdateLayeredWindowIndirect thất bại, mặc định khởi tạo toàn màu đen, alpha=1
                  p_draw_common->FillRect(draw_rect, 0x00000000, 1);
                  p_draw_common->SetFont(&m_font);
                  p_draw_common->SetBackColor(theApp.m_taskbar_data.back_color);
                  // Khởi tạo buffer
                  EmplaceAt(p_draw_buffer, this->m_d2d1_device_context_support.Get());
              }},
             {DrawCommonHelper::RenderType::D2D1,
              [&](IDrawBuffer* p_draw_buffer_interface, IDrawCommon* p_draw_common_interface)
              {
                  auto p_draw_buffer = static_cast<CTaskBarDlgDrawBuffer*>(p_draw_buffer_interface);
                  auto p_draw_common = static_cast<CTaskBarDlgDrawCommon*>(p_draw_common_interface);

                  auto& ref_d2d1_app_support = theApp.m_d2d_taskbar_draw_common_support.Get();
                  m_d2d1_device_context_support.Get().SetWorkingDevice(ref_d2d1_app_support.GetD3D10Device1());
                  // Ở đây ngược với phía trên: khởi tạo DrawCommon trước rồi mới khởi tạo Buffer
                  // Khởi tạo đối tượng vẽ tại đây
                  EmplaceAt(p_draw_common);
                  D2D1_SIZE_U d2d_size;
                  d2d_size.width = draw_rect.Width();
                  d2d_size.height = draw_rect.Height();
                  p_draw_common->Create(
                      this->m_taskbar_draw_common_window_support.Get(),
                      this->m_d2d1_device_context_support.Get(),
                      d2d_size);
                  // Chỉ bật bộ kết xuất này khi trong suốt, mặc định khởi tạo toàn màu đen, alpha=1
                  p_draw_common->FillRect(draw_rect, 0x00000000, 1);
                  p_draw_common->SetFont(&m_font);
                  p_draw_common->SetBackColor(theApp.m_taskbar_data.back_color);
                  // Khởi tạo buffer
                  CSize draw_size{draw_rect.Width(), draw_rect.Height()};
                  EmplaceAt(p_draw_buffer,
                            this->m_taskbar_draw_common_window_support.Get(),
                            this->m_d2d1_device_context_support.Get(),
                            draw_size,
                            current_hwnd);

#ifdef DEBUG
                  DXGIGetDebugInterface1(0, IID_PPV_ARGS(&p_dxgi_analysis));
                  if (p_dxgi_analysis)
                  {
                      p_dxgi_analysis->BeginCapture();
                  }
#endif
              }}});
    IDrawCommon& draw = *draw_common_interface;
    // Vẽ các mục trên thanh tác vụ
    for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
    {
        CRect item_rect = m_item_rects[*iter];
        if (!item_rect.IsRectEmpty())
        {
            if (iter->IsPlugin())
                DrawPluginItem(draw, iter->PluginItem(), item_rect, iter->item_width.label_width, iter->is_double_line);
            else
                DrawDisplayItem(draw, iter->ItemType(), item_rect, iter->item_width.label_width, iter->is_double_line);
        }
    }

#ifdef DEBUG
    if (p_dxgi_analysis)
    {
        p_dxgi_analysis->EndCapture();
    }
#endif
}

void CTaskBarDlg::DrawDisplayItem(IDrawCommon& drawer, DisplayItem type, CRect rect, int label_width, bool vertical)
{
    //Thiết lập màu văn bản cần vẽ
    COLORREF label_color{};
    COLORREF text_color{};
    if (theApp.m_taskbar_data.specify_each_item_color)
    {
        label_color = theApp.m_taskbar_data.text_colors[type].label;
        text_color = theApp.m_taskbar_data.text_colors[type].value;
    }
    else if (!theApp.m_taskbar_data.text_colors.empty())
    {
        label_color = theApp.m_taskbar_data.text_colors.begin()->second.label;
        text_color = theApp.m_taskbar_data.text_colors.begin()->second.label;
    }

    //Thiết lập vùng hình chữ nhật của nhãn và giá trị
    CRect rect_label{ rect };
    CRect rect_value{ rect };
    if (!vertical)
    {
        rect_label.right = rect_label.left + label_width;
        rect_value.left = rect_label.right;
    }
    else if (label_width != 0)
    {
        rect_label.bottom = rect_label.top + (rect_label.Height() / 2);
        rect_value.top = rect_label.bottom;
    }

    // Vẽ thanh trạng thái
    if (type == TDI_CPU || type == TDI_MEMORY || type == TDI_GPU_USAGE || type == TDI_CPU_TEMP
        || type == TDI_GPU_TEMP || type == TDI_HDD_TEMP || type == TDI_MAIN_BOARD_TEMP || type == TDI_HDD_USAGE
        || type == TDI_UP || type == TDI_DOWN || type == TDI_TOTAL_SPEED/* ||type==TDI_CPU_FREQ*/)
    {
        int figure_value{};
        switch (type)
        {
        case TDI_CPU:
            figure_value = theApp.m_cpu_usage;
            break;
        case TDI_MEMORY:
            figure_value = theApp.m_memory_usage;
            break;
        case TDI_GPU_USAGE:
            figure_value = theApp.m_gpu_usage;
            break;
        case TDI_CPU_TEMP:
            figure_value = static_cast<int>(theApp.m_cpu_temperature);
            break;
        case TDI_GPU_TEMP:
            figure_value = static_cast<int>(theApp.m_gpu_temperature);
            break;
        case TDI_HDD_TEMP:
            figure_value = static_cast<int>(theApp.m_hdd_temperature);
            break;
        case TDI_MAIN_BOARD_TEMP:
            figure_value = static_cast<int>(theApp.m_main_board_temperature);
            break;
        case TDI_HDD_USAGE:
            figure_value = theApp.m_hdd_usage;
            break;
        //case TDI_CPU_FREQ:
        //    figure_value = theApp.m_cpu_freq;
        //    break;
        case TDI_UP:
            figure_value = CalculateNetspeedPercent(theApp.m_out_speed);
            break;
        case TDI_DOWN:
            figure_value = CalculateNetspeedPercent(theApp.m_in_speed);
            break;
        case TDI_TOTAL_SPEED:
            figure_value = CalculateNetspeedPercent(theApp.m_in_speed + theApp.m_out_speed);
            break;
        default:
            break;
        }

        if ((type != TDI_UP && type != TDI_DOWN && type != TDI_TOTAL_SPEED) && theApp.m_taskbar_data.show_status_bar
            || (type == TDI_UP || type == TDI_DOWN || type == TDI_TOTAL_SPEED) && theApp.m_taskbar_data.show_netspeed_figure)
        {
            if (theApp.m_taskbar_data.cm_graph_type)
            {
                AddHisToList(type, figure_value);
                TryDrawGraph(drawer, rect, type);
            }
            else
            {
                TryDrawStatusBar(drawer, rect, figure_value);
            }
        }
    }

    //Vẽ nhãn
    if (label_width > 0)
    {
        wstring str_label = theApp.m_taskbar_data.disp_str.GetConst(type);
        drawer.DrawWindowText(rect_label, str_label.c_str(), label_color, (vertical ? IDrawCommon::Alignment::CENTER : IDrawCommon::Alignment::LEFT));
    }

    //Vẽ giá trị
    IDrawCommon::Alignment value_alignment{ theApp.m_taskbar_data.value_right_align ? IDrawCommon::Alignment::RIGHT : IDrawCommon::Alignment::LEFT };      //Kiểu căn chỉnh của giá trị
    if (vertical)
        value_alignment = IDrawCommon::Alignment::CENTER;
    CString str_value = CommonDisplayItem(type).GetItemValueText(false);
    drawer.DrawWindowText(rect_value, str_value, text_color, value_alignment);
}

void CTaskBarDlg::DrawPluginItem(IDrawCommon& drawer, IPluginItem* item, CRect rect, int label_width, bool vertical)
{
    if (item == nullptr)
        return;

    //Vẽ biểu đồ sử dụng tài nguyên
    ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(item);
    if (theApp.m_taskbar_data.show_status_bar && plugin != nullptr && plugin->GetAPIVersion() >= 6)
    {
        if (item->IsDrawResourceUsageGraph())
        {
            int figure_value = static_cast<int>(item->GetResourceUsageGraphValue() * 100);
            //Biểu đồ cuộn ngang
            if (theApp.m_taskbar_data.cm_graph_type)
            {
                AddHisToList(item, figure_value);
                TryDrawGraph(drawer, rect, item);
            }
            //Biểu đồ cột
            else
            {
                TryDrawStatusBar(drawer, rect, figure_value);
            }
        }
    }

    //Thiết lập màu văn bản cần vẽ
    COLORREF label_text_color{};
    COLORREF value_text_color{};
    if (theApp.m_taskbar_data.specify_each_item_color)
    {
        label_text_color = theApp.m_taskbar_data.text_colors[item].label;
        value_text_color = theApp.m_taskbar_data.text_colors[item].value;
    }
    else if (!theApp.m_taskbar_data.text_colors.empty())
    {
        label_text_color = theApp.m_taskbar_data.text_colors.begin()->second.label;
        value_text_color = theApp.m_taskbar_data.text_colors.begin()->second.label;
    }

    if (plugin != nullptr && plugin->GetAPIVersion() >= 2)
    {
        plugin->OnExtenedInfo(ITMPlugin::EI_DRAW_TASKBAR_WND, L"1");
    }
    if (item->IsCustomDraw())
    {
        //Xác định chế độ tối hay sáng dựa trên độ sáng của màu nền
        const COLORREF& bk{ theApp.m_taskbar_data.back_color };
        int background_brightness{ (GetRValue(bk) + GetGValue(bk) + GetBValue(bk)) / 3 };
        //Do plugin tự vẽ
        if (plugin != nullptr && plugin->GetAPIVersion() >= 2)
        {
            plugin->OnExtenedInfo(ITMPlugin::EI_LABEL_TEXT_COLOR, std::to_wstring(label_text_color).c_str());
            plugin->OnExtenedInfo(ITMPlugin::EI_VALUE_TEXT_COLOR, std::to_wstring(value_text_color).c_str());
        }
        drawer.SetTextColor(value_text_color);
        //Cần RTTI
        if (typeid(drawer) == typeid(CDrawCommon))
        {
            auto* p_dc = static_cast<CDrawCommon&>(drawer).GetDC();
            item->DrawItem(p_dc->GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(), background_brightness < 128);
            p_dc->SelectObject(&m_font);
        }
        else if (typeid(drawer) == typeid(CTaskBarDlgDrawCommon))
        {
            auto& ref_d2d1_drawer = static_cast<CTaskBarDlgDrawCommon&>(drawer);
            ref_d2d1_drawer.ExecuteGdiOperation(rect,
                [item, rect, background_brightness](HDC gdi_dc)
                { item->DrawItem(gdi_dc,
                    rect.left,
                    rect.top,
                    rect.Width(),
                    rect.Height(),
                    background_brightness < 128); });
        }
        if (plugin != nullptr && plugin->GetAPIVersion() >= 8)
            item->DrawItemEx(&drawer, rect.left, rect.top, rect.Width(), rect.Height(), background_brightness < 128);
    }
    else
    {
        CRect rect_label, rect_value;
        rect_label = rect_value = rect;
        if (label_width > 0)
        {
            if (!vertical)
            {
                rect_label = rect_value = rect;
                rect_label.right = rect_label.left + label_width;
                rect_value.left = rect_label.right;
            }
            else
            {
                rect_label.bottom = rect_label.top + rect.Height() / 2;
                rect_value.top = rect_label.bottom;
            }
        }
        //Vẽ nhãn
        CString lable_text = theApp.m_taskbar_data.disp_str.GetConst(item).c_str();
        drawer.DrawWindowText(rect_label, lable_text, label_text_color, (vertical ? IDrawCommon::Alignment::CENTER : IDrawCommon::Alignment::LEFT));
        //Vẽ giá trị
        IDrawCommon::Alignment value_alignment{ theApp.m_taskbar_data.value_right_align ? IDrawCommon::Alignment::RIGHT : IDrawCommon::Alignment::LEFT };      //Kiểu căn chỉnh của giá trị
        if (vertical)
            value_alignment = IDrawCommon::Alignment::CENTER;
        drawer.DrawWindowText(rect_value, item->GetItemValueText(), value_text_color, value_alignment);
    }
}

void CTaskBarDlg::MoveWindow(CRect rect)
{
    if (IsWindow(GetSafeHwnd()))
    {
        ::MoveWindow(GetSafeHwnd(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
    }
}

void CTaskBarDlg::DisableRenderFeatureIfNecessary(CSupportedRenderEnums& ref_supported_render_enums)
{
    bool is_transparent = theApp.m_taskbar_data.IsTaskbarTransparent();
    // UpdateLayeredWindowIndirect thất bại thì tắt D2D
    if (theApp.m_taskbar_data.update_layered_window_error_code)
    {
        ref_supported_render_enums.DisableD2D1();
    }
    // Không thỏa điều kiện thì dùng Default (MFC)
    if (!is_transparent || theApp.m_taskbar_data.auto_set_background_color || theApp.m_taskbar_data.disable_d2d)
    {
        ref_supported_render_enums.EnableDefaultOnly();
    }
}

void CTaskBarDlg::TryDrawStatusBar(IDrawCommon& drawer, const CRect& rect_bar, int usage_percent)
{
    //Giới hạn phạm vi
    if (usage_percent > 100)
        usage_percent = 100;
    if (usage_percent < 0)
        usage_percent = 0;
    COLORREF graph_color = theApp.m_taskbar_data.GetUsageGraphColor();
    CSize fill_size = CSize(rect_bar.Width() * usage_percent / 100, rect_bar.Height());
    CRect rect_fill(rect_bar.TopLeft(), fill_size);
    if (theApp.m_taskbar_data.show_graph_dashed_box)
        drawer.DrawRectOutLine(rect_bar, graph_color, 1, true);
    drawer.FillRect(rect_fill, graph_color);
}

bool CTaskBarDlg::AdjustWindowPos(bool force_adjust)
{
    if (this->GetSafeHwnd() == NULL || !IsWindow(this->GetSafeHwnd()))
        return false;

    if (m_is_width_changed)
        force_adjust = true;

    if (force_adjust)
        ResetTaskbarPos();

    ::GetWindowRect(m_hTaskbar, m_rcTaskbar);   //Lấy vùng hình chữ nhật của thanh tác vụ

    static bool last_taskbar_on_top_or_bottom;
    CheckTaskbarOnTopOrBottom();
    if (force_adjust || m_taskbar_on_top_or_bottom != last_taskbar_on_top_or_bottom)
    {
        CalculateWindowSize();
        last_taskbar_on_top_or_bottom = m_taskbar_on_top_or_bottom;
        force_adjust = true;
    }

    AdjustTaskbarWndPos(force_adjust);

    //Nếu cửa sổ chưa nhúng thành công vào thanh tác vụ và đã bị di chuyển tới vị trí tuyệt đối tính từ góc trên bên trái màn hình thì sửa lại vị trí cửa sổ
    if (m_connot_insert_to_task_bar)
    {
        CRect rc_parent;
        ::GetWindowRect(GetParentHwnd(), rc_parent);
        CRect rect{ m_rect };
        rect.MoveToXY(rect.left + rc_parent.left, rect.top + rc_parent.top);
        this->MoveWindow(rect);

        if (::GetForegroundWindow() == m_hTaskbar)   //Khi cửa sổ không nhúng được vào thanh tác vụ, nếu tiêu điểm đang ở trên thanh tác vụ thì cho cửa sổ luôn trên cùng
        {
            SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);         //Đặt luôn trên cùng
        }
    }

    m_is_width_changed = false;     //Đặt lại cờ sau khi điều chỉnh xong vị trí cửa sổ
    return true;
}

void CTaskBarDlg::ApplyWindowTransparentColor()
{
#ifndef COMPILE_FOR_WINXP
    if (theApp.IsWindows11Taskbar())      //Trên Windows 11 màu nền không dùng màu đen thuần, để khắc phục lỗi không hiện được menu chuột phải ở chế độ tối
    {
        if (theApp.m_taskbar_data.transparent_color == 0 && theApp.m_taskbar_data.back_color == 0)
        {
            theApp.m_taskbar_data.transparent_color = 1;
            theApp.m_taskbar_data.back_color = 1;
        }
    }
    if ((theApp.m_taskbar_data.transparent_color != 0) && theApp.m_taksbar_transparent_color_enable)
    {
        auto render_type = m_supported_render_enums.GetAutoFitEnum();
        switch (render_type)
        {
            using namespace DrawCommonHelper;
        case RenderType::D2D1_WITH_DCOMPOSITION:
            // Vẽ bằng D2D và trộn bằng DComposition, không thiết lập thuộc tính nào
            break;
        case RenderType::D2D1:
            // Vẽ bằng D2D trộn alpha trực tiếp, chỉ thiết lập WS_EX_LAYERED
            SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            break;
        case RenderType::DEFAULT:
            // Vẽ bằng GDI dùng tách nền theo màu khóa (color key)
            // Chỉ khởi động bộ kết xuất D2D khi trong suốt và không dùng màu nền tự động
            SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(theApp.m_taskbar_data.transparent_color, 0, LWA_COLORKEY);
        }
    }
    else
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
    }
#endif // !COMPILE_FOR_WINXP
}

//bool CTaskBarDlg::IsTaskbarChanged()
//{
//    bool is_scendary_display;
//    static HWND last_taskbar_handle{};
//    HWND taskbar_handle = FindTaskbarHandle(is_scendary_display);
//    bool changed{ false };
//    if (last_taskbar_handle != taskbar_handle)
//    {
//        changed = true;
//        last_taskbar_handle = taskbar_handle;
//    }
//    return changed;
//}

void CTaskBarDlg::WidthChanged()
{
    m_is_width_changed = true;
}

const CRect& CTaskBarDlg::GetRectForDpiCheck() const
{
    return m_rcTaskbar;
}

UINT CTaskBarDlg::GetDPI() const
{
    return m_taskbar_dpi;
}

void CTaskBarDlg::SetDPI(UINT dpi)
{
    m_taskbar_dpi = dpi;
}

UINT CTaskBarDlg::DPI(UINT pixel) const
{
    return m_taskbar_dpi * pixel / 96;
}

int CTaskBarDlg::DPI(int pixel) const
{
    return static_cast<int>(m_taskbar_dpi) * pixel / 96;
}

LONG CTaskBarDlg::DPI(LONG pixel) const
{
    return static_cast<LONG>(m_taskbar_dpi) * pixel / 96;
}

void CTaskBarDlg::DPI(CRect& rect) const
{
    rect.left = DPI(rect.left);
    rect.right = DPI(rect.right);
    rect.top = DPI(rect.top);
    rect.bottom = DPI(rect.bottom);
}

CTaskBarDlg::ClassCheckWindowMonitorDPIAndHandle CTaskBarDlg::CheckWindowMonitorDPIAndHandle{};

UINT CTaskBarDlg::ClassCheckWindowMonitorDPIAndHandle::buffered_dpi_x{0};
UINT CTaskBarDlg::ClassCheckWindowMonitorDPIAndHandle::buffered_dpi_y{0};
UINT CTaskBarDlg::ClassCheckWindowMonitorDPIAndHandle::dpi_x{0};
UINT CTaskBarDlg::ClassCheckWindowMonitorDPIAndHandle::dpi_y{0};

HWND CTaskBarDlg::FindTaskbarHandle(bool& is_scendary_display)
{
    is_scendary_display = false;
    HWND hTaskbar = nullptr;
    //Hiển thị trên màn hình phụ
    if (theApp.m_taskbar_data.show_taskbar_wnd_in_secondary_display && CWindowsSettingHelper::IsTaskbarShowingInAllDisplays())
    {
        //Lấy thanh tác vụ của tất cả màn hình phụ
        std::vector<HWND> secondary_taskbars;
        CTaskbarHelper::GetAllSecondaryDisplayTaskbar(secondary_taskbars);
        if (!secondary_taskbars.empty())
        {
            int index = theApp.m_taskbar_data.secondary_display_index;
            if (index < 0)
                index = 0;
            if (index >= static_cast<int>(secondary_taskbars.size()))
                index = static_cast<int>(secondary_taskbars.size()) - 1;
            hTaskbar = secondary_taskbars[index];
            if (hTaskbar != nullptr)
                is_scendary_display = true;
        }
    }
    if (hTaskbar == nullptr)
        hTaskbar = ::FindWindow(_T("Shell_TrayWnd"), NULL);

    return hTaskbar;
}

CString CTaskBarDlg::GetMouseTipsInfo()
{
    CString tip_info;
    CString temp;
    temp.Format(_T("%s: %s\r\n (%s: %s/%s: %s)"), CCommon::LoadText(IDS_TRAFFIC_USED_TODAY),
        CCommon::KBytesToString((theApp.m_today_up_traffic + theApp.m_today_down_traffic) / 1024u),
        CCommon::LoadText(IDS_UPLOAD), CCommon::KBytesToString(theApp.m_today_up_traffic / 1024u),
        CCommon::LoadText(IDS_DOWNLOAD), CCommon::KBytesToString(theApp.m_today_down_traffic / 1024u)
    );
    tip_info += temp;
    if (!IsItemShow(TDI_UP))
    {
        temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_UPLOAD),
            CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_taskbar_data));
        tip_info += temp;
    }
    if (!IsItemShow(TDI_DOWN))
    {
        temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_DOWNLOAD),
            CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_taskbar_data));
        tip_info += temp;
    }
    if (!IsItemShow(TDI_CPU))
    {
        temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_CPU_USAGE), theApp.m_cpu_usage);
        tip_info += temp;
    }
    if (!IsShowCpuMemory())
    {
        temp.Format(_T("\r\n%s: %s/%s"),
            CCommon::LoadText(IDS_MEMORY_USAGE),
            CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory));
    }
    if (!IsItemShow(TDI_MEMORY))
    {
        temp.Format(_T("\r\n%s: %s/%s (%d %%)"), CCommon::LoadText(IDS_MEMORY_USAGE),
            CCommon::KBytesToString(theApp.m_used_memory),
            CCommon::KBytesToString(theApp.m_total_memory), theApp.m_memory_usage);
        tip_info += temp;
    }
    else
    {
        temp.Format(_T("\r\n%s: %s/%s"), CCommon::LoadText(IDS_MEMORY_USAGE),
            CCommon::KBytesToString(theApp.m_used_memory),
            CCommon::KBytesToString(theApp.m_total_memory));
        tip_info += temp;
    }
    if (!IsItemShow(TDI_CPU_FREQ) && theApp.m_cpu_freq > 0)
    {
        temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_CPU_FREQ), CCommon::FreqToString(theApp.m_cpu_freq, theApp.m_taskbar_data));
        tip_info += temp;
    }

    if (!IsItemShow(TDI_GPU_USAGE) && theApp.m_gpu_usage >= 0)
    {
        temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_GPU_USAGE), theApp.m_gpu_usage);
        tip_info += temp;
    }

#ifndef WITHOUT_TEMPERATURE
    CTrafficMonitorDlg* pMainWnd = dynamic_cast<CTrafficMonitorDlg*>(theApp.m_pMainWnd);
    if (pMainWnd->IsTemperatureNeeded())
    {
        if (!IsItemShow(TDI_CPU_TEMP) && theApp.m_cpu_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_CPU_TEMPERATURE), CCommon::TemperatureToString(theApp.m_cpu_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsItemShow(TDI_GPU_TEMP) && theApp.m_gpu_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_GPU_TEMPERATURE), CCommon::TemperatureToString(theApp.m_gpu_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsItemShow(TDI_HDD_TEMP) && theApp.m_hdd_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_HDD_TEMPERATURE), CCommon::TemperatureToString(theApp.m_hdd_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsItemShow(TDI_MAIN_BOARD_TEMP) && theApp.m_main_board_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE), CCommon::TemperatureToString(theApp.m_main_board_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
    }
#endif
    if (!IsItemShow(TDI_HDD_USAGE) && theApp.m_hdd_usage >= 0)
    {
        temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_HDD_USAGE), theApp.m_hdd_usage);
        tip_info += temp;
    }

    //Thêm chú giải chuột cho mục plugin
    tip_info += theApp.GetPlauginTooltipInfo().c_str();

    return tip_info;
}

void CTaskBarDlg::SetTextFont()
{
    //Nếu m_font đã gắn với một đối tượng tài nguyên phông chữ thì giải phóng nó
    if (m_font.m_hObject)
    {
        m_font.DeleteObject();
    }
    //Tạo phông chữ mới
    theApp.m_taskbar_data.font.Create(m_font, GetDPI());
}

void CTaskBarDlg::ApplySettings()
{
    SetTextFont();
    CalculateWindowSize();
}

void CTaskBarDlg::CalculateWindowSize()
{
    bool horizontal_arrange = theApp.m_taskbar_data.horizontal_arrange && m_taskbar_on_top_or_bottom;
    if (theApp.m_taskbar_data.display_item.IsEmpty() && theApp.m_taskbar_data.plugin_display_item.data().empty())
        theApp.m_taskbar_data.display_item.Add(TDI_UP);        //Hiển thị ít nhất một mục

    m_item_widths.clear();
    //Chiều rộng của mục hiển thị
    std::map<CommonDisplayItem, ItemWidth> item_widths;

    m_pDC->SelectObject(&m_font);
    //Tính chiều rộng của nhãn và giá trị
    //const auto& item_map = theApp.m_taskbar_data.disp_str.GetAllItems();
    for (auto iter = theApp.m_plugins.AllDisplayItemsWithPlugins().begin(); iter != theApp.m_plugins.AllDisplayItemsWithPlugins().end(); ++iter)
    {
        if (iter->IsPlugin())
        {
            auto plugin = iter->PluginItem();
            if (plugin != nullptr && theApp.m_taskbar_data.plugin_display_item.Contains(plugin->GetItemId()))
            {
                //Chiều rộng nhãn
                int& label_width{ item_widths[*iter].label_width };
                //Chiều rộng giá trị
                int& value_width{ item_widths[plugin].value_width };
                if (plugin->IsCustomDraw())
                {
                    label_width = 0;
                    value_width = theApp.m_plugins.GetItemWidth(plugin, m_pDC);
                }
                else
                {
                    CString lable_text = theApp.m_taskbar_data.disp_str.GetConst(plugin).c_str();
                    label_width = m_pDC->GetTextExtent(lable_text).cx;
                    value_width = m_pDC->GetTextExtent(plugin->GetItemValueSampleText()).cx;
                }
            }
        }
        else
        {
            //Chiều rộng nhãn
            item_widths[*iter].label_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.GetConst(*iter).c_str()).cx;
            //Chiều rộng giá trị
            CString sample_str = iter->GetItemValueSampleText(false);
            item_widths[*iter].value_width = m_pDC->GetTextExtent(sample_str).cx;
        }
    }

    auto item_order{ theApp.m_taskbar_data.item_order.GetAllDisplayItemsWithOrder() };
    for (const auto& item : item_order)
    {
        if (theApp.IsTaksbarItemDisplayed(item))
        {
            ItemWidthInfo width_info = item;
            width_info.item_width = item_widths[item];
            m_item_widths.push_back(width_info);
        }
    }

    int item_count = static_cast<int>(m_item_widths.size());

    auto last_window_width{ m_window_width };
    auto last_window_height{ m_window_height };

    //Tính tổng chiều rộng/chiều cao cửa sổ và các vùng hình chữ nhật
    if (IsTasksbarOnTopOrBottom())  //Khi thanh tác vụ nằm ở trên cùng hoặc dưới cùng màn hình
    {
        if (!horizontal_arrange)
            m_window_height = TASKBAR_WND_HEIGHT;
        else
            m_window_height = TASKBAR_WND_HEIGHT * 2 / 3;

        m_window_width = 0;
        if (theApp.m_taskbar_data.horizontal_arrange)   //Khi xếp ngang
        {
            CRect item_rect{};
            int index = 0;
            for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
            {
                m_window_width += iter->item_width.TotalWidth();
                if (index > 0)
                    item_rect.MoveToX(item_rect.right + DPI(theApp.m_taskbar_data.item_space));
                item_rect.right = item_rect.left + iter->item_width.TotalWidth();
                item_rect.bottom = item_rect.top + m_window_height;
                m_item_rects[*iter] = item_rect;
                index++;
            }
            m_window_width += DPI(theApp.m_taskbar_data.item_space) * item_count;   //Cộng thêm khoảng trống giữa các nhãn
        }
        else        //Khi không xếp ngang thì xếp theo từng cặp hai mục
        {
            int current_x = DPI(theApp.m_taskbar_data.item_space); // Tọa độ X ban đầu là khoảng trống bên trái
            int item_space = DPI(theApp.m_taskbar_data.item_space);

            //Tính vị trí dọc của vùng trên và dưới khi xếp theo cặp
            int item_height = m_window_height / 2;
            int y_pos_up = -DPI(theApp.m_taskbar_data.vertical_margin) / 2;
            int y_pos_down = y_pos_up + item_height + DPI(theApp.m_taskbar_data.vertical_margin);

            int width0 = 0;
            int max_width0 = 0;
            bool has_first_item = false;
            auto first_iter = m_item_widths.end(); // Dùng để lưu iterator của mục đầu tiên trong cột hiện tại, để sửa chiều rộng của nó về sau

            for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
            {
                if (iter->IsDoubleLineExclusive())
                {
                    // Nếu phía trước có mục một dòng chưa được ghép cặp thì giờ nó buộc phải đứng riêng một cột và nên chiếm hai dòng
                    if (has_first_item)
                    {
                        int col_width = max_width0; // Khi chiếm hai dòng thì chiều rộng dùng MaxWidth()
                        m_item_rects[*first_iter].SetRect(current_x, 0, current_x + col_width, m_window_height);
                        first_iter->is_double_line = true;

                        current_x += col_width + item_space;
                        has_first_item = false;
                    }

                    // Xử lý mục chiếm trọn hai dòng hiện tại: đứng riêng một cột, chiếm hai dòng
                    int col_width = iter->item_width.MaxWidth();
                    m_item_rects[*iter].SetRect(current_x, 0, current_x + col_width, m_window_height);
                    iter->is_double_line = true;

                    current_x += col_width + item_space;
                }
                else
                {
                    iter->is_double_line = false;
                    if (!has_first_item)
                    {
                        // Mục đầu tiên của cột hiện tại (nửa trên), chiếm một dòng
                        width0 = iter->item_width.TotalWidth();
                        max_width0 = iter->item_width.MaxWidth();

                        first_iter = iter;
                        // Tạm thời thiết lập rect, mép right sẽ được sửa theo chiều rộng lớn nhất của cột khi ghép cặp thành công hoặc khi kết thúc vòng lặp
                        m_item_rects[*iter].SetRect(current_x, y_pos_up, current_x + width0, y_pos_up + item_height);

                        has_first_item = true;
                    }
                    else
                    {
                        // Mục thứ hai của cột hiện tại (nửa dưới), chiếm một dòng, cùng mục đầu tiên tạo thành một cột
                        int col_width = max(width0, iter->item_width.TotalWidth());

                        // Sửa mép phải của mục đầu tiên, đảm bảo hai mục trong cùng một cột có chiều rộng bằng nhau
                        m_item_rects[*first_iter].right = current_x + col_width;

                        // Thiết lập vùng hình chữ nhật của mục thứ hai
                        m_item_rects[*iter].SetRect(current_x, y_pos_down, current_x + col_width, y_pos_down + item_height);

                        current_x += col_width + item_space;
                        has_first_item = false;
                    }
                }
            }

            // Sau khi kết thúc vòng lặp, nếu vẫn còn mục một dòng chưa ghép cặp (ví dụ tổng số mục là lẻ và mục cuối không chiếm trọn hai dòng) thì nó đứng riêng một cột và nên chiếm hai dòng
            if (has_first_item)
            {
                int col_width = max_width0; // Khi chiếm hai dòng thì chiều rộng dùng MaxWidth()
                m_item_rects[*first_iter].SetRect(current_x, 0, current_x + col_width, m_window_height);
                first_iter->is_double_line = true;

                current_x += col_width + item_space;
            }

            // Lưu tổng chiều rộng
            m_window_width = current_x;
        }
    }
    else        //Khi thanh tác vụ nằm ở hai bên màn hình
    {
        m_window_width = 0;
        m_window_height = 0;
        CRect item_rect;
        for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
        {
            //Chiều rộng lớn nhất trong tất cả nhãn chính là chiều rộng cửa sổ
            if (m_window_width < iter->item_width.TotalWidth())
                m_window_width = iter->item_width.TotalWidth();

            int item_height = TASKBAR_WND_HEIGHT / 2;
            if (iter->IsDoubleLineExclusive())
                item_height = TASKBAR_WND_HEIGHT;

            //Tính vùng hình chữ nhật
            item_rect.top = m_window_height;
            item_rect.bottom = item_rect.top + item_height;
            int item_width = min(m_window_width, m_rcTaskbar.Width() - DPI(2));
            item_rect.left = 0;
            item_rect.right = item_rect.left + item_width;
            m_item_rects[*iter] = item_rect;

            m_window_height += item_height;
            m_window_height += DPI(theApp.m_taskbar_data.item_space);
        }
    }

    // Nếu kích thước cửa sổ thay đổi thì điều chỉnh lại vị trí cửa sổ thanh tác vụ
    if (last_window_width != m_window_width || last_window_height != m_window_height) {
        WidthChanged();
    }

    m_rect.right = m_rect.left + m_window_width;
    m_rect.bottom = m_rect.top + m_window_height;

}

void CTaskBarDlg::SetToolTipsTopMost()
{
    m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
}

void CTaskBarDlg::UpdateToolTips()
{
    if (theApp.m_taskbar_data.show_tool_tip && IsWindow(m_tool_tips.GetSafeHwnd()))
    {
        CString tip_info;
        tip_info = GetMouseTipsInfo();
        m_tool_tips.UpdateTipText(tip_info, this);
    }
}

bool CTaskBarDlg::IsItemShow(DisplayItem item)
{
    return (theApp.m_taskbar_data.display_item.Contains(item));
}

bool CTaskBarDlg::IsShowCpuMemory()
{
    return (theApp.m_taskbar_data.display_item.Contains(TDI_CPU) || theApp.m_taskbar_data.display_item.Contains(TDI_MEMORY));
}

bool CTaskBarDlg::IsShowNetSpeed()
{
    return (theApp.m_taskbar_data.display_item.Contains(TDI_UP) || theApp.m_taskbar_data.display_item.Contains(TDI_DOWN));
}

bool CTaskBarDlg::IsTaskbarCloseToIconEnable(bool taskbar_wnd_on_left)
{
    //Trên Windows 11, khi cửa sổ thanh tác vụ hiển thị ở bên phải, nếu chọn “Cửa sổ thanh tác vụ gần biểu tượng thay vì gần cạnh thanh tác vụ” sẽ bị chồng lên biểu tượng
    //Vì vùng hình chữ nhật của cửa sổ "MSTaskSwWClass" không còn là vùng chứa các nút biểu tượng thu nhỏ trên thanh tác vụ
    return theApp.m_win_version.IsWindows11OrLater() &&
        CWindowsSettingHelper::IsTaskbarCenterAlign() && taskbar_wnd_on_left;
}

BOOL CTaskBarDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây
    SetWindowText(TASKBAR_WINDOW_NAME);
    // Kiểm tra hệ thống đã cài MicrosoftWindows.Client.WebExperience (còn gọi là Windows Web Experience Pack) chưa
    theApp.m_taskbar_data.is_windows_web_experience_detected =
        WindowsWebExperienceDetector::IsDetected();
    // Tắt các tùy chọn kết xuất cần thiết theo thiết lập cửa sổ thanh tác vụ; chỉ dùng kết xuất D2D khi trong suốt và hỗ trợ D2D
    DisableRenderFeatureIfNecessary(m_supported_render_enums);
    //Thiết lập ẩn biểu tượng trên thanh tác vụ
    ModifyStyleEx(0, WS_EX_TOOLWINDOW);

    m_pDC = GetDC();

    m_hTaskbar = FindTaskbarHandle(m_is_secondary_display); //Tìm handle của thanh tác vụ
    ::GetWindowRect(m_hTaskbar, m_rcTaskbar);   //Lấy vùng hình chữ nhật của thanh tác vụ

    //Thiết lập màu trong suốt của cửa sổ
    ApplyWindowTransparentColor();

    InitTaskbarWnd();
    m_connot_insert_to_task_bar = !(::SetParent(this->m_hWnd, GetParentHwnd())); //Đặt cửa sổ chương trình thành cửa sổ con của thanh tác vụ

    //Từ vùng cửa sổ thu nhỏ của thanh tác vụ đã xác định, lấy màn hình tương ứng và DPI của màn hình đó (Windows 8.1 trở lên)
    if (theApp.m_win_version.IsWindows8Point1OrLater())
    {
        UINT dpi_x, dpi_y;
        if (theApp.DPIFromRect(GetRectForDpiCheck(), &dpi_x, &dpi_y))
            SetDPI(dpi_x);
        else
            SetDPI(theApp.GetDpi());
    }
    else
    {
        SetDPI(theApp.GetDpi());
    }

    //Thiết lập phông chữ
    SetTextFont();
    m_pDC->SelectObject(&m_font);

    CheckTaskbarOnTopOrBottom();
    CalculateWindowSize();
    m_rect.SetRectEmpty();
    m_rect.bottom = m_window_height;
    m_rect.right = m_rect.left + m_window_width;
    m_error_code = GetLastError();
    AdjustWindowPos(true);

    SetBackgroundColor(theApp.m_taskbar_data.back_color);

    //Khởi tạo chú giải chuột
    if (IsWindow(GetSafeHwnd()) && m_tool_tips.Create(this, TTS_ALWAYSTIP) && IsWindow(m_tool_tips.GetSafeHwnd()))
    {
        m_tool_tips.SetMaxTipWidth(600);
        m_tool_tips.AddTool(this, _T(""));
        SetToolTipsTopMost();       //Đặt chú giải luôn trên cùng
    }

    SetTimer(TASKBAR_TIMER, 1000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}

void CTaskBarDlg::OnCancel()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    //SaveConfig();

    //Đóng tất cả cửa sổ có cửa sổ cha là cửa sổ thanh tác vụ
    for (const auto& item : CBaseDialog::AllUniqueHandels())
    {
        HWND parent = ::GetParent(item.second);
        if (parent == GetSafeHwnd())
        {
            ::SendMessage(item.second, WM_COMMAND, IDCANCEL, 0);
        }
    }

    DestroyWindow();
    ResetTaskbarPos();

    //CDialogEx::OnCancel();
}

void CTaskBarDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    m_menu_popuped = true;
    m_tool_tips.Pop();
    ITMPlugin* plugin{};
    bool is_plugin_item_clicked = (CheckClickedItem(point) && m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr);
    if (is_plugin_item_clicked)
    {
        plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.PluginItem()->OnMouseEvent(IPluginItem::MT_RCLICKED, point.x, point.y, (void*)GetSafeHwnd(), IPluginItem::MF_TASKBAR_WND) != 0)
                return;
        }
    }

    CPoint point1;  //Khai báo vị trí dùng để xác định vị trí con trỏ chuột
    GetCursorPos(&point1);  //Lấy vị trí con trỏ chuột hiện tại để menu đi theo con trỏ
    CMenu* pMenu = (is_plugin_item_clicked ? theApp.m_taskbar_menu_plugin.GetSubMenu(0) : theApp.m_taskbar_menu.GetSubMenu(0));
    if (pMenu != nullptr)
    {
        if (plugin != nullptr)
        {
            //Đổi văn bản hiển thị của menu plugin trong menu chuột phải thành tên plugin
            const int PLUGIN_ITEM_INDEX = pMenu->GetMenuItemCount() - 1;    //Mục plugin là mục cuối cùng trong menu
            pMenu->ModifyMenu(PLUGIN_ITEM_INDEX, MF_BYPOSITION, PLUGIN_ITEM_INDEX, plugin->GetInfo(ITMPlugin::TMI_NAME));
            //Lấy biểu tượng plugin
            HICON plugin_icon{};
            if (plugin->GetAPIVersion() >= 5)
                plugin_icon = (HICON)plugin->GetPluginIcon();
            //Thiết lập biểu tượng plugin
            if (plugin_icon != nullptr)
                CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), PLUGIN_ITEM_INDEX, TRUE, plugin_icon);
        }
        //Cập nhật menu con của plugin
        theApp.UpdatePluginMenu(&theApp.m_taskbar_menu_plugin_sub_menu, plugin, 2);
        //Menu bật lên
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);
    }
    CDialogEx::OnRButtonUp(nFlags, point1);
}

void CTaskBarDlg::OnInitMenu(CMenu* pMenu)
{
    CDialogEx::OnInitMenu(pMenu);

    // TODO: Thêm mã xử lý thông điệp tại đây
    pMenu->CheckMenuItem(ID_SHOW_MAIN_WND, MF_BYCOMMAND | (!theApp.m_cfg_data.m_hide_main_window ? MF_CHECKED : MF_UNCHECKED));

    pMenu->EnableMenuItem(ID_SELECT_ALL_CONNECTION, MF_BYCOMMAND | (theApp.m_general_data.show_all_interface ? MF_GRAYED : MF_ENABLED));
    pMenu->EnableMenuItem(ID_CHECK_UPDATE, MF_BYCOMMAND | (theApp.IsCheckingForUpdate() ? MF_GRAYED : MF_ENABLED));

    //pMenu->SetDefaultItem(ID_NETWORK_INFO);
    //Thiết lập mục menu mặc định
    switch (theApp.m_taskbar_data.double_click_action)
    {
    case DoubleClickAction::CONNECTION_INFO:
        pMenu->SetDefaultItem(ID_NETWORK_INFO);
        break;
    case DoubleClickAction::HISTORY_TRAFFIC:
        pMenu->SetDefaultItem(ID_TRAFFIC_HISTORY);
        break;
    case DoubleClickAction::SHOW_MORE_INFO:
        pMenu->SetDefaultItem(ID_SHOW_CPU_MEMORY2);
        break;
    case DoubleClickAction::OPTIONS:
        pMenu->SetDefaultItem(ID_OPTIONS2);
        break;
    case DoubleClickAction::TASK_MANAGER:
        pMenu->SetDefaultItem(ID_OPEN_TASK_MANAGER);
        break;
    default:
        pMenu->SetDefaultItem(-1);
        break;
    }

    //Thiết lập trạng thái đánh dấu cho lệnh của plugin
    ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
    if (plugin != nullptr && plugin->GetAPIVersion() >= 5)
    {
        for (int i = ID_PLUGIN_COMMAND_START; i <= ID_PLUGIN_COMMAND_MAX; i++)
        {
            bool checked = (plugin->IsCommandChecked(i - ID_PLUGIN_COMMAND_START) != 0);
            pMenu->CheckMenuItem(i, MF_BYCOMMAND | (checked ? MF_CHECKED : MF_UNCHECKED));
        }
    }

    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKBAR_MENU_POPED_UP, 0, 0); //Thông báo cho cửa sổ chính là menu đã bật lên
}

BOOL CTaskBarDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    //Chặn thoát bằng phím Enter và ESC
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

    if (theApp.m_taskbar_data.show_tool_tip && !m_menu_popuped && IsWindow(m_tool_tips.GetSafeHwnd()) && (pMsg->message == WM_LBUTTONDOWN ||
        pMsg->message == WM_LBUTTONUP ||
        pMsg->message == WM_MOUSEMOVE))
    {
        m_tool_tips.RelayEvent(pMsg);
    }

    if (pMsg->message == WM_KEYDOWN)
    {
        bool ctrl = (GetKeyState(VK_CONTROL) & 0x80);
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000);
        bool alt = (GetKeyState(VK_MENU) & 0x8000);
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr && plugin->GetAPIVersion() >= 4)
        {
            if (m_clicked_item.PluginItem()->OnKeboardEvent(static_cast<int>(pMsg->wParam), ctrl, shift, alt, (void*)GetSafeHwnd(), IPluginItem::KF_TASKBAR_WND) != 0)
                return TRUE;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}

void CTaskBarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây

    CDialogEx::OnMouseMove(nFlags, point);
}

void CTaskBarDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    if (CheckClickedItem(point) && m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr)
    {
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.PluginItem()->OnMouseEvent(IPluginItem::MT_DBCLICKED, point.x, point.y, (void*)GetSafeHwnd(), IPluginItem::MF_TASKBAR_WND) != 0)
                return;
        }
    }
    switch (theApp.m_taskbar_data.double_click_action)
    {
    case DoubleClickAction::CONNECTION_INFO:
        SendMessage(WM_COMMAND, ID_NETWORK_INFO);        //Nhấp đúp thì mở hộp thoại “Chi tiết kết nối”
        break;
    case DoubleClickAction::HISTORY_TRAFFIC:
        SendMessage(WM_COMMAND, ID_TRAFFIC_HISTORY);        //Nhấp đúp thì mở hộp thoại “Thống kê lưu lượng”
        break;
    case DoubleClickAction::SHOW_MORE_INFO:
        PostMessage(WM_COMMAND, ID_SHOW_CPU_MEMORY2);       //Bật/tắt hiển thị mức sử dụng CPU và bộ nhớ
        break;
    case DoubleClickAction::OPTIONS:
        SendMessage(WM_COMMAND, ID_OPTIONS2);       //Nhấp đúp thì mở hộp thoại “Tùy chọn”
        break;
    case DoubleClickAction::TASK_MANAGER:
        ShellExecuteW(NULL, _T("open"), (theApp.m_system_dir + L"\\Taskmgr.exe").c_str(), NULL, NULL, SW_NORMAL);       //Mở Trình quản lý tác vụ (Task Manager)
        break;
    case DoubleClickAction::SEPCIFIC_APP:
        ShellExecuteW(NULL, _T("open"), (theApp.m_taskbar_data.double_click_exe).c_str(), NULL, NULL, SW_NORMAL);   //Mở chương trình chỉ định, mặc định là Trình quản lý tác vụ
        break;
    default:
        break;
    }
    //CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CTaskBarDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    if (nIDEvent == TASKBAR_TIMER)
    {
        if (m_menu_popuped)     //Khi đang hiện menu chuột phải thì không hiển thị chú giải chuột
        {
            m_tool_tips.Pop();
        }
        
        //Mỗi giây tính lại chiều rộng cửa sổ
        CalculateWindowSize();
    }

    CDialogEx::OnTimer(nIDEvent);
}

BOOL CTaskBarDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    UINT uMsg = LOWORD(wParam);
    if (uMsg == ID_SELECT_ALL_CONNECTION || uMsg == ID_SELETE_CONNECTION
        || (uMsg > ID_SELECT_ALL_CONNECTION && uMsg <= ID_SELETE_CONNECTION_MAX))
    {
        ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_COMMAND, wParam, lParam);    //Nếu đã nhấn mục menu con “Chọn kết nối mạng” thì chuyển tiếp thông điệp tới cửa sổ chính
        return TRUE;
    }

    //Đã chọn mục plugin trong “Mục hiển thị”
    if (uMsg >= ID_SHOW_PLUGIN_ITEM_START && uMsg <= ID_SHOW_PLUGIN_ITEM_MAX)
    {
        IPluginItem* item = theApp.m_plugins.GetItemByIndex(uMsg - ID_SHOW_PLUGIN_ITEM_START);
        if (item != nullptr)
        {
            bool displayed = theApp.m_taskbar_data.plugin_display_item.Contains(item->GetItemId());
            theApp.m_taskbar_data.plugin_display_item.SetStrContained(item->GetItemId(), !displayed);
            ::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_REOPEN_TASKBAR_WND, 0, 0);
        }
    }
    //Đã chọn lệnh của plugin
    if (uMsg >= ID_PLUGIN_COMMAND_START && uMsg <= ID_PLUGIN_COMMAND_MAX)
    {
        int index = uMsg - ID_PLUGIN_COMMAND_START;
        if (m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr)
        {
            ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
            if (plugin != nullptr && plugin->GetAPIVersion() >= 5)
            {
                plugin->OnPluginCommand(index, (void*)GetSafeHwnd(), nullptr);
            }
        }
    }

    return CDialogEx::OnCommand(wParam, lParam);
}

void CTaskBarDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Thêm mã xử lý thông điệp tại đây
                       // Không gọi CDialogEx::OnPaint() cho thông điệp vẽ

    try
    {
        ShowInfo(&dc);
    }
    catch (CD3D10Exception1& ex)
    {
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler{ex}(
            [p_window_support_wrapper = &this->m_taskbar_draw_common_window_support](CHResultException& ex)
            {
                return DrawCommonHelper::HandleIfNeedRecreate(
                    ex,
                    [p_window_support_wrapper]()
                    { p_window_support_wrapper->Get().RequestD3D10Device1Recreate(); });
            });
    }
    catch (CD2D1Exception& ex)
    {
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler{ex}(
            [p_device_context_support_wrapper = &this->m_d2d1_device_context_support](CHResultException& ex)
            {
                return DrawCommonHelper::HandleIfD2D1DeviceNeedRecreate(
                    ex,
                    [&]()
                    { p_device_context_support_wrapper->Get().RequestD2D1DeviceRecreate(ex.GetHResult()); });
            });
    }
    catch (CDCompositionException& ex)
    {
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler{ex}(
            [p_device_context_support_wrapper = &this->m_d2d1_device_context_support](CHResultException& ex)
            {
                p_device_context_support_wrapper->Get().RequestDCompositionDeviceRecreate(ex.GetHResult());
                return true;
            });
    }
    catch (CHResultException& ex)
    {
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler{ex}();
    }
    catch (std::runtime_error& ex)
    {
        auto* log = ex.what();
        CCommon::WriteLog(log, theApp.m_log_path.c_str());
        // Hiện chỉ có nó chủ động ném ngoại lệ, nên mọi ngoại lệ đều tính cho nó
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::IncreaseErrorCountManually();
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::HandleErrorCountIncrement();
    }

}

void CTaskBarDlg::AddHisToList(CommonDisplayItem item_type, int current_usage_percent)
{
    int& data_count{ m_history_data_count[item_type] };
    std::list<int>& list = m_map_history_data[item_type];
    //Cộng dồn giá trị vào đầu danh sách liên kết, cho đến khi số dữ liệu đã thêm đạt bội số của TASKBAR_GRAPH_STEP thì tính trung bình
    if (data_count % TASKBAR_GRAPH_STEP == 0)
    {
        //Tính trung bình của TASKBAR_GRAPH_STEP dữ liệu đã cộng dồn trước đó
        if (!list.empty())
            list.front() /= TASKBAR_GRAPH_STEP;
        //Thêm dữ liệu mới vào cuối
        list.push_front(current_usage_percent);
    }
    else
    {
        //Cộng dồn giá trị vào đầu danh sách liên kết
        list.front() += current_usage_percent;
    }
    size_t graph_max_length = m_item_rects[item_type].Width();
    //Kiểm tra có vượt quá độ dài tối đa không; nếu vượt thì xóa dữ liệu ở cuối danh sách liên kết
    if (list.size() > graph_max_length)
    {
        list.pop_back();
    }
    data_count++;
}

int CTaskBarDlg::CalculateNetspeedPercent(unsigned __int64 net_speed)
{
    int percet = 0;
    unsigned __int64 max_value{ theApp.m_taskbar_data.GetNetspeedFigureMaxValueInBytes() };

    if (net_speed >= max_value)
        percet = 100;
    else if (max_value > 0)
        percet = static_cast<int>(net_speed * 100 / max_value);    //net_speed < max_value nên kết quả < 100
    return percet;
}

bool CTaskBarDlg::CheckClickedItem(CPoint point)
{
    for (const auto& item : m_item_rects)
    {
        if (item.second.PtInRect(point))
        {
            m_clicked_item = item.first;
            return true;
        }
    }
    m_clicked_item = TDI_UP;
    return false;
}

void CTaskBarDlg::TryDrawGraph(IDrawCommon& drawer, const CRect& value_rect, CommonDisplayItem item_type)
{
    std::list<int>& list = m_map_history_data[item_type];
    COLORREF graph_color = theApp.m_taskbar_data.GetUsageGraphColor();
    if (theApp.m_taskbar_data.show_graph_dashed_box)
        drawer.DrawRectOutLine(value_rect, graph_color, 1, true);
    int i{ -1 };
    for (int value : list)
    {
        i++;
        if (i == 0)     //Không vẽ dữ liệu ở đầu danh sách liên kết, vì đang trong quá trình cộng dồn, chưa lấy trung bình
            continue;
        if (i >= value_rect.Width())
            break;
        //Giới hạn phạm vi
        if (value > 100)
            value = 100;
        if (value < 0)
            value = 0;
        //Vẽ đường từ phải sang trái
        CPoint start_point = CPoint(value_rect.right - i, value_rect.bottom);
        int height = value * value_rect.Height() / 100;
        drawer.DrawLine(start_point, height, graph_color);
    }
}

void CTaskBarDlg::OnClose()
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKBAR_WND_CLOSED, 0, 0);

    CDialogEx::OnClose();
}

void CTaskBarDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    if (CheckClickedItem(point) && m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr)
    {
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.PluginItem()->OnMouseEvent(IPluginItem::MT_LCLICKED, point.x, point.y, (void*)GetSafeHwnd(), IPluginItem::MF_TASKBAR_WND) != 0)
                return;
        }
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}

afx_msg LRESULT CTaskBarDlg::OnExitmenuloop(WPARAM wParam, LPARAM lParam)
{
    m_menu_popuped = false;
    return 0;
}


afx_msg LRESULT CTaskBarDlg::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


BOOL CTaskBarDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (zDelta >= 120 || zDelta <= -120)
    {
        CPoint point = pt;
        ScreenToClient(&point);
        ITMPlugin* plugin{};
        bool is_plugin_item_clicked = (CheckClickedItem(point) && m_clicked_item.IsPlugin() && m_clicked_item.PluginItem() != nullptr);
        if (is_plugin_item_clicked)
        {
            plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.PluginItem());
            if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
            {
                IPluginItem::MouseEventType type;
                if (zDelta > 0)
                    type = IPluginItem::MT_WHEEL_UP;
                else
                    type = IPluginItem::MT_WHEEL_DOWN;
                if (m_clicked_item.PluginItem()->OnMouseEvent(type, point.x, point.y, (void*)GetSafeHwnd(), IPluginItem::MF_TASKBAR_WND) != 0)
                    return TRUE;
            }
        }
    }

    return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
