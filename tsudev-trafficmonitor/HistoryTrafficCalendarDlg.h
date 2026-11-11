#pragma once
#include "TabDlg.h"
#include "CalendarHelper.h"
#include "DrawCommon.h"
#include "afxwin.h"

// Hộp thoại CHistoryTrafficCalendarDlg

class CHistoryTrafficCalendarDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CHistoryTrafficCalendarDlg)

public:
	CHistoryTrafficCalendarDlg(deque<HistoryTraffic>& history_traffics, CWnd* pParent = nullptr);   // Hàm khởi tạo chuẩn
	virtual ~CHistoryTrafficCalendarDlg();

// Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HISTORY_TRAFFIC_CALENDAR_DIALOG };
#endif

protected:
	deque<HistoryTraffic>& m_history_traffics;
	DayTraffic m_calendar[CALENDAR_HEIGHT][CALENDAR_WIDTH];
	int m_year;
	int m_month;
	__int64 m_month_total_upload;		//Tổng lưu lượng tải lên của tháng hiện tại
	__int64 m_month_total_download;		//Tổng lưu lượng tải xuống của tháng hiện tại

	int m_year_max;
	int m_year_min;

	CComboBox m_year_combo;
	CComboBox m_month_combo;
	CToolTipCtrl m_tool_tips;
	CMenu m_menu;

	CRect m_draw_rect;
	const int m_start_x{ theApp.DPI(10) };		//Tọa độ X bắt đầu vẽ
	const int m_start_y{ theApp.DPI(44) };		//Tọa độ Y bắt đầu vẽ

	void SetDayTraffic();
	void MonthSelectChanged();
	void CalculateMonthTotalTraffic();		//Tính tổng lưu lượng của tháng hiện tại
	void SetComboSel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

	bool IsWeekend(int index);		//Kiểm tra có phải cuối tuần không, index nhận giá trị 0~6
	CString GetWeekdayString(int index);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeYearCombo();
	afx_msg void OnCbnSelchangeMonthCombo();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedPreviousButton();
	afx_msg void OnBnClickedNextButton();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedMenuButton();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnFirstDayOfWeekSunday();
	afx_msg void OnFirstDayOfWeekMonday();
	afx_msg void OnCalendarJumpToToday();
};
