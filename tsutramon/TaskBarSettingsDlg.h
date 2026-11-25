#pragma once
#include "ColorStatic.h"
#include "afxwin.h"
#include "SpinEdit.h"
#include "TabDlg.h"
#include "TaskbarColorDlg.h"
#include "ComboBox2.h"

// Hộp thoại CTaskBarSettingsDlg

class CTaskBarSettingsDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CTaskBarSettingsDlg)

public:
    CTaskBarSettingsDlg(CWnd* pParent = NULL);   // Hàm khởi tạo chuẩn
    virtual ~CTaskBarSettingsDlg();

    bool IsStyleModified();

    //Nếu đã bật tính năng tự động thích ứng chế độ sáng/tối của Windows 10 thì lưu cấu hình hiện tại vào mẫu màu tương ứng
    void SaveColorSettingToDefaultStyle();

    //Dữ liệu tùy chọn
    TaskBarSettingData m_data;

	CWinVersionHelper m_win_version;        //Phiên bản Windows hiện tại

    // Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TASKBAR_SETTINGS_DIALOG };
#endif

protected:
    //Biến điều khiển
    CColorStatic m_text_color_static;
    CColorStatic m_back_color_static;
    CColorStatic m_trans_color_static;
    CColorStatic m_status_bar_color_static;
    CToolTipCtrl m_toolTip;
    CComboBox2 m_unit_combo;
    CButton m_hide_unit_chk;
    CSpinEdit m_font_size_edit;
    CComboBox2 m_double_click_combo;
    CComboBox2 m_digit_number_combo;
    CMenu m_default_style_menu;
    CMenu m_modify_default_style_menu;
    CButton m_background_transparent_chk;
    CButton m_atuo_adapt_light_theme_chk;
    CButton m_auto_set_back_color_chk;
    CComboBox2 m_memory_display_combo;
    CSpinEdit m_item_space_edit;
    CSpinEdit m_vertical_margin_edit;
    CSpinEdit m_net_speed_figure_max_val_edit;
    CComboBox2 m_net_speed_figure_max_val_unit_combo;
    CComboBox2 m_displays_combo;

    bool m_style_modified{};

protected:
    void DrawStaticColor();
    void IniUnitCombo();

    void ApplyDefaultStyle(int index);      //Áp dụng một mẫu màu

public:
    void ModifyDefaultStyle(int index);     //Lưu thiết lập màu hiện tại vào một mẫu màu

protected:
    void EnableControl();
    virtual void SetControlMouseWheelEnable(bool enable) override;
    virtual bool InitializeControls() override;

    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedSetFontButton1();
    afx_msg void OnBnClickedTaskbarWndOnLeftCheck();
    afx_msg void OnBnClickedSpeedShortModeCheck();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnCbnSelchangeUnitCombo();
    afx_msg void OnBnClickedHideUnitCheck();
    virtual void OnOK();
    afx_msg void OnBnClickedValueRightAlignCheck();
    afx_msg void OnBnClickedHidePercentageCheck();
protected:
    afx_msg LRESULT OnStaticClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedSpecifyEachItemColorCheck();
    afx_msg void OnCbnSelchangeDoubleClickCombo();
    afx_msg void OnBnClickedHorizontalArrangeCheck();
    afx_msg void OnBnClickedShowStatusBarCheck();
    afx_msg void OnBnClickedSeparateValueUnitCheck();
    afx_msg void OnBnClickedUnitByteRadio();
    afx_msg void OnBnClickedUnitBitRadio();
    afx_msg void OnBnClickedShowToolTipChk();
    afx_msg void OnBnClickedDefaultStyleButton();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedBrowseButton();
    afx_msg void OnBnClickedCMGraphBarRadio();
    afx_msg void OnBnClickedCMGraphPLOTRadio();
    afx_msg void OnBnClickedBackgroundTransparentCheck();
    afx_msg void OnBnClickedAutoAdaptSettingsButton();
    afx_msg void OnBnClickedAutoAdaptLightThemeCheck();
    afx_msg void OnBnClickedAutoSetBackColorCheck();
    afx_msg void OnBnClickedDisplayTextSettingButton();
    afx_msg void OnCbnSelchangeMemoryDisplayCombo();
    afx_msg void OnBnClickedShowDashedBox();
    afx_msg void OnBnClickedSetOrderButton();
    afx_msg void OnEnChangeItemSpaceEdit();
    afx_msg void OnEnChangeVerticalMarginEdit();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedShowNetSpeedFigureCheck();
    afx_msg void OnCbnSelchangeNetSpeedFigureMaxValueUnitCombo();
    afx_msg void OnEnChangeNetSpeedFigureMaxValueEdit();
    afx_msg void OnBnClickedGdiRadio();
    afx_msg void OnBnClickedD2dRadio();
    afx_msg void OnBnClickedEnableColorEmojiCheck();
    afx_msg void OnCbnSelchangeDigitNumberCombo();
    afx_msg void OnBnClickedWin11SettingsButton();
    afx_msg void OnBnClickedTaskbarWndInSecondaryDisplayCheck();
    afx_msg void OnCbnSelchangeDisplayToShowTaskbarWndCombo();
    afx_msg void OnBnClickedUsageGraphFollowSystemCheck();
    afx_msg void OnEnChangeFontSizeEdit1();
};
