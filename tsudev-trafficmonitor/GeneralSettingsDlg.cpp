// GeneralSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TsudevTrafficMonitor.h"
#include "TsudevTrafficMonitorDlg.h"
#include "GeneralSettingsDlg.h"
#include "PluginManagerDlg.h"
#include "SelectConnectionsDlg.h"


// CGeneralSettingsDlg dialog

static const int MONITOR_SPAN_STEP = 100;

IMPLEMENT_DYNAMIC(CGeneralSettingsDlg, CTabDlg)

CGeneralSettingsDlg::CGeneralSettingsDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_GENERAL_SETTINGS_DIALOG, pParent)
{

}

CGeneralSettingsDlg::~CGeneralSettingsDlg()
{
}

void CGeneralSettingsDlg::CheckTaskbarDisplayItem()
{
    //Nếu đã tắt giám sát một phần cứng trong Tùy chọn thì không hiển thị các mục giám sát nhiệt độ tương ứng
    if (!theApp.m_general_data.IsHardwareEnable(HI_CPU))
    {
        theApp.m_taskbar_data.display_item.Remove(TDI_CPU_TEMP);
    }
    if (!theApp.m_general_data.IsHardwareEnable(HI_GPU))
    {
        theApp.m_taskbar_data.display_item.Remove(TDI_GPU_TEMP);
    }
    if (!theApp.m_general_data.IsHardwareEnable(HI_HDD))
    {
        theApp.m_taskbar_data.display_item.Remove(TDI_HDD_TEMP);
    }
    if (!theApp.m_general_data.IsHardwareEnable(HI_MBD))
        theApp.m_taskbar_data.display_item.Remove(TDI_MAIN_BOARD_TEMP);
}

void CGeneralSettingsDlg::SetControlMouseWheelEnable(bool enable)
{
    m_traffic_tip_combo.SetMouseWheelEnable(enable);
    m_language_combo.SetMouseWheelEnable(enable);
    m_traffic_tip_edit.SetMouseWheelEnable(enable);
    m_memory_tip_edit.SetMouseWheelEnable(enable);
    m_monitor_span_edit.SetMouseWheelEnable(enable);
    m_cpu_temp_tip_edit.SetMouseWheelEnable(enable);
    m_gpu_temp_tip_edit.SetMouseWheelEnable(enable);
    m_hdd_temp_tip_edit.SetMouseWheelEnable(enable);
    m_mbd_temp_tip_edit.SetMouseWheelEnable(enable);
    m_hard_disk_combo.SetMouseWheelEnable(enable);
    m_select_cpu_combo.SetMouseWheelEnable(enable);
}

void CGeneralSettingsDlg::OnSettingsApplied()
{
    //Khi thiết lập được áp dụng thì đặt lại giá trị xxxx_ori
    m_monitor_time_span_ori = m_data.monitor_time_span;
    m_update_source_ori = m_data.update_source;
}

bool CGeneralSettingsDlg::InitializeControls()
{
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_CHECK_NOW_BUTTON, CtrlTextInfo::W16 }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_UPDATE_SORUCE_STATIC },
        { CtrlTextInfo::L3, IDC_GITHUB_RADIO },
        { CtrlTextInfo::L2, IDC_GITEE_RADIO }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_RESET_AUTO_RUN_BUTTON, CtrlTextInfo::W16 }
        });

    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_LANGUAGE_STATIC },
        { CtrlTextInfo::L3, IDC_LANGUAGE_COMBO }
        });

    //Điều chỉnh vị trí ngang của hàng điều khiển “Lưu lượng hôm nay đã đạt”
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_TODAY_TRAFFIC_TIP_CHECK, CtrlTextInfo::W24 },
        { CtrlTextInfo::L3, IDC_TODAY_TRAFFIC_TIP_EDIT },
        { CtrlTextInfo::L2, IDC_TODAY_TRAFFIC_TIP_COMBO },
        { CtrlTextInfo::L1, IDC_TODAY_TRAFFIC_BACK_STATIC}
        });
    //Điều chỉnh vị trí ngang của các hàng điều khiển “Mức sử dụng bộ nhớ đã đạt”, “Nhiệt độ đã đạt”
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_MEMORY_USAGE_TIP_CHECK, CtrlTextInfo::W24 },
        { CtrlTextInfo::L3, IDC_MEMORY_USAGE_TIP_EDIT },
        { CtrlTextInfo::L2, IDC_MEMORY_USAGE_BACK_STATIC },
        { CtrlTextInfo::L4, IDC_CPU_TEMP_TIP_CHECK, CtrlTextInfo::W24 },
        { CtrlTextInfo::L3, IDC_CPU_TEMP_TIP_EDIT },
        { CtrlTextInfo::L2, IDC_CPU_TEMP_STATIC },
        { CtrlTextInfo::L4, IDC_GPU_TEMP_TIP_CHECK, CtrlTextInfo::W24 },
        { CtrlTextInfo::L3, IDC_GPU_TEMP_TIP_EDIT },
        { CtrlTextInfo::L2, IDC_GPU_TEMP_STATIC },
        { CtrlTextInfo::L4, IDC_HDD_TEMP_TIP_CHECK, CtrlTextInfo::W24 },
        { CtrlTextInfo::L3, IDC_HDD_TIP_EDIT },
        { CtrlTextInfo::L2, IDC_HDD_STATIC },
        { CtrlTextInfo::L4, IDC_MBD_TEMP_TIP_CHECK, CtrlTextInfo::W24 },
        { CtrlTextInfo::L3, IDC_MBD_TEMP_TIP_EDIT },
        { CtrlTextInfo::L2, IDC_MBD_TEMP_STATIC },
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_SELECT_HDD_STATIC },
        { CtrlTextInfo::C0, IDC_SELECT_HARD_DISK_COMBO },
        { CtrlTextInfo::L1, IDC_SELECT_CPU_STATIC },
        { CtrlTextInfo::C0, IDC_SELECT_CPU_COMBO },
    });

    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_SELECT_CONNECTIONS_BUTTON, CtrlTextInfo::W32 }
    });
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_MONITOR_INTERVAL_STATIC },
        { CtrlTextInfo::L3, IDC_MONITOR_SPAN_EDIT },
        { CtrlTextInfo::L2, IDC_MILLISECONDS_STATIC },
        { CtrlTextInfo::L1, IDC_RESTORE_DEFAULT_TIME_SPAN_BUTTON, CtrlTextInfo::W16 }
    });
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_PLUGIN_MANAGE_BUTTON, CtrlTextInfo::W32 }
    });

    return true;
}

bool CGeneralSettingsDlg::ShowHardwareMonitorWarning()
{
    //Nếu đã có mục giám sát phần cứng được chọn thì không hiện thông báo nữa
    if (m_data.hardware_monitor_item != 0)
        return true;

    if (SHMessageBoxCheck(m_hWnd, CCommon::LoadText(IDS_HARDWARE_MONITOR_WARNING), APP_NAME, MB_OKCANCEL | MB_ICONWARNING, IDOK, _T("{B8A281A7-76DF-4F0F-BF6A-1A394EF8BAD5}")) == IDOK)
        return true;

    return false;
}

void CGeneralSettingsDlg::AddOrUpdateAutoRunTooltip(bool add)
{
    if (m_data.auto_run)
    {
        CString str_tool_tip;
        if (m_data.auto_run_by_task_scheduler)
            str_tool_tip = CCommon::LoadText(IDS_AUTO_RUN_METHOD_TASK_SCHEDULE);
        else
            str_tool_tip = CCommon::LoadText(IDS_AUTO_RUN_METHOD_REGESTRY);
        if (!m_auto_run_path.empty())
        {
            str_tool_tip += _T("\r\n");
            str_tool_tip += CCommon::LoadText(IDS_PATH, _T(": "));
            str_tool_tip += m_auto_run_path.c_str();
        }
        if (add)
            m_toolTip.AddTool(GetDlgItem(IDC_AUTO_RUN_CHECK), str_tool_tip);
        else
            m_toolTip.UpdateTipText(str_tool_tip, GetDlgItem(IDC_AUTO_RUN_CHECK));
    }
    else
    {
        if (add)
            m_toolTip.AddTool(GetDlgItem(IDC_AUTO_RUN_CHECK), CString());
        else
            m_toolTip.UpdateTipText(CString(), GetDlgItem(IDC_AUTO_RUN_CHECK));
    }
}

bool CGeneralSettingsDlg::IsMonitorTimeSpanModified() const
{
    return m_data.monitor_time_span != m_monitor_time_span_ori;
}

void CGeneralSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TODAY_TRAFFIC_TIP_EDIT, m_traffic_tip_edit);
    DDX_Control(pDX, IDC_TODAY_TRAFFIC_TIP_COMBO, m_traffic_tip_combo);
    DDX_Control(pDX, IDC_MEMORY_USAGE_TIP_EDIT, m_memory_tip_edit);
    DDX_Control(pDX, IDC_LANGUAGE_COMBO, m_language_combo);
    DDX_Control(pDX, IDC_MONITOR_SPAN_EDIT, m_monitor_span_edit);
    DDX_Control(pDX, IDC_CPU_TEMP_TIP_EDIT, m_cpu_temp_tip_edit);
    DDX_Control(pDX, IDC_GPU_TEMP_TIP_EDIT, m_gpu_temp_tip_edit);
    DDX_Control(pDX, IDC_HDD_TIP_EDIT, m_hdd_temp_tip_edit);
    DDX_Control(pDX, IDC_MBD_TEMP_TIP_EDIT, m_mbd_temp_tip_edit);
    DDX_Control(pDX, IDC_SELECT_HARD_DISK_COMBO, m_hard_disk_combo);
    DDX_Control(pDX, IDC_SELECT_CPU_COMBO, m_select_cpu_combo);
    DDX_Control(pDX, IDC_PLUGIN_MANAGE_BUTTON, m_plugin_manager_btn);
    DDX_Control(pDX, IDC_SELECT_CONNECTIONS_BUTTON, m_select_connection_btn);
}

void CGeneralSettingsDlg::SetControlEnable()
{
    m_traffic_tip_edit.EnableWindow(m_data.traffic_tip_enable);
    m_traffic_tip_combo.EnableWindow(m_data.traffic_tip_enable);
    m_memory_tip_edit.EnableWindow(m_data.memory_usage_tip.enable);
    m_cpu_temp_tip_edit.EnableWindow(m_data.cpu_temp_tip.enable);
    m_gpu_temp_tip_edit.EnableWindow(m_data.gpu_temp_tip.enable);
    m_hdd_temp_tip_edit.EnableWindow(m_data.hdd_temp_tip.enable);
    m_mbd_temp_tip_edit.EnableWindow(m_data.mainboard_temp_tip.enable);

    //m_hard_disk_combo.EnableWindow(m_data.IsHardwareEnable(HI_HDD));
    m_select_cpu_combo.EnableWindow(m_data.IsHardwareEnable(HI_CPU));

    EnableDlgCtrl(IDC_SELECT_CONNECTIONS_BUTTON, !m_data.show_all_interface);

    EnableDlgCtrl(IDC_RESET_AUTO_RUN_BUTTON, m_data.auto_run);
    EnableDlgCtrl(IDC_AUTO_RUN_METHOD_REGESTRY_RADIO, m_data.auto_run);
    EnableDlgCtrl(IDC_AUTO_RUN_METHOD_TASK_SCHEDULE_RADIO, m_data.auto_run);
}


BEGIN_MESSAGE_MAP(CGeneralSettingsDlg, CTabDlg)
    ON_BN_CLICKED(IDC_CHECK_NOW_BUTTON, &CGeneralSettingsDlg::OnBnClickedCheckNowButton)
    ON_BN_CLICKED(IDC_CHECK_UPDATE_CHECK, &CGeneralSettingsDlg::OnBnClickedCheckUpdateCheck)
    ON_BN_CLICKED(IDC_AUTO_RUN_CHECK, &CGeneralSettingsDlg::OnBnClickedAutoRunCheck)
    ON_BN_CLICKED(IDC_TODAY_TRAFFIC_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedTodayTrafficTipCheck)
    ON_BN_CLICKED(IDC_MEMORY_USAGE_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedMemoryUsageTipCheck)
    ON_BN_CLICKED(IDC_OPEN_CONFIG_PATH_BUTTON, &CGeneralSettingsDlg::OnBnClickedOpenConfigPathButton)
    ON_BN_CLICKED(IDC_SHOW_ALL_CONNECTION_CHECK, &CGeneralSettingsDlg::OnBnClickedShowAllConnectionCheck)
    ON_BN_CLICKED(IDC_USE_CPU_TIME_RADIO, &CGeneralSettingsDlg::OnBnClickedUseCpuTimeRadio)
    ON_BN_CLICKED(IDC_USE_PDH_RADIO, &CGeneralSettingsDlg::OnBnClickedUsePdhRadio)
    ON_EN_KILLFOCUS(IDC_MONITOR_SPAN_EDIT, &CGeneralSettingsDlg::OnEnKillfocusMonitorSpanEdit)
    ON_BN_CLICKED(IDC_CPU_TEMP_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedCpuTempTipCheck)
    ON_BN_CLICKED(IDC_GPU_TEMP_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedGpuTempTipCheck)
    ON_BN_CLICKED(IDC_HDD_TEMP_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedHddTempTipCheck)
    ON_BN_CLICKED(IDC_MBD_TEMP_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedMbdTempTipCheck)
    ON_BN_CLICKED(IDC_GITHUB_RADIO, &CGeneralSettingsDlg::OnBnClickedGithubRadio)
    ON_BN_CLICKED(IDC_GITEE_RADIO, &CGeneralSettingsDlg::OnBnClickedGiteeRadio)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_TIME_SPAN_BUTTON, &CGeneralSettingsDlg::OnBnClickedRestoreDefaultTimeSpanButton)
    ON_CBN_SELCHANGE(IDC_SELECT_HARD_DISK_COMBO, &CGeneralSettingsDlg::OnCbnSelchangeSelectHardDiskCombo)
    ON_BN_CLICKED(IDC_CPU_CHECK, &CGeneralSettingsDlg::OnBnClickedCpuCheck)
    ON_BN_CLICKED(IDC_GPU_CHECK, &CGeneralSettingsDlg::OnBnClickedGpuCheck)
    ON_BN_CLICKED(IDC_HDD_CHECK, &CGeneralSettingsDlg::OnBnClickedHddCheck)
    ON_BN_CLICKED(IDC_MBD_CHECK, &CGeneralSettingsDlg::OnBnClickedMbdCheck)
    ON_CBN_SELCHANGE(IDC_SELECT_CPU_COMBO, &CGeneralSettingsDlg::OnCbnSelchangeSelectCpuCombo)
    ON_BN_CLICKED(IDC_PLUGIN_MANAGE_BUTTON, &CGeneralSettingsDlg::OnBnClickedPluginManageButton)
    ON_BN_CLICKED(IDC_SHOW_NOTIFY_ICON_CHECK, &CGeneralSettingsDlg::OnBnClickedShowNotifyIconCheck)
    ON_BN_CLICKED(IDC_SELECT_CONNECTIONS_BUTTON, &CGeneralSettingsDlg::OnBnClickedSelectConnectionsButton)
    ON_BN_CLICKED(IDC_RESET_AUTO_RUN_BUTTON, &CGeneralSettingsDlg::OnBnClickedResetAutoRunButton)
    ON_EN_CHANGE(IDC_MONITOR_SPAN_EDIT, &CGeneralSettingsDlg::OnEnChangeMonitorSpanEdit)
    ON_MESSAGE(WM_SPIN_EDIT_POS_CHANGED, &CGeneralSettingsDlg::OnSpinEditPosChanged)
    ON_BN_CLICKED(IDC_AUTO_RUN_METHOD_REGESTRY_RADIO, &CGeneralSettingsDlg::OnBnClickedAutoRunMethodRegestryRadio)
    ON_BN_CLICKED(IDC_AUTO_RUN_METHOD_TASK_SCHEDULE_RADIO, &CGeneralSettingsDlg::OnBnClickedAutoRunMethodTaskScheduleRadio)
END_MESSAGE_MAP()


// Trình xử lý thông điệp của CGeneralSettingsDlg


BOOL CGeneralSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  Thêm khởi tạo bổ sung tại đây

    ((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);
    if (theApp.IsForceShowNotifyIcon())
    {
        m_data.show_notify_icon = true;
        EnableDlgCtrl(IDC_SHOW_NOTIFY_ICON_CHECK, FALSE);
    }
    CheckDlgButton(IDC_SHOW_NOTIFY_ICON_CHECK, m_data.show_notify_icon);

    //Bản Việt hóa: chỉ hỗ trợ nguồn cập nhật GitHub (repo tsudev-org/tsudev-trafficmonitor), vô hiệu hóa lựa chọn Gitee
    m_data.update_source = 0;
    CheckDlgButton(IDC_GITHUB_RADIO, TRUE);
    CheckDlgButton(IDC_GITEE_RADIO, FALSE);
    EnableDlgCtrl(IDC_GITEE_RADIO, FALSE);

    //Kiểm tra trong thư mục “Startup” của menu Start có lối tắt của chương trình không; nếu có thì thiết lập tự khởi động rồi xóa lối tắt
    wstring start_up_path = CCommon::GetStartUpPath();
    bool shortcut_exist = CCommon::FileExist((start_up_path + L"\\tsudev-trafficmonitor.lnk").c_str());
    m_data.auto_run = false;
#ifdef WITHOUT_TEMPERATURE
    m_data.auto_run_by_task_scheduler = false;
#else
    m_data.auto_run_by_task_scheduler = true;
#endif

    if (shortcut_exist)
    {
        theApp.SetAutoRun(true, false);
        m_data.auto_run = true;
        DeleteFile((start_up_path + L"\\tsudev-trafficmonitor.lnk").c_str());
        CheckDlgButton(IDC_AUTO_RUN_METHOD_REGESTRY_RADIO, TRUE);
    }
    //Kiểm tra thiết lập tự khởi động được thực hiện qua Registry hay Task Scheduler và chọn tùy chọn tương ứng
    else
    {
        bool auto_run_by_registry = theApp.GetAutoRun(&m_auto_run_path, false);
        if (auto_run_by_registry)
        {
            CheckDlgButton(IDC_AUTO_RUN_METHOD_REGESTRY_RADIO, TRUE);
            m_data.auto_run = true;
            m_data.auto_run_by_task_scheduler = false;
        }
        else
        {
            bool auto_run_by_task_scheduler = theApp.GetAutoRun(&m_auto_run_path, true);
            if (auto_run_by_task_scheduler)
            {
                CheckDlgButton(IDC_AUTO_RUN_METHOD_TASK_SCHEDULE_RADIO, TRUE);
                m_data.auto_run = true;
                m_data.auto_run_by_task_scheduler = true;
            }
        }
    }
    if (m_data.auto_run_by_task_scheduler)
        CheckDlgButton(IDC_AUTO_RUN_METHOD_TASK_SCHEDULE_RADIO, TRUE);
    else
        CheckDlgButton(IDC_AUTO_RUN_METHOD_REGESTRY_RADIO, TRUE);


    ((CButton*)GetDlgItem(IDC_SAVE_TO_APPDATA_RADIO))->SetCheck(!m_data.portable_mode);
    ((CButton*)GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO))->SetCheck(m_data.portable_mode);
    GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO)->EnableWindow(theApp.m_module_dir_writable);

    ((CButton*)GetDlgItem(IDC_AUTO_RUN_CHECK))->SetCheck(m_data.auto_run);

    ((CButton*)GetDlgItem(IDC_TODAY_TRAFFIC_TIP_CHECK))->SetCheck(m_data.traffic_tip_enable);
    m_traffic_tip_edit.SetRange(1, 32767);
    m_traffic_tip_edit.SetValue(m_data.traffic_tip_value);
    m_traffic_tip_combo.AddString(_T("MB"));
    m_traffic_tip_combo.AddString(_T("GB"));
    m_traffic_tip_combo.SetCurSel(m_data.traffic_tip_unit);
    CheckDlgButton(IDC_MEMORY_USAGE_TIP_CHECK, m_data.memory_usage_tip.enable);
    m_memory_tip_edit.SetRange(1, 100);
    m_memory_tip_edit.SetValue(m_data.memory_usage_tip.tip_value);

    CheckDlgButton(IDC_CPU_TEMP_TIP_CHECK, m_data.cpu_temp_tip.enable);
    m_cpu_temp_tip_edit.SetRange(1, 120);
    m_cpu_temp_tip_edit.SetValue(m_data.cpu_temp_tip.tip_value);

    CheckDlgButton(IDC_GPU_TEMP_TIP_CHECK, m_data.gpu_temp_tip.enable);
    m_gpu_temp_tip_edit.SetRange(1, 120);
    m_gpu_temp_tip_edit.SetValue(m_data.gpu_temp_tip.tip_value);

    CheckDlgButton(IDC_HDD_TEMP_TIP_CHECK, m_data.hdd_temp_tip.enable);
    m_hdd_temp_tip_edit.SetRange(1, 120);
    m_hdd_temp_tip_edit.SetValue(m_data.hdd_temp_tip.tip_value);

    CheckDlgButton(IDC_MBD_TEMP_TIP_CHECK, m_data.mainboard_temp_tip.enable);
    m_mbd_temp_tip_edit.SetRange(1, 120);
    m_mbd_temp_tip_edit.SetValue(m_data.mainboard_temp_tip.tip_value);

    SetControlEnable();

    m_language_combo.AddString(CCommon::LoadText(IDS_FOLLOWING_SYSTEM));
    int current_language_index{ -1 };       //Số thứ tự của ngôn ngữ hiện tại trong danh sách tất cả ngôn ngữ
    for (size_t i = 0; i < theApp.m_str_table.GetLanguageList().size(); i++)
    {
        const LanguageInfo& language_info = theApp.m_str_table.GetLanguageList()[i];
        m_language_combo.AddString(language_info.display_name.c_str());
        if (language_info == m_data.language)
            current_language_index = static_cast<int>(i);
    }
    m_language_combo.SetCurSel(current_language_index + 1);     //Vì mục đầu tiên của ComboBox là “Theo hệ thống” nên số thứ tự của ComboBox cần cộng thêm 1

    ((CButton*)GetDlgItem(IDC_SHOW_ALL_CONNECTION_CHECK))->SetCheck(m_data.show_all_interface);

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_SHOW_ALL_CONNECTION_CHECK), CCommon::LoadText(IDS_SHOW_ALL_INFO_TIP));
    m_toolTip.AddTool(GetDlgItem(IDC_SAVE_TO_APPDATA_RADIO), theApp.m_appdata_dir.c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO), theApp.m_module_dir.c_str());
    AddOrUpdateAutoRunTooltip(true);

    if (m_data.cpu_usage_acquire_method == GeneralSettingData::CA_CPU_TIME)
    {
        CheckDlgButton(IDC_USE_CPU_TIME_RADIO, TRUE);
    }
    else if (m_data.cpu_usage_acquire_method == GeneralSettingData::CA_PDH)
    {
        CheckDlgButton(IDC_USE_PDH_RADIO, TRUE);
    }

    m_monitor_span_edit.SetRange(MONITOR_TIME_SPAN_MIN, MONITOR_TIME_SPAN_MAX, MONITOR_SPAN_STEP);
    m_monitor_span_edit.SetValue(m_data.monitor_time_span);

    m_monitor_time_span_ori = m_data.monitor_time_span;
    m_update_source_ori = m_data.update_source;

    if (CTsudevTrafficMonitorDlg::Instance()->IsGetDiskUsageByPdh())
    {
        const auto& disk_names = CTsudevTrafficMonitorDlg::Instance()->GetPdhDiskUsageHelper().GetDiskNames();
        for (const auto& hdd_name : disk_names)
            m_hard_disk_combo.AddString(hdd_name);
        int cur_index = m_hard_disk_combo.FindString(-1, m_data.hard_disk_name.c_str());
        m_hard_disk_combo.SetCurSel(cur_index);
    }
#ifndef WITHOUT_TEMPERATURE
    //Khởi tạo các ô đánh dấu giám sát phần cứng
    CheckDlgButton(IDC_CPU_CHECK, m_data.IsHardwareEnable(HI_CPU));
    CheckDlgButton(IDC_GPU_CHECK, m_data.IsHardwareEnable(HI_GPU));
    CheckDlgButton(IDC_HDD_CHECK, m_data.IsHardwareEnable(HI_HDD));
    CheckDlgButton(IDC_MBD_CHECK, m_data.IsHardwareEnable(HI_MBD));

    if (theApp.m_pMonitor != nullptr)
    {
        CSingleLock sync(&theApp.m_minitor_lib_critical, TRUE);
        //Khởi tạo danh sách thả xuống chọn ổ cứng
        if (!CTsudevTrafficMonitorDlg::Instance()->IsGetDiskUsageByPdh())
        {
            for (const auto& hdd_item : theApp.m_pMonitor->AllHDDTemperature())
                m_hard_disk_combo.AddString(hdd_item.first.c_str());
            int cur_index = m_hard_disk_combo.FindString(-1, m_data.hard_disk_name.c_str());
            m_hard_disk_combo.SetCurSel(cur_index);
        }
        //Khởi tạo danh sách thả xuống chọn CPU
        m_select_cpu_combo.AddString(CCommon::LoadText(IDS_AVREAGE_TEMPERATURE));
        for (const auto& cpu_item : theApp.m_pMonitor->AllCpuTemperature())
            m_select_cpu_combo.AddString(cpu_item.first.c_str());
        int cur_index = m_select_cpu_combo.FindString(-1, m_data.cpu_core_name.c_str());
        if (cur_index < 0)
            cur_index = 0;
        m_select_cpu_combo.SetCurSel(cur_index);
    }
#endif

    //Phiên bản không có giám sát nhiệt độ: vô hiệu hóa các điều khiển liên quan đến nhiệt độ
#ifdef WITHOUT_TEMPERATURE
    EnableDlgCtrl(IDC_CPU_TEMP_TIP_CHECK, false);
    EnableDlgCtrl(IDC_CPU_TEMP_TIP_EDIT, false);
    EnableDlgCtrl(IDC_GPU_TEMP_TIP_CHECK, false);
    EnableDlgCtrl(IDC_GPU_TEMP_TIP_EDIT, false);
    EnableDlgCtrl(IDC_HDD_TEMP_TIP_CHECK, false);
    EnableDlgCtrl(IDC_HDD_TIP_EDIT, false);
    EnableDlgCtrl(IDC_MBD_TEMP_TIP_CHECK, false);
    EnableDlgCtrl(IDC_MBD_TEMP_TIP_EDIT, false);
    EnableDlgCtrl(IDC_CPU_CHECK, false);
    EnableDlgCtrl(IDC_GPU_CHECK, false);
    EnableDlgCtrl(IDC_HDD_CHECK, false);
    EnableDlgCtrl(IDC_MBD_CHECK, false);
    //EnableDlgCtrl(IDC_SELECT_HARD_DISK_COMBO, false);
    EnableDlgCtrl(IDC_SELECT_CPU_COMBO, false);
    EnableDlgCtrl(IDC_CPU_TEMP_STATIC, false);
    EnableDlgCtrl(IDC_GPU_TEMP_STATIC, false);
    EnableDlgCtrl(IDC_HDD_STATIC, false);
    EnableDlgCtrl(IDC_MBD_TEMP_STATIC, false);
    //EnableDlgCtrl(IDC_SELECT_HDD_STATIC, false);
    EnableDlgCtrl(IDC_SELECT_CPU_STATIC, false);
    EnableDlgCtrl(IDC_HARDWARE_MONITOR_STATIC, false);
#endif

    m_plugin_manager_btn.SetIcon(theApp.GetMenuIcon(IDI_PLUGINS));
    m_select_connection_btn.SetIcon(theApp.GetMenuIcon(IDI_CONNECTION));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}


void CGeneralSettingsDlg::OnBnClickedCheckNowButton()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    theApp.CheckUpdateInThread(true);
}


void CGeneralSettingsDlg::OnBnClickedCheckUpdateCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.check_update_when_start = (IsDlgButtonChecked(IDC_CHECK_UPDATE_CHECK) != 0);
}


void CGeneralSettingsDlg::OnBnClickedAutoRunCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.auto_run = (IsDlgButtonChecked(IDC_AUTO_RUN_CHECK) != 0);
    m_auto_run_modified = true;
    SetControlEnable();
}


void CGeneralSettingsDlg::OnOK()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    //Lấy thiết lập thông báo
    m_data.traffic_tip_value = m_traffic_tip_edit.GetValue();
    if (m_data.traffic_tip_value < 1) m_data.traffic_tip_value = 1;
    if (m_data.traffic_tip_value > 32767) m_data.traffic_tip_value = 32767;
    m_data.traffic_tip_unit = m_traffic_tip_combo.GetCurSel();

    auto checkTipValue = [](int& value)
    {
        if (value < 1) value = 1;
        if (value > 100) value = 100;
    };
    auto checkTempTipValue = [](int& value)
    {
        if (value < 1) value = 1;
        if (value > 120) value = 120;
    };
    m_data.memory_usage_tip.tip_value = m_memory_tip_edit.GetValue();
    checkTipValue(m_data.memory_usage_tip.tip_value);

    m_data.cpu_temp_tip.tip_value = m_cpu_temp_tip_edit.GetValue();
    checkTempTipValue(m_data.cpu_temp_tip.tip_value);

    m_data.gpu_temp_tip.tip_value = m_gpu_temp_tip_edit.GetValue();
    checkTempTipValue(m_data.gpu_temp_tip.tip_value);

    m_data.hdd_temp_tip.tip_value = m_hdd_temp_tip_edit.GetValue();
    checkTempTipValue(m_data.hdd_temp_tip.tip_value);

    m_data.mainboard_temp_tip.tip_value = m_mbd_temp_tip_edit.GetValue();
    checkTempTipValue(m_data.mainboard_temp_tip.tip_value);

    //Lấy thiết lập ngôn ngữ
    m_data.language = LanguageInfo();
    if (m_language_combo.GetCurSel() > 0)
    {
        //Lựa chọn không phải “Theo hệ thống”
        int current_language_index = m_language_combo.GetCurSel() - 1;
        if (current_language_index >= 0 && current_language_index < static_cast<int>(theApp.m_str_table.GetLanguageList().size()))
        {
            m_data.language = theApp.m_str_table.GetLanguageList()[current_language_index];
        }
    }
    if (m_data.language != theApp.m_general_data.language)
    {
        MessageBox(CCommon::LoadText(IDS_LANGUAGE_CHANGE_INFO), NULL, MB_ICONINFORMATION | MB_OK);
    }
    m_show_all_interface_modified = (m_data.show_all_interface != theApp.m_general_data.show_all_interface);

    //Lấy thiết lập vị trí lưu tệp dữ liệu
    m_data.portable_mode = (((CButton*)GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO))->GetCheck() != 0);
    if (m_data.portable_mode != theApp.m_general_data.portable_mode)
    {
        MessageBox(CCommon::LoadText(IDS_CFG_DIR_CHANGED_INFO), NULL, MB_ICONINFORMATION | MB_OK);
    }

    //m_taskbar_item_modified = (theApp.m_taskbar_data.display_item != taskbar_displat_item_ori);

    CTabDlg::OnOK();
}


void CGeneralSettingsDlg::OnBnClickedTodayTrafficTipCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.traffic_tip_enable = (IsDlgButtonChecked(IDC_TODAY_TRAFFIC_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedMemoryUsageTipCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.memory_usage_tip.enable = (IsDlgButtonChecked(IDC_MEMORY_USAGE_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedOpenConfigPathButton()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    ShellExecute(NULL, _T("explore"), theApp.m_config_dir.c_str(), NULL, NULL, SW_SHOWNORMAL);
}


void CGeneralSettingsDlg::OnBnClickedShowAllConnectionCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.show_all_interface = (IsDlgButtonChecked(IDC_SHOW_ALL_CONNECTION_CHECK) != 0);
    SetControlEnable();
}


BOOL CGeneralSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}

void CGeneralSettingsDlg::OnBnClickedUseCpuTimeRadio()
{
    m_data.cpu_usage_acquire_method = GeneralSettingData::CA_CPU_TIME;
}


void CGeneralSettingsDlg::OnBnClickedUsePdhRadio()
{
    m_data.cpu_usage_acquire_method = GeneralSettingData::CA_PDH;
}

afx_msg LRESULT CGeneralSettingsDlg::OnSpinEditPosChanged(WPARAM wParam, LPARAM lParam)
{
    CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)wParam;
    if (pSpin == nullptr)
        return 0;
    CWnd* pEdit = pSpin->GetBuddy();
    if (pEdit == &m_monitor_span_edit)       //Khi người dùng nhấn nút tăng/giảm của “Khoảng thời gian giám sát”
    {
        LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(lParam);
        if (pNMUpDown->iDelta == -1)
        {
            // Người dùng nhấn mũi tên xuống của điều khiển spin
            int value = m_monitor_span_edit.GetValue();
            value -= MONITOR_SPAN_STEP;
            value /= MONITOR_SPAN_STEP;
            value *= MONITOR_SPAN_STEP;
            m_monitor_span_edit.SetValue(value);
        }
        else if (pNMUpDown->iDelta == 1)
        {
            // Người dùng nhấn mũi tên lên của điều khiển spin
            int value = m_monitor_span_edit.GetValue();
            value += MONITOR_SPAN_STEP;
            value /= MONITOR_SPAN_STEP;
            value *= MONITOR_SPAN_STEP;
            m_monitor_span_edit.SetValue(value);
        }
        pNMUpDown->iDelta = 0;
    }
    return 0;
}



void CGeneralSettingsDlg::OnEnKillfocusMonitorSpanEdit()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây

    //Ở đây giới hạn khoảng thời gian giám sát chỉ được nhập bội số của 100
    CString str;
    GetDlgItemText(IDC_MONITOR_SPAN_EDIT, str);
    str.Replace(_T(","), _T(""));
    int value = _ttoi(str.GetString());
    if (value < MONITOR_TIME_SPAN_MIN || value > MONITOR_TIME_SPAN_MAX)
    {
        value = 1000;
    }
    else
    {
        value /= MONITOR_SPAN_STEP;
        value *= MONITOR_SPAN_STEP;
    }
    m_monitor_span_edit.SetValue(value);
}


void CGeneralSettingsDlg::OnBnClickedCpuTempTipCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.cpu_temp_tip.enable = (IsDlgButtonChecked(IDC_CPU_TEMP_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedGpuTempTipCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.gpu_temp_tip.enable = (IsDlgButtonChecked(IDC_GPU_TEMP_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedHddTempTipCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.hdd_temp_tip.enable = (IsDlgButtonChecked(IDC_HDD_TEMP_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedMbdTempTipCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.mainboard_temp_tip.enable = (IsDlgButtonChecked(IDC_MBD_TEMP_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedGithubRadio()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.update_source = 0;
    theApp.m_general_data.update_source = 0;        //Khi nhấn nút chọn “Nguồn cập nhật” thì đồng thời đổi thiết lập trong theApp, đảm bảo khi nhấn “Kiểm tra ngay” sẽ dùng nguồn cập nhật đã chọn
}


void CGeneralSettingsDlg::OnBnClickedGiteeRadio()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    //Bản Việt hóa: không hỗ trợ Gitee, luôn giữ nguồn GitHub
    m_data.update_source = 0;
    theApp.m_general_data.update_source = 0;
    CheckDlgButton(IDC_GITHUB_RADIO, TRUE);
    CheckDlgButton(IDC_GITEE_RADIO, FALSE);
}


void CGeneralSettingsDlg::OnCancel()
{
    // TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
    theApp.m_general_data.update_source = m_update_source_ori;      //Khôi phục tùy chọn “Nguồn cập nhật” ban đầu khi nhấn “Hủy”

    CTabDlg::OnCancel();
}


void CGeneralSettingsDlg::OnBnClickedRestoreDefaultTimeSpanButton()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_monitor_span_edit.SetValue(1000);
}


void CGeneralSettingsDlg::OnCbnSelchangeSelectHardDiskCombo()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    CString hard_disk_name;
    m_hard_disk_combo.GetWindowText(hard_disk_name);
    m_data.hard_disk_name = hard_disk_name.GetString();
}


void CGeneralSettingsDlg::OnBnClickedCpuCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    bool checked = IsDlgButtonChecked(IDC_CPU_CHECK) != 0;
    if (checked && !ShowHardwareMonitorWarning())
    {
        checked = false;
        CheckDlgButton(IDC_CPU_CHECK, FALSE);
    }
    m_data.SetHardwareEnable(HI_CPU, checked);
}


void CGeneralSettingsDlg::OnBnClickedGpuCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    bool checked = IsDlgButtonChecked(IDC_GPU_CHECK) != 0;
    if (checked && !ShowHardwareMonitorWarning())
    {
        checked = false;
        CheckDlgButton(IDC_GPU_CHECK, FALSE);
    }
    m_data.SetHardwareEnable(HI_GPU, checked);
}


void CGeneralSettingsDlg::OnBnClickedHddCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    bool checked = IsDlgButtonChecked(IDC_HDD_CHECK) != 0;
    if (checked && !ShowHardwareMonitorWarning())
    {
        checked = false;
        CheckDlgButton(IDC_HDD_CHECK, FALSE);
    }
    m_data.SetHardwareEnable(HI_HDD, checked);
}


void CGeneralSettingsDlg::OnBnClickedMbdCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    bool checked = IsDlgButtonChecked(IDC_MBD_CHECK) != 0;
    if (checked && !ShowHardwareMonitorWarning())
    {
        checked = false;
        CheckDlgButton(IDC_MBD_CHECK, FALSE);
    }
    m_data.SetHardwareEnable(HI_MBD, checked);

}


void CGeneralSettingsDlg::OnCbnSelchangeSelectCpuCombo()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    CString cpu_core_name;
    m_select_cpu_combo.GetWindowText(cpu_core_name);
    m_data.cpu_core_name = cpu_core_name.GetString();
}


void CGeneralSettingsDlg::OnBnClickedPluginManageButton()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    CPluginManagerDlg dlg;
    dlg.DoModal();
}


void CGeneralSettingsDlg::OnBnClickedShowNotifyIconCheck()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    m_data.show_notify_icon = (IsDlgButtonChecked(IDC_SHOW_NOTIFY_ICON_CHECK) != 0);
}


void CGeneralSettingsDlg::OnBnClickedSelectConnectionsButton()
{
    // TODO: Thêm mã xử lý thông báo của điều khiển tại đây
    CSelectConnectionsDlg dlg(m_data.connections_hide);
    if (dlg.DoModal() == IDOK)
    {
        m_data.connections_hide = dlg.GetData();
    }
}


void CGeneralSettingsDlg::OnBnClickedResetAutoRunButton()
{
    //Xóa tự khởi động cùng hệ thống trước
    theApp.SetAutoRunByRegistry(false);
    theApp.SetAutoRunByTaskScheduler(false);
    if (!theApp.SetAutoRun(true, m_data.auto_run_by_task_scheduler))    //Thiết lập lại tự khởi động cùng Windows
    {
        MessageBox(CCommon::LoadText(IDS_SET_AUTO_RUN_FAILED_WARNING), NULL, MB_ICONWARNING | MB_OK);
        return;
    }
    //Lấy đường dẫn tự khởi động cùng hệ thống
    bool auto_run = theApp.GetAutoRun(&m_auto_run_path, m_data.auto_run_by_task_scheduler);
    //Đánh dấu lại ô “Khởi động cùng Windows”
    CheckDlgButton(IDC_AUTO_RUN_CHECK, auto_run);
    //Cập nhật chú giải khi trỏ chuột
    AddOrUpdateAutoRunTooltip(false);
}


void CGeneralSettingsDlg::OnEnChangeMonitorSpanEdit()
{
    m_data.monitor_time_span = m_monitor_span_edit.GetValue();
}

void CGeneralSettingsDlg::OnBnClickedAutoRunMethodRegestryRadio()
{
    m_data.auto_run_by_task_scheduler = false;
    m_auto_run_modified = true;
    SetControlEnable();
}

void CGeneralSettingsDlg::OnBnClickedAutoRunMethodTaskScheduleRadio()
{
    m_data.auto_run_by_task_scheduler = true;
    m_auto_run_modified = true;
    SetControlEnable();
}
