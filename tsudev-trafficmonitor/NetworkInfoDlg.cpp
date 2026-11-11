// CNetworkInfoDlg.cpp : tệp triển khai
//

#include "stdafx.h"
#include "TsudevTrafficMonitor.h"
#include "NetworkInfoDlg.h"
#include "afxdialogex.h"


// Hộp thoại CNetworkInfoDlg

IMPLEMENT_DYNAMIC(CNetworkInfoDlg, CBaseDialog)

CNetworkInfoDlg::CNetworkInfoDlg(const vector<NetWorkConection>& adapters, const vector<MIB_IFROW>& if_rows, int connection_selected, CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_NETWORK_INFO_DIALOG, pParent), m_connections(adapters), m_if_rows(if_rows), m_connection_selected(connection_selected)
{
    m_current_connection = connection_selected;
}

CNetworkInfoDlg::~CNetworkInfoDlg()
{
}


void CNetworkInfoDlg::ShowInfo()
{
    CString temp;
    MIB_IFROW& network_info = GetConnectIfTable(m_connection_selected);
    //Tên giao diện mạng
    m_info_list.SetItemText(0, 1, network_info.wszName);
    //Mô tả giao diện mạng
    m_info_list.SetItemText(1, 1, CCommon::StrToUnicode(CAdapterCommon::GetIfRowDescription(network_info).c_str()).c_str());
    //Loại kết nối
    switch (network_info.dwType)
    {
    case IF_TYPE_OTHER: temp = CCommon::LoadText(IDS_IF_TYPE_OTHER); break;
    case IF_TYPE_ETHERNET_CSMACD: temp = CCommon::LoadText(IDS_IF_TYPE_ETHERNET_CSMACD); break;
    case IF_TYPE_ISO88025_TOKENRING: temp = CCommon::LoadText(IDS_IF_TYPE_ISO88025_TOKENRING); break;
    case IF_TYPE_FDDI: temp = CCommon::LoadText(IDS_IF_TYPE_FDDI); break;
    case IF_TYPE_PPP: temp = CCommon::LoadText(IDS_IF_TYPE_PPP); break;
    case IF_TYPE_SOFTWARE_LOOPBACK: temp = CCommon::LoadText(IDS_IF_TYPE_SOFTWARE_LOOPBACK); break;
    case IF_TYPE_ATM: temp = CCommon::LoadText(IDS_IF_TYPE_ATM); break;
    case IF_TYPE_IEEE80211: temp = CCommon::LoadText(IDS_IF_TYPE_IEEE80211); break;
    case IF_TYPE_TUNNEL: temp = CCommon::LoadText(IDS_IF_TYPE_TUNNEL); break;
    case IF_TYPE_IEEE1394: temp = CCommon::LoadText(IDS_IF_TYPE_IEEE1394); break;
    case IF_TYPE_IEEE80216_WMAN: temp = CCommon::LoadText(IDS_IF_TYPE_IEEE80216_WMAN); break;
    case IF_TYPE_WWANPP: temp = CCommon::LoadText(IDS_IF_TYPE_WWANPP); break;
    case IF_TYPE_WWANPP2: temp = CCommon::LoadText(IDS_IF_TYPE_WWANPP2); break;
    default: temp = CCommon::LoadText(IDS_UNKNOW_CONNECTION); break;
    }
    m_info_list.SetItemText(2, 1, temp);
    //Tốc độ (dùng giá trị 64-bit: dwSpeed của MIB_IFROW bị giới hạn ở ~4,29 Gbps)
    temp.Format(_T("%I64uMbps"), CAdapterCommon::GetInterfaceSpeed(network_info) / 1000000);
    m_info_list.SetItemText(3, 1, temp);
    //Địa chỉ vật lý (MAC) của bộ điều hợp
    temp = _T("");
    char buff[3];
    for (size_t i{}; i < network_info.dwPhysAddrLen; i++)
    {
        sprintf_s(buff, "%.2x", network_info.bPhysAddr[i]);
        temp += buff;
        if (i != network_info.dwPhysAddrLen - 1)
            temp += _T('-');
    }
    m_info_list.SetItemText(4, 1, temp);
    //Địa chỉ IP
    m_info_list.SetItemText(5, 1, GetConnection(m_connection_selected).ip_address.c_str());
    //Mặt nạ mạng con (subnet mask)
    m_info_list.SetItemText(6, 1, GetConnection(m_connection_selected).subnet_mask.c_str());
    //Cổng mặc định (default gateway)
    m_info_list.SetItemText(7, 1, GetConnection(m_connection_selected).default_gateway.c_str());
    //Trạng thái kết nối
    switch (network_info.dwOperStatus)
    {
    case IF_OPER_STATUS_NON_OPERATIONAL: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_NON_OPERATIONAL); break;
    case IF_OPER_STATUS_UNREACHABLE: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_UNREACHABLE); break;
    case IF_OPER_STATUS_DISCONNECTED: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_DISCONNECTED); break;
    case IF_OPER_STATUS_CONNECTING: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_CONNECTING); break;
    case IF_OPER_STATUS_CONNECTED: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_CONNECTED); break;
    case IF_OPER_STATUS_OPERATIONAL: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_OPERATIONAL); break;
        //case IfOperStatusUp:
        //    temp = CCommon::LoadText(IDS_IF_OPER_STATUS_UP);
        //    break;
        //case IfOperStatusDown:
        //case IfOperStatusNotPresent:
        //case IfOperStatusLowerLayerDown:
        //    temp = CCommon::LoadText(IDS_IF_OPER_STATUS_DOWN);
        //    break;
        //case IfOperStatusTesting:
        //case IfOperStatusUnknown:
        //    temp = CCommon::LoadText(IDS_UNKNOW_STATUS);
        //    break;
        //case IfOperStatusDormant:
        //    temp = CCommon::LoadText(IDS_IF_OPER_STATUS_DORMANT);
            //break;
    default: temp = CCommon::LoadText(IDS_UNKNOW_STATUS); break;
    }
    m_info_list.SetItemText(8, 1, temp);
    ShowTrafficInfo();

    //Hiển thị chỉ báo lựa chọn hiện tại; kết nối không hoạt động (card bị tắt, rút cáp...) được ghi chú thêm
    //để người dùng hiểu vì sao IP là 0.0.0.0 và số byte bằng 0
    CString str;
    str.Format(_T("%d/%d"), m_connection_selected + 1, static_cast<int>(m_connections.size()));
    if (!IsConnectionActive(m_connection_selected))
    {
        str += _T(" - ");
        str += CCommon::LoadText(IDS_CONNECTION_INACTIVE);
    }
    SetDlgItemText(IDC_INDEX_STATIC, str);
    CFont* font = GetFont();
    CWnd* index_static = GetDlgItem(IDC_INDEX_STATIC);
    if (m_current_connection == m_connection_selected && !theApp.m_cfg_data.m_select_all)
        index_static->SetFont(&m_font_bold);
    else
        index_static->SetFont(font);
    m_info_list.Invalidate(FALSE);      //Vẽ lại để cập nhật màu chữ mờ/thường theo trạng thái kết nối
}

void CNetworkInfoDlg::ShowTrafficInfo()
{
    const MIB_IFROW& network_info = GetConnectIfTable(m_connection_selected);
    NetWorkConection connection = GetConnection(m_connection_selected);
    unsigned __int64 in_bytes{}, out_bytes{};
    CAdapterCommon::GetInterfaceOctets(network_info, in_bytes, out_bytes);
    CString temp;
    //Số byte đã nhận
    temp.Format(_T("%s (%s)"), CCommon::IntToString(in_bytes, true, true), CCommon::DataSizeToString(in_bytes));
    m_info_list.SetItemText(9, 1, temp);
    //Số byte đã gửi
    temp.Format(_T("%s (%s)"), CCommon::IntToString(out_bytes, true, true), CCommon::DataSizeToString(out_bytes));
    m_info_list.SetItemText(10, 1, temp);
    //Số byte đã nhận/gửi kể từ khi chương trình khởi tạo kết nối. Nếu bộ đếm bị đặt lại (ví dụ card mạng
    //được tắt rồi bật lại) thì giá trị hiện tại nhỏ hơn giá trị ban đầu: hiển thị giá trị hiện tại thay vì số âm bị tràn.
    unsigned __int64 in_bytes_since_start = (in_bytes >= connection.in_bytes) ? in_bytes - connection.in_bytes : in_bytes;
    temp.Format(_T("%s (%s)"), CCommon::IntToString(in_bytes_since_start, true, true), CCommon::DataSizeToString(in_bytes_since_start));
    m_info_list.SetItemText(11, 1, temp);
    unsigned __int64 out_bytes_since_start = (out_bytes >= connection.out_bytes) ? out_bytes - connection.out_bytes : out_bytes;
    temp.Format(_T("%s (%s)"), CCommon::IntToString(out_bytes_since_start, true, true), CCommon::DataSizeToString(out_bytes_since_start));
    m_info_list.SetItemText(12, 1, temp);
}

void CNetworkInfoDlg::GetProgramElapsedTime()
{
    //Thời gian chương trình đã chạy
    SYSTEMTIME current_time, time;
    GetLocalTime(&current_time);
    time = CCommon::CompareSystemTime(current_time, m_start_time);
    CString temp;
    temp.Format(CCommon::LoadText(IDS_HOUR_MINUTE_SECOND), time.wHour, time.wMinute, time.wSecond);
    m_info_list.SetItemText(13, 1, temp);
}

MIB_IFROW& CNetworkInfoDlg::GetConnectIfTable(int connection_index)
{
    static MIB_IFROW nouse{};
    if (connection_index >= 0 && connection_index < static_cast<int>(m_connections.size()))
    {
        int index = m_connections[connection_index].index;
        if (index >= 0 && index < static_cast<int>(m_if_rows.size()))
            return m_if_rows[index];
    }
    return nouse;
}

NetWorkConection CNetworkInfoDlg::GetConnection(int connection_index)
{
    if (connection_index >= 0 && connection_index < static_cast<int>(m_connections.size()))
        return m_connections[connection_index];
    else
        return NetWorkConection();
}

bool CNetworkInfoDlg::IsConnectionActive(int connection_index)
{
    if (connection_index < 0 || connection_index >= static_cast<int>(m_connections.size()))
        return false;
    DWORD status = GetConnectIfTable(connection_index).dwOperStatus;
    return status == IF_OPER_STATUS_OPERATIONAL || status == IF_OPER_STATUS_CONNECTED;
}

int CNetworkInfoDlg::FindBusiestActiveConnection()
{
    int result = -1;
    unsigned __int64 max_bytes{};
    for (int i = 0; i < static_cast<int>(m_connections.size()); i++)
    {
        if (!IsConnectionActive(i))
            continue;
        unsigned __int64 in_bytes{}, out_bytes{};
        CAdapterCommon::GetInterfaceOctets(GetConnectIfTable(i), in_bytes, out_bytes);
        if (result < 0 || in_bytes + out_bytes > max_bytes)
        {
            result = i;
            max_bytes = in_bytes + out_bytes;
        }
    }
    return result;
}

namespace
{
    struct GetIpThreadParam
    {
        HWND hwnd;
        std::shared_ptr<void> state;
    };
}

UINT CNetworkInfoDlg::GetInternetIPThreadFunc(LPVOID lpParam)
{
    //Luồng làm việc chỉ dùng HWND và trạng thái dùng chung, không truy cập đối tượng hộp thoại hay điều khiển
    //(đối tượng hộp thoại có thể đã bị hủy; điều khiển chỉ được thao tác trên luồng giao diện).
    std::unique_ptr<GetIpThreadParam> param(static_cast<GetIpThreadParam*>(lpParam));
    auto state = std::static_pointer_cast<PublicIpState>(param->state);
    CCommon::SetThreadLanguage(theApp.m_general_data.language.language_id);		//Thiết lập ngôn ngữ cho luồng

    for (int i = 0; i < 2; i++)     //0: IPv4, 1: IPv6
    {
        wstring ip_address, ip_location;
        CCommon::PublicIpResult result = CCommon::GetInternetIp2(ip_address, ip_location, i == 1);
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->result[i] = result;
            state->ip_address[i] = ip_address;
            state->ip_location[i] = ip_location;
        }
        //Gửi kết quả về luồng giao diện. Hộp thoại đã đóng thì PostMessage thất bại và luồng kết thúc.
        if (!::IsWindow(param->hwnd) || !::PostMessage(param->hwnd, WM_PUBLIC_IP_ACQUIRED, i, 0))
            break;
    }
    return 0;
}

LRESULT CNetworkInfoDlg::OnPublicIpAcquired(WPARAM wParam, LPARAM lParam)
{
    int i = static_cast<int>(wParam);
    if (i < 0 || i > 1 || m_ip_state == nullptr)
        return 0;
    CCommon::PublicIpResult result;
    wstring ip_address, ip_location;
    {
        std::lock_guard<std::mutex> lock(m_ip_state->mutex);
        result = m_ip_state->result[i];
        ip_address = m_ip_state->ip_address[i];
        ip_location = m_ip_state->ip_location[i];
    }
    CString info;
    if (result == CCommon::PublicIpResult::OK && !ip_address.empty())
    {
        if (ip_location.empty())
            info = ip_address.c_str();
        else
            info.Format(_T("%s (%s)"), ip_address.c_str(), ip_location.c_str());
    }
    else if (result == CCommon::PublicIpResult::NO_CONNECTION)
    {
        info = CCommon::LoadText(IDS_NO_IPV6_CONNECTION);
    }
    else
    {
        info = CCommon::LoadText(IDS_GET_FAILED);
    }
    m_info_list.SetItemText(14 + i, 1, info);
    if (i == 1)
        m_ip_acquiring = false;     //Đã xong cả hai; cho phép nhấp đúp để lấy lại
    return 0;
}

CString CNetworkInfoDlg::GetDialogName() const
{
    return _T("NetworkInfoDlg");
}

void CNetworkInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_INFO_LIST1, m_info_list);
}


BEGIN_MESSAGE_MAP(CNetworkInfoDlg, CBaseDialog)
    ON_COMMAND(ID_COPY_TEXT, &CNetworkInfoDlg::OnCopyText)
    ON_NOTIFY(NM_RCLICK, IDC_INFO_LIST1, &CNetworkInfoDlg::OnNMRClickInfoList1)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_PREVIOUS_BUTTON, &CNetworkInfoDlg::OnBnClickedPreviousButton)
    ON_BN_CLICKED(IDC_NEXT_BUTTON, &CNetworkInfoDlg::OnBnClickedNextButton)
    ON_WM_TIMER()
    ON_WM_MOUSEWHEEL()
    ON_NOTIFY(NM_DBLCLK, IDC_INFO_LIST1, &CNetworkInfoDlg::OnNMDblclkInfoList1)
    ON_MESSAGE(WM_PUBLIC_IP_ACQUIRED, &CNetworkInfoDlg::OnPublicIpAcquired)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_INFO_LIST1, &CNetworkInfoDlg::OnNMCustomdrawInfoList1)
END_MESSAGE_MAP()


// Trình xử lý thông điệp của CNetworkInfoDlg


BOOL CNetworkInfoDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây
    SetIcon(theApp.GetMenuIcon(IDI_INFO), FALSE);		// Thiết lập biểu tượng nhỏ

    //Lấy lại địa chỉ IP
    CAdapterCommon::RefreshIpAddress(m_connections);

    //Khởi tạo điều khiển danh sách
    CRect rect;
    m_info_list.GetClientRect(rect);
    m_info_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() / 4;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_info_list.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//Chèn cột thứ 0
    m_info_list.InsertColumn(1, CCommon::LoadText(IDS_VALUE), LVCFMT_LEFT, width1);		//Chèn cột thứ 1

    //Chèn hàng vào danh sách
    m_info_list.InsertItem(0, CCommon::LoadText(IDS_INTERFACE_NAME));
    m_info_list.InsertItem(1, CCommon::LoadText(IDS_INTERFACE_DESCRIPTION));
    m_info_list.InsertItem(2, CCommon::LoadText(IDS_CONNECTION_TYPE));
    m_info_list.InsertItem(3, CCommon::LoadText(IDS_SPEED));
    m_info_list.InsertItem(4, CCommon::LoadText(IDS_ADAPTER_PHYSICAL_ADDRESS));
    m_info_list.InsertItem(5, CCommon::LoadText(IDS_IP_ADDRESS));
    m_info_list.InsertItem(6, CCommon::LoadText(IDS_SUBNET_MASK));
    m_info_list.InsertItem(7, CCommon::LoadText(IDS_DEFAULT_GATEWAY));
    m_info_list.InsertItem(8, CCommon::LoadText(IDS_OPERATIONAL_STATUS));
    m_info_list.InsertItem(9, CCommon::LoadText(IDS_BYTES_RECEIVED));
    m_info_list.InsertItem(10, CCommon::LoadText(IDS_BYTES_SENT));
    m_info_list.InsertItem(11, CCommon::LoadText(IDS_BYTES_RECEIVED_SINCE_START));
    m_info_list.InsertItem(12, CCommon::LoadText(IDS_BYTES_SENT_SINCE_START));
    m_info_list.InsertItem(13, CCommon::LoadText(IDS_PROGRAM_ELAPSED_TIME));
    m_info_list.InsertItem(14, CCommon::LoadText(IDS_INTERNET_IP_ADDRESS, _T(" (ipv4)")));
    m_info_list.InsertItem(15, CCommon::LoadText(IDS_INTERNET_IP_ADDRESS, _T(" (ipv6)")));
    //if (theApp.m_cfg_data.m_show_internet_ip)
    //{
    //	m_info_list.SetItemText(14, 1, CCommon::LoadText(IDS_ACQUIRING, _T("...")));
    //	m_info_list.SetItemText(15, 1, CCommon::LoadText(IDS_ACQUIRING, _T("...")));
    //}
    //else
    //{
    m_info_list.SetItemText(14, 1, CCommon::LoadText(IDS_DOUBLE_CLICK_TO_ACQUIRE));
    m_info_list.SetItemText(15, 1, CCommon::LoadText(IDS_DOUBLE_CLICK_TO_ACQUIRE));
    //}

    //Trang mở đầu: khi đang “Chọn tất cả” hoặc kết nối đang chọn không hoạt động (ví dụ card bị tắt),
    //mở trang của kết nối đang hoạt động có lưu lượng lớn nhất thay vì một trang toàn số 0.
    //m_current_connection vẫn giữ kết nối đang giám sát để in đậm chỉ báo khi lật tới trang đó.
    if (theApp.m_cfg_data.m_select_all || !IsConnectionActive(m_connection_selected))
    {
        int active_connection = FindBusiestActiveConnection();
        if (active_connection >= 0)
            m_connection_selected = active_connection;
    }

    //Hiển thị thông tin trong danh sách
    LOGFONT lf{};
    GetFont()->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    m_font_bold.CreateFontIndirect(&lf);
    ShowInfo();
    GetProgramElapsedTime();

    //CCommon::GetInternetIp();
    //if (theApp.m_cfg_data.m_show_internet_ip)
 //       m_pGetIPThread = AfxBeginThread(GetInternetIPThreadFunc, this);		//Khởi động luồng lấy IP công cộng

    //SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//Bỏ ghim trên cùng
    m_info_list.GetToolTips()->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    CCommon::LoadMenuResource(m_menu, IDR_INFO_MENU); //Nạp menu chuột phải

    SetTimer(CONNECTION_DETAIL_TIMER, 1000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}


void CNetworkInfoDlg::OnCopyText()
{
    // TODO: Thêm mã xử lý lệnh tại đây
    if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
        MessageBox(CCommon::LoadText(IDS_COPY_TO_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


void CNetworkInfoDlg::OnNMRClickInfoList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây

    //Lấy văn bản tại vị trí nhấp chuột
    int item, sub_item;
    item = pNMItemActivate->iItem;
    sub_item = pNMItemActivate->iSubItem;
    m_selected_string = m_info_list.GetItemText(item, sub_item);

    //Hiện menu chuột phải
    CMenu* pContextMenu = m_menu.GetSubMenu(0);	//Lấy menu bật lên đầu tiên
    CPoint point1;	//Khai báo vị trí dùng để xác định vị trí con trỏ chuột
    GetCursorPos(&point1);	//Lấy vị trí con trỏ chuột hiện tại để menu đi theo con trỏ
    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //Hiển thị menu bật lên tại vị trí chỉ định

    *pResult = 0;
}


void CNetworkInfoDlg::OnClose()
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    //Khi đóng hộp thoại thì buộc kết thúc luồng lấy địa chỉ IP
    //if(theApp.m_cfg_data.m_show_internet_ip)
 //       TerminateThread(m_pGetIPThread->m_hThread, 0);
    CBaseDialog::OnClose();
}


void CNetworkInfoDlg::OnBnClickedPreviousButton()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    if (m_connections.size() > 1 && m_connection_selected > 0)
    {
        m_connection_selected--;
        ShowInfo();
    }
}


void CNetworkInfoDlg::OnBnClickedNextButton()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    if (m_connections.size() > 1 && m_connection_selected < static_cast<int>(m_connections.size()) - 1)
    {
        m_connection_selected++;
        ShowInfo();
    }
}


BOOL CNetworkInfoDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_LEFT)
        {
            OnBnClickedPreviousButton();
            return TRUE;
        }
        if (pMsg->wParam == VK_RIGHT)
        {
            OnBnClickedNextButton();
            return TRUE;
        }
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
}



void CNetworkInfoDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    if (nIDEvent == CONNECTION_DETAIL_TIMER)
    {
        GetProgramElapsedTime();
        ShowTrafficInfo();      //Cập nhật số byte đã nhận/gửi theo thời gian thực
    }

    CBaseDialog::OnTimer(nIDEvent);
}

BOOL CNetworkInfoDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
    //Lật trang bằng con lăn chuột
    if (zDelta > 0)
    {
        OnBnClickedPreviousButton();
    }
    if (zDelta < 0)
    {
        OnBnClickedNextButton();
    }

    return CBaseDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CNetworkInfoDlg::OnNMDblclkInfoList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    if (!m_ip_acquiring && (pNMItemActivate->iItem == 14 || pNMItemActivate->iItem == 15))		//Khi nhấp đúp vào dòng địa chỉ IP
    {
        m_ip_state = std::make_shared<PublicIpState>();
        auto* param = new GetIpThreadParam{ GetSafeHwnd(), m_ip_state };
        if (AfxBeginThread(GetInternetIPThreadFunc, param) != nullptr)
        {
            m_ip_acquiring = true;
            m_info_list.SetItemText(14, 1, CCommon::LoadText(IDS_ACQUIRING, _T("...")));
            m_info_list.SetItemText(15, 1, CCommon::LoadText(IDS_ACQUIRING, _T("...")));
        }
        else
        {
            delete param;
        }
    }
    *pResult = 0;
}

void CNetworkInfoDlg::OnNMCustomdrawInfoList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    //Làm mờ các giá trị phụ thuộc card mạng (hàng 0-12, trừ hàng trạng thái) khi kết nối không hoạt động
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    *pResult = CDRF_DODEFAULT;
    switch (pLVCD->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;
    case CDDS_ITEMPREPAINT:
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
        break;
    case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
    {
        int row = static_cast<int>(pLVCD->nmcd.dwItemSpec);
        bool dim = pLVCD->iSubItem == 1 && row <= 12 && row != 8 && !IsConnectionActive(m_connection_selected);
        pLVCD->clrText = dim ? GetSysColor(COLOR_GRAYTEXT) : CLR_DEFAULT;     //Phải đặt lại cho mọi ô vì màu được giữ giữa các ô
        break;
    }
    default:
        break;
    }
}
