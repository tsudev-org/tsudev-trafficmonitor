// DisplayTextSettingDlg.cpp: tệp triển khai
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "DisplayTextSettingDlg.h"
#include "TrafficMonitorDlg.h"
#include "SkinManager.h"


// Hộp thoại CDisplayTextSettingDlg

IMPLEMENT_DYNAMIC(CDisplayTextSettingDlg, CBaseDialog)

CDisplayTextSettingDlg::CDisplayTextSettingDlg(DispStrings& display_texts, bool main_window_text, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_DISPLAY_TEXT_SETTINGS_DIALOG, pParent), m_display_texts(display_texts), m_main_window_text(main_window_text)
{

}

CDisplayTextSettingDlg::~CDisplayTextSettingDlg()
{
}

void CDisplayTextSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


CString CDisplayTextSettingDlg::GetDialogName() const
{
    return _T("DisplayTextSettingDlg");
}

BEGIN_MESSAGE_MAP(CDisplayTextSettingDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_BUTTON, &CDisplayTextSettingDlg::OnBnClickedRestoreDefaultButton)
    ON_COMMAND(ID_RESTORE_DEFAULT, &CDisplayTextSettingDlg::OnRestoreDefault)
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDisplayTextSettingDlg::OnNMRClickList1)
    ON_WM_INITMENU()
    ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDisplayTextSettingDlg::OnNMClickList1)
END_MESSAGE_MAP()


// Trình xử lý thông điệp của CDisplayTextSettingDlg


BOOL CDisplayTextSettingDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây

    SetIcon(theApp.GetMenuIcon(IDI_ITEM), FALSE);		// Thiết lập biểu tượng nhỏ

    //Khởi tạo điều khiển danh sách
    CRect rect;
    m_list_ctrl.GetClientRect(rect);
    m_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() / 2;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//Chèn cột thứ 0
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_VALUE), LVCFMT_LEFT, width1);		//Chèn cột thứ 1

    //Nếu là cửa sổ chính thì xóa các hàng không có trong skin hiện tại
    if (m_main_window_text)
    {
        std::set<CommonDisplayItem> all_skin_items;
        CTrafficMonitorDlg::Instance()->GetCurSkin().GetSkinDisplayItems(all_skin_items);

        DispStrings temp = m_display_texts;
        m_display_texts = DispStrings();
        for (const auto& display_item : all_skin_items)
        {
            m_display_texts.Get(display_item) = temp.GetConst(display_item);
        }
    }

    //Chèn hàng vào danh sách
    for (auto iter = m_display_texts.GetAllItems().begin(); iter != m_display_texts.GetAllItems().end(); ++iter)
    {
        CString item_name = iter->first.GetItemName();
        if (!item_name.IsEmpty())
        {
            int index = m_list_ctrl.GetItemCount();
            m_list_ctrl.InsertItem(index, item_name);
            m_list_ctrl.SetItemText(index, 1, iter->second.c_str());
            m_list_ctrl.SetItemData(index, (DWORD_PTR)&(iter->first));
        }
    }

    m_list_ctrl.SetEditColMethod(CListCtrlEx::EC_SPECIFIED);        //Cho phép chỉnh sửa danh sách
    m_list_ctrl.SetEditableCol({ 1 });                              //Thiết lập cột có thể chỉnh sửa

    CCommon::LoadMenuResource(m_menu, IDR_DISPLAY_ITEM_CONTEXT_MENU); //Nạp menu chuột phải

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}

CommonDisplayItem CDisplayTextSettingDlg::GetDisplayItem(int row)
{
    CommonDisplayItem* item = (CommonDisplayItem*)m_list_ctrl.GetItemData(row);
    return *item;
}

void CDisplayTextSettingDlg::OnOK()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây

    int item_count = m_list_ctrl.GetItemCount();
    for (int i{}; i < item_count; i++)
    {
        CommonDisplayItem display_item = GetDisplayItem(i);
        m_display_texts.Get(display_item) = m_list_ctrl.GetItemText(i, 1).GetString();
    }

    CBaseDialog::OnOK();
}



void CDisplayTextSettingDlg::OnBnClickedRestoreDefaultButton()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    int item_count = m_list_ctrl.GetItemCount();
    CTrafficMonitorDlg* pMainWnd = CTrafficMonitorDlg::Instance();
    if (m_main_window_text && pMainWnd != nullptr)
    {
        //Khi cửa sổ chính khôi phục văn bản hiển thị mặc định thì lấy từ skin
        SkinSettingData skin_setting_data;
        CSkinManager::SkinSettingDataFronSkin(skin_setting_data, pMainWnd->GetCurSkin());
        for (int i{}; i < item_count; i++)
        {
            CommonDisplayItem display_item = GetDisplayItem(i);
            std::wstring default_text = skin_setting_data.disp_str.GetConst(display_item);
            m_list_ctrl.SetItemText(i, 1, default_text.c_str());
        }
    }
    else
    {
        for (int i{}; i < item_count; i++)
        {
            CommonDisplayItem display_item = GetDisplayItem(i);
            std::wstring default_text = display_item.DefaultString(m_main_window_text);
            m_list_ctrl.SetItemText(i, 1, default_text.c_str());
        }
    }
}

void CDisplayTextSettingDlg::OnRestoreDefault()
{
    if (m_item_selected >= 0)
    {
        CTrafficMonitorDlg* pMainWnd = CTrafficMonitorDlg::Instance();
        if (m_main_window_text && pMainWnd != nullptr)
        {
            //Khi cửa sổ chính khôi phục văn bản hiển thị mặc định thì lấy từ skin
            SkinSettingData skin_setting_data;
            CSkinManager::SkinSettingDataFronSkin(skin_setting_data, pMainWnd->GetCurSkin());
            CommonDisplayItem display_item = GetDisplayItem(m_item_selected);
            std::wstring default_text = skin_setting_data.disp_str.GetConst(display_item);
            m_list_ctrl.SetItemText(m_item_selected, 1, default_text.c_str());
        }
        else
        {
            CommonDisplayItem display_item = GetDisplayItem(m_item_selected);
            std::wstring default_text = display_item.DefaultString(m_main_window_text);
            m_list_ctrl.SetItemText(m_item_selected, 1, default_text.c_str());
        }
    }
}

void CDisplayTextSettingDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    m_item_selected = pNMItemActivate->iItem;
    //Hiện menu chuột phải
    CMenu* pContextMenu = m_menu.GetSubMenu(0);	//Lấy menu bật lên đầu tiên
    CPoint point1;	//Khai báo vị trí dùng để xác định vị trí con trỏ chuột
    GetCursorPos(&point1);	//Lấy vị trí con trỏ chuột hiện tại để menu đi theo con trỏ
    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //Hiển thị menu bật lên tại vị trí chỉ định

    *pResult = 0;
}

void CDisplayTextSettingDlg::OnInitMenu(CMenu* pMenu)
{
    CBaseDialog::OnInitMenu(pMenu);

    bool selected_enable{ m_item_selected >= 0 };
    pMenu->EnableMenuItem(ID_RESTORE_DEFAULT, MF_BYCOMMAND | (selected_enable ? MF_ENABLED : MF_GRAYED));
}

void CDisplayTextSettingDlg::OnNMClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    m_item_selected = pNMItemActivate->iItem;
    *pResult = 0;
}
