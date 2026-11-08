#pragma once
#include "HistoryTrafficListCtrl.h"
#include "TabDlg.h"

// Hộp thoại CHistoryTrafficListDlg

class CHistoryTrafficListDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CHistoryTrafficListDlg)

public:
	CHistoryTrafficListDlg(deque<HistoryTraffic>& history_traffics, CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
	virtual ~CHistoryTrafficListDlg();

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HISTORY_TRAFFIC_LIST_DIALOG };
#endif

protected:
	CHistoryTrafficListCtrl m_history_list;
	CMenu m_menu;
    CComboBox m_view_type_combo;
    CComboBox m_view_scale_combo;

	deque<HistoryTraffic>& m_history_traffics;

    struct ListRowData
    {
        CString str;
        unsigned __int64 up_kBytes{};
        unsigned __int64 down_kBytes{};
        bool mixed{};
    };

protected:
    bool CalculateColumeWidth(std::vector<int>& widths);
    void AddListRow(const ListRowData& data, unsigned __int64 max_traffic);          //Thêm một hàng dữ liệu vào danh sách
    void ShowListData();

	virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnUseLinearScale();
	afx_msg void OnUseLogScale();

	virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCbnSelchangeViewTypeCombo();
    afx_msg void OnCbnSelchangeViewScaleCombo();
};
