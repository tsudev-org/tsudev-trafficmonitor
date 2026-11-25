#pragma once
#include"Common.h"
#include "afxcmn.h"
#include "AdapterCommon.h"
#include "BaseDialog.h"
#include "ListCtrlEx.h"
#include <memory>
#include <mutex>

// Hộp thoại CNetworkInfoDlg

class CNetworkInfoDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CNetworkInfoDlg)

public:
    //adapters, if_rows: bản sao danh sách kết nối và bảng MIB_IFROW tại thời điểm mở hộp thoại.
    //Hộp thoại giữ bản sao riêng vì luồng giám sát có thể cấp phát lại các dữ liệu gốc bất cứ lúc nào.
    CNetworkInfoDlg(const vector<NetWorkConection>& adapters, const vector<MIB_IFROW>& if_rows, int connection_selected, CWnd* pParent = NULL);   // Hàm khởi tạo chuẩn
    virtual ~CNetworkInfoDlg();

    // Dữ liệu hộp thoại
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_NETWORK_INFO_DIALOG };
#endif

    SYSTEMTIME m_start_time;		//Thời điểm chương trình khởi động

protected:

    vector<NetWorkConection> m_connections;
    vector<MIB_IFROW> m_if_rows;
    int m_connection_selected;		//Kết nối đang hiển thị trong hộp thoại hiện tại
    int m_current_connection;		//Kết nối được chọn ban đầu

    CListCtrlEx m_info_list;
    CMenu m_menu;
    CString m_selected_string;
    CFont m_font_bold;		//Kiểu in đậm của phông chữ mặc định

    //Kết quả tra cứu IP công cộng, dùng chung giữa hộp thoại và luồng làm việc.
    //Luồng giữ một shared_ptr riêng nên vẫn an toàn khi hộp thoại đã đóng trước khi luồng kết thúc.
    struct PublicIpState
    {
        std::mutex mutex;
        CCommon::PublicIpResult result[2]{ CCommon::PublicIpResult::FAILED, CCommon::PublicIpResult::FAILED };
        wstring ip_address[2];
        wstring ip_location[2];
    };
    std::shared_ptr<PublicIpState> m_ip_state;
    bool m_ip_acquiring{ false };       //Đang lấy địa chỉ IP công cộng (tránh tạo nhiều luồng khi nhấp đúp liên tục)

    //void GetIPAddress();	//Lấy địa chỉ IP
    void ShowInfo();
    void ShowTrafficInfo();     //Hiển thị số byte đã nhận/gửi (bộ đếm 64-bit, cập nhật mỗi giây)
    void GetProgramElapsedTime();
    MIB_IFROW& GetConnectIfTable(int connection_index);    //Lấy đối tượng MIB_IFROW của kết nối mạng đang chọn. connection_index là chỉ số trong m_connections
    NetWorkConection GetConnection(int connection_index); //Lấy đối tượng NetWorkConection của kết nối mạng đang chọn. connection_index là chỉ số trong m_connections
    bool IsConnectionActive(int connection_index);      //Kết nối có đang hoạt động không (card bị tắt, rút cáp... trả về false)
    int FindBusiestActiveConnection();                  //Chỉ số kết nối đang hoạt động có tổng lưu lượng lớn nhất; -1 nếu không có

    //Hàm luồng lấy IP công cộng
    static UINT GetInternetIPThreadFunc(LPVOID lpParam);

    virtual CString GetDialogName() const override;

    virtual void DoDataExchange(CDataExchange* pDX);    // Hỗ trợ DDX/DDV

    DECLARE_MESSAGE_MAP()

    virtual BOOL OnInitDialog();
public:
    afx_msg void OnCopyText();
    afx_msg void OnNMRClickInfoList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnClose();
    afx_msg void OnBnClickedPreviousButton();
    afx_msg void OnBnClickedNextButton();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnNMDblclkInfoList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnPublicIpAcquired(WPARAM wParam, LPARAM lParam);
    afx_msg void OnNMCustomdrawInfoList1(NMHDR* pNMHDR, LRESULT* pResult);
};
