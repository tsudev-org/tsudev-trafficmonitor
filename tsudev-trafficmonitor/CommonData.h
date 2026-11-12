//Tệp này chứa định nghĩa các cấu trúc và kiểu liệt kê toàn cục
#pragma once
#include "stdafx.h"
#include "TaskbarItemOrderHelper.h"

struct Date
{
    int year{};
    int month{};
    int day{};

    int week() const;       //Ngày này thuộc tuần thứ mấy trong năm

    //So sánh ngày của hai đối tượng HistoryTraffic; nếu thời gian của a lớn hơn b thì trả về true
    static bool DateGreater(const Date& a, const Date& b);

    //Kiểm tra ngày của hai đối tượng HistoryTraffic có bằng nhau không
    static bool DateEqual(const Date& a, const Date& b);

};

//Lưu lưu lượng lịch sử của một ngày
struct HistoryTraffic : public Date
{
    //Lưu lượng đã dùng trong ngày (đơn vị KB)
    unsigned __int64 up_kBytes{};
    unsigned __int64 down_kBytes{};
    bool mixed{ true };     //Là true nếu không phân biệt lưu lượng tải lên và tải xuống

    unsigned __int64 kBytes() const;

};

//Màu dùng để biểu thị lưu lượng ở các khoảng khác nhau trong thống kê lưu lượng
#define TRAFFIC_COLOR_BLUE RGB(0, 183, 238)
#define TRAFFIC_COLOR_GREEN RGB(128, 194, 105)
#define TRAFFIC_COLOR_YELLOE RGB(255, 216, 58)
#define TRAFFIC_COLOR_RED RGB(255, 95, 74)
#define TRAFFIC_COLOR_DARK_RED RGB(166, 19, 0)

//Đơn vị tốc độ mạng
enum class SpeedUnit
{
    AUTO,       //Tự động
    KBPS,       //KB/s
    MBPS        //MB/s
};


//Các mục giám sát phần cứng
enum HardwareItem
{
    HI_CPU = 1 << 0,        //CPU
    HI_GPU = 1 << 1,        //Card đồ họa (GPU)
    HI_HDD = 1 << 2,        //Ổ cứng
    HI_MBD = 1 << 3         //Bo mạch chủ
};

#define DEF_CH L'\"'        //Ký tự thêm vào đầu và cuối chuỗi khi ghi và đọc chuỗi trong tệp ini
struct DispStrings      //Văn bản hiển thị
{
private:
    std::map<CommonDisplayItem, wstring> map_str;

public:
    //Lấy một văn bản hiển thị
    wstring& Get(CommonDisplayItem item);
    const wstring& GetConst(CommonDisplayItem item) const;

    const std::map<CommonDisplayItem, wstring>& GetAllItems() const;

    bool operator==(const DispStrings& disp_str) const;

    //Tải văn bản hiển thị của một mục plugin
    void Load(const std::wstring& plugin_id, const std::wstring& disp_str);

    //Có vô hiệu hay không
    bool IsInvalid() const;
};

//Hành động khi nhấp đúp chuột vào cửa sổ
enum class DoubleClickAction
{
    CONNECTION_INFO,    //Chi tiết kết nối
    HISTORY_TRAFFIC,    //Thống kê lưu lượng
    SHOW_MORE_INFO,     //Hiển thị thêm thông tin
    OPTIONS,            //Tùy chọn
    TASK_MANAGER,       //Trình quản lý tác vụ (Task Manager)
    SEPCIFIC_APP,       //Ứng dụng chỉ định
    CHANGE_SKIN,        //Đổi skin
    NONE                //Không làm gì
};

//Chế độ màu
enum class ColorMode
{
    Default, //Màu mặc định
    Light   //Sáng
};

//Chuyển cỡ chữ thành lfHeight trong cấu trúc LOGFONT
inline int FontSizeToLfHeight(int font_size, int dpi = 0)
{
    if (dpi == 0)
    {
        HDC hDC = ::GetDC(HWND_DESKTOP);
        dpi = GetDeviceCaps(hDC, LOGPIXELSY);
        ::ReleaseDC(HWND_DESKTOP, hDC);
    }
    int lfHeight = -MulDiv(font_size, dpi, 72);
    return lfHeight;
}

//Phông chữ
struct FontInfo
{
    CString name;       //Tên phông chữ
    int size{ 9 };      //Cỡ chữ
    bool bold{};        //In đậm
    bool italic{};      //In nghiêng
    bool underline{};   //Gạch chân
    bool strike_out{};  //Gạch ngang

    bool operator==(const FontInfo& a) const;

    //Tạo một đối tượng CFont
    void Create(CFont& font, int dpi = 0) const
    {
        font.CreateFont(
            FontSizeToLfHeight(size, dpi), // nHeight
            0, // nWidth
            0, // nEscapement
            0, // nOrientation
            (bold ? FW_BOLD : FW_NORMAL), // nWeight
            italic, // bItalic
            underline, // bUnderline
            strike_out, // cStrikeOut
            DEFAULT_CHARSET, // nCharSet
            OUT_DEFAULT_PRECIS, // nOutPrecision
            CLIP_DEFAULT_PRECIS, // nClipPrecision
            DEFAULT_QUALITY, // nQuality
            DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
            name);
    }
};

//Chế độ hiển thị trong chế độ xem danh sách của thống kê lưu lượng
enum class HistoryTrafficViewType
{
    HV_DAY,         //Xem theo ngày
    HV_WEEK,        //Xem theo tuần
    HV_MONTH,          //Xem theo tháng
    HV_QUARTER,     //Xem theo quý
    HV_YEAR            //Xem theo năm
};

struct StringSet
{
public:
    bool Contains(const std::wstring& str) const;
    void SetStrContained(const std::wstring& str, bool contained);
    void FromString(const std::wstring& str);
    std::wstring ToString() const;
    void FromVector(const std::vector<std::wstring>& vec);
    std::vector<std::wstring> ToVector() const;
    std::set<std::wstring>& data();
private:
    std::set<std::wstring> string_set;
};

struct LanguageInfo
{
    wstring display_name;           // Chuỗi hiển thị trong menu thả xuống chọn ngôn ngữ
    wstring bcp_47;                 // Mã BCP-47
    wstring default_font_name;      // Phông chữ mặc định
    wstring translator;
    wstring translator_url;
    WORD language_id{};

    bool operator==(const LanguageInfo& another) const;
    wstring toConfigString() const;
    void fromConfigString(const wstring& config_str);
    bool isEmpty() const;
};

//Dữ liệu tùy chọn
struct MainConfigData
{
    int m_transparency{ 100 };          //Độ trong suốt của cửa sổ
    bool m_show_more_info{ false };     //Hiển thị thêm thông tin
    bool m_show_task_bar_wnd{ false };  //Hiển thị cửa sổ thanh tác vụ
    bool m_hide_main_window;            //Ẩn cửa sổ chính
    //bool m_tbar_show_cpu_memory;      //Cửa sổ thanh tác vụ hiển thị mức sử dụng CPU và bộ nhớ

    int m_position_x;   //Tọa độ x của vị trí cửa sổ
    int m_position_y;   //Tọa độ y của vị trí cửa sổ
    int m_edge_dock{};  //Cửa sổ nổi đang thu vào mép nào (giá trị EdgeDock::Edge; 0 = không thu); khi thu, vị trí lưu là vị trí lúc hiện lại

    bool m_auto_select{ false };    //Tự động chọn kết nối
    bool m_select_all{ false };     //Thống kê tốc độ mạng của tất cả kết nối
    string m_connection_name;      //Tên mạng đang được chọn

    wstring m_skin_name;            //Tên skin đã chọn

    bool skin_auto_adapt{ false };  //Tự động đổi skin theo chế độ sáng/tối
    wstring skin_name_light_mode;   //Tên skin dùng ở chế độ sáng
    wstring skin_name_dark_mode;    //Tên skin dùng ở chế độ tối

    int m_dft_notify_icon = 0;      //Biểu tượng thông báo mặc định (dùng để phân biệt chế độ sáng và tối của Win10)
    int m_notify_icon_selected{};   //Biểu tượng vùng thông báo cần hiển thị
    bool m_notify_icon_auto_adapt{ false }; //Biểu tượng vùng thông báo có tự động thích ứng chế độ sáng/tối của Win10 hay không

    //bool m_show_internet_ip{ false };     //Có hiển thị địa chỉ IP công cộng trong hộp thoại “Chi tiết kết nối” hay không
    bool m_use_log_scale{ false };          //Có dùng tỉ lệ logarit khi vẽ hình chữ nhật biểu thị lưu lượng trong hộp thoại “Thống kê lưu lượng” hay không
    HistoryTrafficViewType m_view_type{};
    bool m_sunday_first{ true };            //Có lấy Chủ nhật làm ngày đầu tuần hay không
    StringSet plugin_disabled;      //Các plugin đã bị vô hiệu hóa

};

//Kiểu hiển thị bộ nhớ
enum class MemoryDisplay
{
    USAGE_PERCENTAGE,       //Phần trăm đã dùng
    MEMORY_USED,            //Bộ nhớ đã dùng
    MEMORY_AVAILABLE        //Bộ nhớ khả dụng
};

//Dữ liệu lưu riêng cho từng skin
struct SkinSettingData
{
    FontInfo font;          //Phông chữ
    DispStrings disp_str;   //Văn bản hiển thị
    std::map<CommonDisplayItem, COLORREF> text_colors{};    //Màu chữ
    bool specify_each_item_color{};

    bool IsEmpty() const;
    bool operator==(const SkinSettingData& a) const;
};

//Dữ liệu dùng chung giữa “Thiết lập cửa sổ chính” và “Thiết lập cửa sổ thanh tác vụ” trong Tùy chọn (không dùng cấu trúc này để tạo đối tượng)
struct PublicSettingData
{
    bool specify_each_item_color{ false };      //Có chỉ định màu cho từng mục hay không
    FontInfo font;          //Phông chữ
    DispStrings disp_str;   //Văn bản hiển thị
    bool speed_short_mode{ false };     //Chế độ hiển thị tốc độ mạng rút gọn (giảm số chữ số thập phân, đơn vị không hiển thị “B”)
    bool separate_value_unit_with_space{ true };    //Giá trị và đơn vị tốc độ mạng cách nhau bằng khoảng trắng
    bool show_tool_tip{ true };         //Hiển thị chú giải khi trỏ chuột
    MemoryDisplay memory_display{ MemoryDisplay::USAGE_PERCENTAGE };    //Kiểu hiển thị bộ nhớ

    bool unit_byte{ true };             //Dùng đơn vị byte (B) thay vì bit (b)
    SpeedUnit speed_unit;       //Đơn vị của tốc độ mạng
    bool hide_unit;         //Ẩn đơn vị
    bool hide_percent;      //Ẩn ký hiệu phần trăm
    DoubleClickAction double_click_action;      //Hành động khi nhấp đúp chuột
    wstring double_click_exe;   //Đường dẫn chương trình được mở khi hành động nhấp đúp là mở ứng dụng chỉ định
};

//#define MAIN_WND_COLOR_NUM 9      //Số màu của cửa sổ chính
//Dữ liệu “Thiết lập cửa sổ chính” trong Tùy chọn
struct MainWndSettingData : public PublicSettingData
{
    std::map<CommonDisplayItem, COLORREF> text_colors{};    //Màu chữ
    bool swap_up_down{ false };     //Đổi chỗ hiển thị tải lên và tải xuống
    bool hide_main_wnd_when_fullscreen;     //Ẩn cửa sổ nổi khi có chương trình chạy toàn màn hình
    bool m_always_on_top{ false };      //Cửa sổ luôn trên cùng
    bool m_lock_window_pos{ false };    //Khóa vị trí cửa sổ
    bool m_mouse_penetrate{ false };    //Cho chuột xuyên qua
    bool m_alow_out_of_border{ false };     //Có cho phép cửa sổ nổi vượt ra ngoài biên màn hình hay không
    bool m_edge_auto_hide{ true };          //Kéo cửa sổ nổi sát mép màn hình thì thu gọn vào mép (hiện tay nắm mũi tên)

    void FormSkinSettingData(const SkinSettingData& sking_setting_data);
    SkinSettingData ToSkinSettingData() const;
};

//#define TASKBAR_COLOR_NUM 18      //Số màu của cửa sổ thanh tác vụ

struct TaskbarItemColor //Màu của từng mục trên cửa sổ thanh tác vụ
{
    COLORREF label{};   //Màu nhãn
    COLORREF value{};   //Màu giá trị

    bool operator==(const TaskbarItemColor& item) const
    {
        return label == item.label && value == item.value;
    }
};

//Dữ liệu “Thiết lập cửa sổ thanh tác vụ” trong Tùy chọn
struct TaskBarSettingData : public PublicSettingData
{
    COLORREF  back_color{ RGB(0, 0, 0) };   //Màu nền
    COLORREF transparent_color{ RGB(0, 0, 0) };     //Màu trong suốt
    COLORREF status_bar_color{ RGB(0, 0, 0) };      // Màu thanh trạng thái CPU/bộ nhớ
    std::map<CommonDisplayItem, TaskbarItemColor> text_colors{};    //Màu chữ
    int dft_back_color = 0;                         //Màu nền mặc định
    int dft_transparent_color = 0;                  //Màu trong suốt mặc định
    int dft_status_bar_color = 0x005A5A5A;          //Màu mặc định của thanh trạng thái CPU/bộ nhớ
    int dft_text_colors = 0x00ffffffU;              //Màu chữ mặc định

    bool auto_adapt_light_theme{ true };            //Có tự động thích ứng chủ đề sáng hay không
    int dark_default_style{ 0 };                    //Mẫu màu dùng cho chủ đề tối
    int light_default_style{ -1 };                  //Mẫu màu dùng cho chủ đề sáng
    bool auto_set_background_color{ false };        //Tự động đặt màu nền theo màu thanh tác vụ
    bool auto_save_taskbar_color_settings_to_preset{};    //Khi bật “Tự động thích ứng chủ đề sáng/tối của Windows 10”, có tự động lưu thiết lập màu hiện tại vào mẫu màu tương ứng khi thiết lập màu thay đổi hay không
    bool IsTaskbarTransparent() const;
    void SetTaskabrTransparent(bool transparent);

    CTaskbarItemOrderHelper item_order;
    DisplayItemSet display_item{ TDI_UP, TDI_DOWN };      //Các mục hiển thị trên cửa sổ thanh tác vụ
    StringSet plugin_display_item;                  //Các mục plugin hiển thị trên cửa sổ thanh tác vụ

    bool show_taskbar_wnd_in_secondary_display{ false };    //Có hiển thị cửa sổ thanh tác vụ trên màn hình phụ hay không
    int secondary_display_index{};      //Hiển thị cửa sổ thanh tác vụ trên màn hình phụ thứ mấy
    bool value_right_align{ false };    //Giá trị có căn phải hay không
    int digits_number{ 4 };             //Số chữ số của dữ liệu
    bool horizontal_arrange{ true };    //Xếp ngang
    bool show_status_bar{ true };       //Hiển thị thanh trạng thái CPU/bộ nhớ
    bool tbar_wnd_on_left{ false };     //Nếu là true thì cửa sổ thanh tác vụ hiển thị ở bên trái (hoặc phía trên) thanh tác vụ
    bool tbar_wnd_snap{ false };     	//Nếu là true thì trên Win11 cửa sổ thanh tác vụ bám sát phần giữa thanh tác vụ, ngược lại thì gần mép
    bool cm_graph_type{ false };        //Nếu là false thì dùng kiểu mặc định, biểu đồ cột hiển thị mức sử dụng; nếu là true thì hiển thị mức sử dụng dạng cuộn
    bool show_graph_dashed_box{ true }; //Hiển thị khung viền nét đứt của biểu đồ sử dụng

    int item_space{};                   //Khoảng cách giữa các mục
    int vertical_margin{};              //Khoảng cách dọc giữa các mục
    int window_offset_top{};            //Lề trên của cửa sổ thanh tác vụ
    int window_offset_left{};           //Lề trái của cửa sổ thanh tác vụ
    void ValidItemSpace();
    void ValidVerticalMargin();
    void ValidWindowOffsetTop();
    void ValidWindowOffsetLeft();
    bool avoid_overlap_with_widgets{ false };   //Tránh chồng lên tiện ích (widget) ở bên phải
    int taskbar_left_space_win11{};         //Trên Windows 11, chiều rộng của tiện ích (widget) trên thanh tác vụ
    int taskbar_right_space_win11{};        //Trên Windows 11, khoảng cách từ cửa sổ thanh tác vụ đến mép phải thanh tác vụ (chỉ có hiệu lực khi không lấy được vị trí cửa sổ TrayNotifyWnd)

    bool show_netspeed_figure{ false };     //Có hiển thị biểu đồ sử dụng tốc độ mạng hay không
    int netspeed_figure_max_value;          //Giá trị lớn nhất của biểu đồ tốc độ mạng
    int netspeed_figure_max_value_unit{};   //Đơn vị giá trị lớn nhất của biểu đồ tốc độ mạng (0: KB, 1: MB)
    unsigned __int64 GetNetspeedFigureMaxValueInBytes() const;  //Lấy giá trị lớn nhất của biểu đồ tốc độ mạng (đơn vị byte)
    bool graph_color_following_system{ false }; //Màu biểu đồ sử dụng theo màu chủ đề hệ thống
    COLORREF GetUsageGraphColor() const;    //Lấy màu của biểu đồ sử dụng

    bool disable_d2d{ false };//Có tắt vẽ bằng D2D hay không
    DWORD update_layered_window_error_code{0}; // Mã lỗi khi UpdateLayeredWindowIndirect thất bại lúc dùng D2D1 để kết xuất; sẽ được đặt lại về 0 khi đóng cửa sổ thanh tác vụ
    bool enable_colorful_emoji{ true };       //Có hiển thị emoji màu hay không

    bool is_windows_web_experience_detected{ false }; //Có phát hiện thông tin cài đặt tiện ích Windows Web Experience hay không
};

//Dữ liệu “Thiết lập chung” trong Tùy chọn
struct GeneralSettingData
{
    bool check_update_when_start{ true };
    bool auto_run{ false };
    bool auto_run_by_task_scheduler{ false };   //Có tự khởi động cùng hệ thống qua Task Scheduler hay không
    bool show_notify_icon{ true };    //Hiển thị biểu tượng ở vùng thông báo
//Thông điệp thông báo
    bool traffic_tip_enable{ false };       //Có bật cảnh báo khi lưu lượng vượt ngưỡng hay không
    int traffic_tip_value;                  //Ngưỡng lưu lượng cần cảnh báo
    int traffic_tip_unit{};                 //Đơn vị của giá trị lưu lượng cần cảnh báo (0: MB, 1: GB)

    struct NotifyTipSettings        //Thiết lập hiện cảnh báo khi vượt quá một giá trị nào đó
    {
        bool enable;                //Có bật cảnh báo hay không
        int tip_value;              //Ngưỡng giá trị để hiện cảnh báo
    };
    NotifyTipSettings memory_usage_tip;     //Cảnh báo mức sử dụng bộ nhớ vượt ngưỡng
    NotifyTipSettings cpu_temp_tip;         //Cảnh báo nhiệt độ CPU vượt ngưỡng
    NotifyTipSettings gpu_temp_tip;         //Cảnh báo nhiệt độ GPU vượt ngưỡng
    NotifyTipSettings hdd_temp_tip;         //Cảnh báo nhiệt độ ổ cứng vượt ngưỡng
    NotifyTipSettings mainboard_temp_tip;   //Cảnh báo nhiệt độ bo mạch chủ vượt ngưỡng


    //Ngôn ngữ
    LanguageInfo language;

    bool show_all_interface{ true };

    //Phương thức lấy mức sử dụng CPU
    enum CpuUsageAcquireMethod
    {
        CA_CPU_TIME,    //Thời gian sử dụng
        CA_PDH         //Bộ đếm hiệu năng (Performance Counter)
    };
    CpuUsageAcquireMethod cpu_usage_acquire_method{};  //Phương thức lấy mức sử dụng CPU

    bool portable_mode{ false };        //Chế độ di động (portable): nếu là true thì toàn bộ dữ liệu chương trình được lưu trong thư mục chứa exe, ngược lại lưu trong thư mục Appdata\Roaming
    int monitor_time_span{ 1000 };    //Khoảng thời gian giám sát

    std::wstring hard_disk_name;        //Tên ổ cứng cần giám sát
    std::wstring cpu_core_name;         //Tên lõi CPU cần giám sát

    unsigned int hardware_monitor_item{};   //Các phần cứng cần giám sát
    bool IsHardwareEnable(HardwareItem item_type) const
    {
        return hardware_monitor_item & item_type;
    }
    void SetHardwareEnable(HardwareItem item_type, bool enable)
    {
        if (enable)
            hardware_monitor_item |= item_type;
        else
            hardware_monitor_item &= ~item_type;
    }

    StringSet connections_hide;     //Dùng để lưu những mạng cần ẩn khỏi menu con “Chọn kết nối mạng”
};

//Định nghĩa giá trị lớn nhất và nhỏ nhất hợp lệ của khoảng thời gian giám sát
#define MONITOR_TIME_SPAN_MIN 200
#define MONITOR_TIME_SPAN_MAX 30000

//Truyền tham chiếu một biến bool qua hàm khởi tạo, đặt thành true khi khởi tạo và thành false khi hủy
class CFlagLocker
{
public:
    CFlagLocker(bool& flag)
        : m_flag(flag)
    {
        m_flag = true;
    }

    ~CFlagLocker()
    {
        m_flag = false;
    }

private:
    bool& m_flag;
};
