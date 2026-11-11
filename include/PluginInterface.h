/*********************************************************
* Giao diện plugin của tsudev-trafficmonitor (API phiên bản 8, tương thích nhị phân với plugin viết cho dự án gốc TrafficMonitor)
* Giấy phép: "Anti 996" License 1.0 - xem tệp LICENSE ở thư mục gốc.
**********************************************************/
#pragma once

class IPluginDrawer;

//Giao diện mục hiển thị của plugin
class IPluginItem
{
public:
    /**
     * @brief   Lấy tên mục hiển thị
     * @return  const wchar_t*
     */
    virtual const wchar_t* GetItemName() const = 0;

    /**
     * @brief   Lấy ID duy nhất của mục hiển thị
     * @return  const wchar_t*
     */
    virtual const wchar_t* GetItemId() const = 0;

    /**
     * @brief   Lấy văn bản nhãn của mục
     * @return  const wchar_t*
     */
    virtual const wchar_t* GetItemLableText() const = 0;

    /**
     * @brief   Lấy văn bản giá trị của mục
     * @detail  Vì hàm này có thể được gọi thường xuyên nên đừng lấy dữ liệu giám sát tại đây,
     *  mà nên lấy dữ liệu trong hàm ITMPlugin::DataRequired rồi lưu lại, sau đó trả về giá trị đã lấy tại đây
     * @return  const wchar_t*
     */
    virtual const wchar_t* GetItemValueText() const = 0;

    /**
     * @brief   Lấy văn bản ví dụ giá trị của mục
     * @detail  Độ dài chuỗi mà hàm này trả về sẽ được dùng để tính chiều rộng vùng hiển thị
     * @return  const wchar_t*
     */
    virtual const wchar_t* GetItemValueSampleText() const = 0;

    /**
     * @brief   Vùng hiển thị có do plugin tự vẽ hay không
     * @detail
     *  Nếu trả về false thì chương trình chính sẽ vẽ vùng hiển thị dựa trên văn bản do GetItemLableText và GetItemValueText trả về, việc ghi đè hàm DrawItem sẽ không có tác dụng.
     *  Nếu ghi đè hàm này và trả về true thì bắt buộc phải ghi đè hàm DrawItem và thêm mã vẽ vùng hiển thị vào đó,
     *  lúc này giá trị trả về của GetItemLableText, GetItemValueText và GetItemValueSampleText sẽ bị chương trình chính bỏ qua
     * @return  bool
     */
    virtual bool IsCustomDraw() const { return false; }

    /**
     * @brief   Lấy chiều rộng vùng hiển thị
     * @detail
     *  Chỉ khi hàm CustomDraw() trả về true thì việc ghi đè hàm này mới có tác dụng.
     *  Giá trị trả về là chiều rộng ở DPI 96 (100%), chương trình chính sẽ tự động phóng to theo tỉ lệ dựa trên thiết lập DPI hiện tại của hệ thống,
     *  Vì vậy bạn không cần trả về giá trị khác nhau cho các thiết lập DPI khác nhau.
     *  Lưu ý: giá trị trả về ở đây là chiều rộng tối thiểu cần cho vùng tự vẽ, giá trị tham số w trong hàm DrawItem có thể lớn hơn giá trị này
     * @return  int
     */
    virtual int GetItemWidth() const { return 0; }

    /**
     * @brief   Hàm tự vẽ vùng hiển thị, chỉ có tác dụng khi hàm CustomDraw() trả về true
     * @param   void * hDC Handle ngữ cảnh vẽ
     * @param   int x Vùng hình chữ nhật để vẽ
     * @param   int y
     * @param   int w
     * @param   int h
     * @param   bool dark_mode Chế độ tối là true, chế độ sáng là false
     * @return  void
     */
    virtual void DrawItem(void* hDC, int x, int y, int w, int h, bool dark_mode) {}

    /**
     * @brief   Lấy chiều rộng vùng hiển thị
     * @detail
     *  Chỉ khi hàm CustomDraw() trả về true thì việc ghi đè hàm này mới có tác dụng.
     *  Hàm này khác GetItemWidth: plugin có thể tính chiều rộng cần thiết dựa trên tham số hDC,
     *  Hàm trả về chiều rộng thực tế, chương trình chính sẽ không phóng to giá trị trả về theo DPI hiện tại của hệ thống.
     *  Lưu ý: giá trị trả về ở đây là chiều rộng tối thiểu cần cho vùng tự vẽ, giá trị tham số w trong hàm DrawItem có thể lớn hơn giá trị này
     * @param   void * hDC Handle ngữ cảnh vẽ
     * @return  int
     */
    virtual int GetItemWidthEx(void* hDC) const { return 0; }

    /** Loại sự kiện chuột */
    enum MouseEventType
    {
        MT_LCLICKED,    /**< Nhấp chuột trái */
        MT_RCLICKED,    /**< Nhấp chuột phải */
        MT_DBCLICKED,   /**< Nhấp đúp chuột trái */
        MT_WHEEL_UP,    /**< Cuộn con lăn chuột lên */
        MT_WHEEL_DOWN,  /**< Cuộn con lăn chuột xuống */
    };

    enum MouseEventFlag
    {
        MF_TASKBAR_WND = 1 << 0,        /**< Có phải sự kiện chuột của cửa sổ thanh tác vụ hay không */
    };

    /**
     * @brief   Được chương trình chính gọi khi có sự kiện chuột trong vùng hiển thị của plugin
     * @param   MouseEventType type Loại sự kiện chuột
     * @param   int x Tọa độ x của con trỏ chuột
     * @param   int y Tọa độ y của con trỏ chuột
     * @param   void* hWnd Handle của cửa sổ phát sinh sự kiện chuột này (cửa sổ chính hoặc cửa sổ thanh tác vụ)
     * @param   int flag Tổ hợp các hằng enum MouseEventFlag
     * @return  int Nếu trả về khác 0, chương trình chính coi như plugin đã phản hồi đầy đủ sự kiện chuột này và sẽ không phản hồi thêm.
     *   Ví dụ khi type là MT_RCLICKED, nếu trả về 0 thì sẽ hiện menu chuột phải do chương trình chính cung cấp; nếu trả về khác 0 thì chương trình chính sẽ không xử lý gì thêm.
     */
    virtual int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) { return 0; }

    enum KeyboardEventFlag
    {
        KF_TASKBAR_WND = 1 << 0,        /**< Có phải sự kiện bàn phím của cửa sổ thanh tác vụ hay không */
    };

    /**
     * @brief   Được chương trình chính gọi khi có sự kiện bàn phím trong vùng hiển thị của plugin
     * @param   int key Phím được nhấn
     * @param   bool ctrl Có nhấn phím Ctrl hay không
     * @param   bool shift Có nhấn phím Shift hay không
     * @param   bool alt Có nhấn phím Alt hay không
     * @param   void* hWnd Handle của cửa sổ phát sinh sự kiện bàn phím này (cửa sổ chính hoặc cửa sổ thanh tác vụ)
     * @param   int flag Tổ hợp các hằng enum KeyboardEventFlag
     * @return  int Nếu trả về khác 0, chương trình chính coi như plugin đã phản hồi đầy đủ sự kiện bàn phím này và sẽ không phản hồi thêm.
     */
    virtual int OnKeboardEvent(int key, bool ctrl, bool shift, bool alt, void* hWnd, int flag) { return 0; }

    enum ItemInfoType
    {

    };
    //Giao diện dự phòng
    virtual void* OnItemInfo(ItemInfoType, void* para1, void* para2) { return 0; }

    /**
     * @brief   Có hiển thị biểu đồ sử dụng tài nguyên của mục này trên thanh tác vụ hay không
     * @return  1: hiển thị, 0: không hiển thị
     */
    virtual int IsDrawResourceUsageGraph() const { return 0; }

    /**
     * @brief   Lấy giá trị biểu đồ sử dụng tài nguyên. Có hiệu lực khi IsDrawResourceUsageGraphType trả về giá trị khác 0
     * @return  float Giá trị biểu đồ sử dụng tài nguyên, phạm vi 0.0~1.0.
     */
    virtual float GetResourceUsageGraphValue() const { return 0.0; }

    /**
     * @brief   Hàm tự vẽ vùng hiển thị, chỉ có tác dụng khi hàm CustomDraw() trả về true
     * @param   IPluginDrawer* pDrawer Con trỏ tới giao diện vẽ dành cho plugin
     * @param   int x Vùng hình chữ nhật để vẽ
     * @param   int y
     * @param   int w
     * @param   int h
     * @param   bool dark_mode Chế độ tối là true, chế độ sáng là false
     * @return  bool
     */
    virtual bool DrawItemEx(IPluginDrawer* pDrawer, int x, int y, int w, int h, bool dark_mode) { return false; }

    /**
     * @brief   Có chiếm trọn hai dòng trên thanh tác vụ hay không
     * @return  1: có, 0: không
     */
    virtual int IsDoubleLineExclusive() const { return 0; }
};

class ITsudevTrafficMonitor;
//Tên cũ của giao diện (dự án gốc TrafficMonitor): giữ để mã nguồn plugin có sẵn vẫn biên dịch được; bố cục vtable không đổi.
using ITrafficMonitor = ITsudevTrafficMonitor;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Giao diện plugin
class ITMPlugin
{
public:
    /**
     * @brief   Phiên bản giao diện plugin, chỉ thay đổi giá trị trả về ở đây khi giao diện plugin thay đổi.
     * @attention Nhà phát triển plugin không nên sửa giá trị trả về ở đây, cũng không nên ghi đè hàm ảo này.
     * @return  int
     */
    virtual int GetAPIVersion() const { return 8; }

    /**
     * @brief   Lấy đối tượng mục hiển thị của plugin
     * @detail  Một dll plugin có thể cung cấp nhiều đối tượng triển khai giao diện IPluginItem, tương ứng với nhiều mục hiển thị.
     *  Khi index lớn hơn hoặc bằng 0 và nhỏ hơn số đối tượng giao diện IPluginItem thì trả về con trỏ giao diện IPluginItem của đối tượng, các trường hợp khác nên trả về con trỏ rỗng.
     *  Ví dụ plugin cung cấp hai mục hiển thị thì khi index bằng 0 hoặc 1 sẽ trả về đối tượng giao diện IPluginItem tương ứng, các giá trị khác bắt buộc phải trả về con trỏ rỗng.
     * @param   int index Chỉ số của đối tượng
     * @return  IPluginItem* Đối tượng mục hiển thị của plugin
     */
    virtual IPluginItem* GetItem(int index) = 0;

    /**
     * @brief   Chương trình chính sẽ gọi hàm này theo chu kỳ, plugin cần lấy dữ liệu giám sát một lần trong hàm này
     */
    virtual void DataRequired() = 0;

    /** Giá trị trả về của hộp thoại tùy chọn */
    enum OptionReturn
    {
        OR_OPTION_CHANGED,          /**< Đã thay đổi tùy chọn trong hộp thoại tùy chọn */
        OR_OPTION_UNCHANGED,        /**< Không thay đổi tùy chọn trong hộp thoại tùy chọn */
        OR_OPTION_NOT_PROVIDED      /**< Không cung cấp hộp thoại tùy chọn */
    };

    /**
     * @brief   Chương trình chính gọi hàm này để mở hộp thoại tùy chọn của plugin
     * @detail  Không bắt buộc phải ghi đè hàm này. Nếu plugin có giao diện tùy chọn thì nên ghi đè hàm này và cuối cùng trả về OR_OPTION_CHANGED hoặc OR_OPTION_UNCHANGED.
     * @param   void * hParent Handle cửa sổ cha
     *  Nếu giá trị trả về là OR_OPTION_NOT_PRVIDED thì coi như plugin không cung cấp hộp thoại tùy chọn.
     * @return  ITMPlugin::OptionReturn
     */
    virtual OptionReturn ShowOptionsDialog(void* hParent) { return OR_OPTION_NOT_PROVIDED; }

    /** Chỉ số thông tin plugin */
    enum PluginInfoIndex
    {
        TMI_NAME,           /**< Tên */
        TMI_DESCRIPTION,    /**< Mô tả */
        TMI_AUTHOR,         /**< Tác giả */
        TMI_COPYRIGHT,      /**< Bản quyền */
        TMI_VERSION,        /**< Phiên bản */
        TMI_URL,            /**< Trang chủ */
        TMI_MAX             /**< Giá trị lớn nhất của chỉ số thông tin plugin */
    };

    /**
     * @brief   Lấy thông tin của plugin này, trả về thông tin tương ứng theo giá trị index
     */
    virtual const wchar_t* GetInfo(PluginInfoIndex index) = 0;

    /** Thông tin giám sát của chương trình chính */
    struct MonitorInfo
    {
        unsigned long long up_speed{};
        unsigned long long down_speed{};
        int cpu_usage{};
        int memory_usage{};
        int gpu_usage{};
        int hdd_usage{};
        int cpu_temperature{};
        int gpu_temperature{};
        int hdd_temperature{};
        int main_board_temperature{};
        int cpu_freq{};
    };

    /**
     * @brief   Chương trình chính gọi hàm này để truyền tất cả thông tin giám sát lấy được cho plugin
     */
    virtual void OnMonitorInfo(const MonitorInfo& monitor_info) {}

    /**
     * @brief   Lấy văn bản plugin muốn hiển thị trong chú giải chuột
     */
    virtual const wchar_t* GetTooltipInfo() { return L""; }

    enum ExtendedInfoIndex
    {
        EI_LABEL_TEXT_COLOR,    //Màu chữ của văn bản nhãn khi vẽ
        EI_VALUE_TEXT_COLOR,    //Màu chữ của văn bản giá trị khi vẽ
        EI_DRAW_TASKBAR_WND,    //Có vẽ cửa sổ thanh tác vụ hay không

        //Tùy chọn cửa sổ chính
        EI_NAIN_WND_NET_SPEED_SHORT_MODE,   //Chế độ hiển thị tốc độ mạng rút gọn
        EI_MAIN_WND_SPERATE_WITH_SPACE,     //Giá trị và đơn vị cách nhau bằng khoảng trắng
        EI_MAIN_WND_UNIT_BYTE,              //Đơn vị tốc độ mạng có dùng B (byte) hay không
        EI_MAIN_WND_UNIT_SELECT,            //Chọn đơn vị tốc độ mạng (0: tự động, 1: cố định KB/s, 2: cố định MB/s)
        EI_MAIN_WND_NOT_SHOW_UNIT,          //Không hiển thị đơn vị tốc độ mạng
        EI_MAIN_WND_NOT_SHOW_PERCENT,       //Không hiển thị ký hiệu phần trăm

        //Thiết lập cửa sổ thanh tác vụ
        EI_TASKBAR_WND_NET_SPEED_SHORT_MODE,    //Chế độ hiển thị tốc độ mạng rút gọn
        EI_TASKBAR_WND_SPERATE_WITH_SPACE,      //Giá trị và đơn vị cách nhau bằng khoảng trắng
        EI_TASKBAR_WND_VALUE_RIGHT_ALIGN,       //Giá trị căn phải
        EI_TASKBAR_WND_NET_SPEED_WIDTH,         //Độ rộng dữ liệu tốc độ mạng
        EI_TASKBAR_WND_UNIT_BYTE,               //Đơn vị tốc độ mạng có dùng B (byte) hay không
        EI_TASKBAR_WND_UNIT_SELECT,             //Chọn đơn vị tốc độ mạng (0: tự động, 1: cố định KB/s, 2: cố định MB/s)
        EI_TASKBAR_WND_NOT_SHOW_UNIT,           //Không hiển thị đơn vị tốc độ mạng
        EI_TASKBAR_WND_NOT_SHOW_PERCENT,        //Không hiển thị ký hiệu phần trăm

        EI_CONFIG_DIR,                      //Thư mục tệp cấu hình
    };

    /**
     * @brief   Chương trình chính gọi hàm này để truyền thêm thông tin cho plugin
     * @param   ExtendedInfoIndex index Chỉ số thông tin, dùng để phân biệt thông tin truyền cho plugin
     * @param   const wchar_t* data Dữ liệu được truyền
     * @return  void
     */
    virtual void OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data) {}

    /**
     * @brief   Lấy biểu tượng của plugin, định dạng HICON
     */
    virtual void* GetPluginIcon() { return nullptr; }

    /**
     * @brief   Lấy số lượng lệnh của plugin
     * @return  int Số lượng lệnh của plugin
     */
    virtual int GetCommandCount() { return 0; }

    /**
     * @brief   Lấy tên lệnh của plugin
     * @param   int command_index Chỉ số lệnh plugin (bắt đầu từ 0, nhỏ hơn giá trị trả về của GetCommandCount())
     * @return  wchar_t* Tên lệnh plugin
     */
    virtual const wchar_t* GetCommandName(int command_index) { return nullptr; }

    /**
     * @brief   Lấy biểu tượng lệnh của plugin
     * @param   int command_index Chỉ số lệnh plugin (bắt đầu từ 0, nhỏ hơn giá trị trả về của GetCommandCount())
     * @return  void* Biểu tượng lệnh plugin, định dạng HICON
     */
    virtual void* GetCommandIcon(int command_index) { return nullptr; }

    /**
     * @brief   Được framework gọi khi thực thi một lệnh của plugin
     * @param   int command_index Chỉ số lệnh plugin (bắt đầu từ 0, nhỏ hơn giá trị trả về của GetCommandCount())
     * @param   void* hWnd Handle cửa sổ gửi lệnh này
     * @param   void* para Tham số dự phòng
     */
    virtual void OnPluginCommand(int command_index, void* hWnd, void* para) {}

    /**
    * @brief   Lấy trạng thái đánh dấu của lệnh plugin
    * @param   int command_index Chỉ số lệnh plugin (bắt đầu từ 0, nhỏ hơn giá trị trả về của GetCommandCount())
    * @return  1: đã đánh dấu; 0: chưa đánh dấu
    */
    virtual int IsCommandChecked(int command_index) { return false; }

    /**
     * @brief   Khởi tạo plugin
     * @detail  Được gọi khi plugin được nạp, truyền con trỏ giao diện ITsudevTrafficMonitor. Plugin có thể lưu con trỏ này để gọi các hàm trong giao diện ITsudevTrafficMonitor
     * @param   pApp
     */
    virtual void OnInitialize(ITsudevTrafficMonitor* pApp) {}
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
//Giao diện chương trình chính
class ITsudevTrafficMonitor
{
public:
    /**
     * @brief   Lấy phiên bản của giao diện này.
     * @attention Khi plugin gọi các hàm trong ITsudevTrafficMonitor cần kiểm tra phiên bản giao diện trước
     * @return  int
     */
    virtual int GetAPIVersion() = 0;

    /**
     * @brief   Lấy phiên bản tsudev-trafficmonitor.
     * @return  const wchar_t*
     */
    virtual const wchar_t* GetVersion() = 0;

    /** Tất cả thông tin giám sát của chương trình chính */
    enum MonitorItem
    {
        MI_UP,                  /**< Tốc độ tải lên */
        MI_DOWN,                /**< Tốc độ tải xuống */
        MI_CPU,                 /**< Mức sử dụng CPU */
        MI_MEMORY,              /**< Mức sử dụng bộ nhớ */
        MI_GPU_USAGE,           /**< Mức sử dụng GPU */
        MI_CPU_TEMP,            /**< Nhiệt độ CPU */
        MI_GPU_TEMP,            /**< Nhiệt độ GPU */
        MI_HDD_TEMP,            /**< Nhiệt độ ổ cứng */
        MI_MAIN_BOARD_TEMP,     /**< Nhiệt độ bo mạch chủ */
        MI_HDD_USAGE,           /**< Mức sử dụng ổ cứng */
        MI_CPU_FREQ,            /**< Tần số CPU */
        MI_TODAY_UP_TRAFFIC,    /**< Lưu lượng tải lên hôm nay */
        MI_TODAY_DOWN_TRAFFIC   /**< Lưu lượng tải xuống hôm nay */
    };

    /**
     * @brief   Lấy giá trị của một thông tin giám sát của chương trình chính
     *          (ITMPlugin::OnMonitorInfo sẽ bị ngừng sử dụng)
     * @param   item Mục cần lấy thông tin giám sát
     * @return  Thông tin giám sát lấy được
     */
    virtual double GetMonitorValue(MonitorItem item) = 0;

    /**
     * @brief   Lấy giá trị của một thông tin giám sát của chương trình chính dưới dạng chuỗi
     * @param   item Mục cần lấy thông tin giám sát
     * @param   is_main_window Có phải giá trị cho cửa sổ chính hay không
     * @return  Thông tin giám sát lấy được
     */
    virtual const wchar_t* GetMonitorValueString(MonitorItem item, int is_main_window = false) = 0;

    /**
     * @brief   Hiển thị một thông báo
     * @param   strMsg Nội dung thông báo cần hiển thị
     */
    virtual void ShowNotifyMessage(const wchar_t* strMsg) = 0;

    /**
     * @brief   Lấy id ngôn ngữ hiện tại
     * @return  id ngôn ngữ hiện tại của chương trình chính
     */
    virtual unsigned short GetLanguageId() const = 0;

    /**
     * @brief   Lấy thư mục tệp cấu hình
     * @return  Thư mục tệp cấu hình
     */
    virtual const wchar_t* GetPluginConfigDir() const = 0;

    /** Kiểu DPI của chương trình chính */
    enum DPIType
    {
        DPI_MAIN_WND,   /**< DPI của cửa sổ chính */
        DPI_TASKBAR     /**< DPI của cửa sổ thanh tác vụ */
    };

    /**
     * @brief   Lấy DPI của chương trình chính
     * @return  DPI của chương trình chính
     */
    virtual int GetDPI(DPIType type) const = 0;

    /**
     * @brief   Lấy màu chủ đề hiện tại của hệ thống
     * @return  Giá trị màu định dạng COLORREF
     */
    virtual unsigned int GetThemeColor() const = 0;

    /**
     * @brief   Lấy tài nguyên chuỗi của ngôn ngữ hiện tại
     * @param   str_id id tài nguyên chuỗi
     * @param   str_section Tên section của tài nguyên chuỗi trong tệp ini
     * @return 
     */
    virtual const wchar_t* GetStringRes(const wchar_t* key, const wchar_t* section) = 0;

    /**
     * @brief   Lấy handle của cửa sổ chính
     * @return  
     */
    virtual void* GetMainWindowHwnd() = 0;

    /**
     * @brief   Lấy handle của cửa sổ thanh tác vụ. Khi không hiển thị cửa sổ thanh tác vụ thì giá trị trả về rỗng
     * @return  
     */
    virtual void* GetTaskbarWindowHwnd() = 0;

    /**
     * API Version
     *      1       Bổ sung hàm GetMainWindowHwnd, GetTaskbarWindowHwnd
     */
};


//Giao diện vẽ cho plugin
class IPluginDrawer
{
public:
    /**
     * @brief   Lấy phiên bản của giao diện này.
     * @return  int
     */
    virtual int GetAPIVersion() = 0;

    // Chế độ co giãn
    enum StretchMode
    {
        STRETCH,    /**< Co giãn, sẽ thay đổi tỉ lệ */
        FILL,       /**< Lấp đầy, giữ nguyên tỉ lệ, sẽ cắt bớt cạnh dài */
        FIT         /**< Vừa khít, giữ nguyên tỉ lệ, không cắt */
    };

    //Kiểu căn chỉnh
    enum Alignment
    {
        LEFT,       /**< Căn trái */
        RIGHT,      /**< Căn phải */
        CENTER,     /**< Căn giữa */
    };

    /**
     * @brief   Vẽ văn bản trong vùng hình chữ nhật chỉ định
     * @param   x Tọa độ cạnh trái của vùng hình chữ nhật
     * @param   y Tọa độ cạnh trên của vùng hình chữ nhật
     * @param   w Chiều rộng vùng hình chữ nhật
     * @param   h Chiều cao vùng hình chữ nhật
     * @param   lpszString Văn bản cần vẽ
     * @param   color Màu văn bản, định dạng 0xBBGGRR
     * @param   align Kiểu căn chỉnh văn bản
     * @param   multi_line Có hiển thị nhiều dòng hay không
     * @param   alpha Độ mờ đục, 0~255
     */
    virtual void DrawWindowText(int x, int y, int w, int h, const wchar_t* lpszString, unsigned long color, Alignment align = Alignment::LEFT, bool multi_line = false, unsigned char alpha = 255) = 0;

    /**
     * @brief   Thiết lập vùng cắt (clip) khi vẽ
     * @param   x Tọa độ cạnh trái của vùng hình chữ nhật
     * @param   y Tọa độ cạnh trên của vùng hình chữ nhật
     * @param   w Chiều rộng vùng hình chữ nhật
     * @param   h Chiều cao vùng hình chữ nhật
     */
    virtual void SetDrawRect(int x, int y, int w, int h) = 0;

    /**
     * @brief   Tô hình chữ nhật bằng màu đơn sắc
     * @param   x Tọa độ cạnh trái của vùng hình chữ nhật
     * @param   y Tọa độ cạnh trên của vùng hình chữ nhật
     * @param   w Chiều rộng vùng hình chữ nhật
     * @param   h Chiều cao vùng hình chữ nhật
     * @param   color Màu tô, định dạng 0xBBGGRR
     * @param   alpha Độ mờ đục, 0~255
     */
    virtual void FillRect(int x, int y, int w, int h, unsigned long color, unsigned char alpha = 255) = 0;
    
    /**
     * @brief   Vẽ viền hình chữ nhật
     * @param   x Tọa độ cạnh trái của vùng hình chữ nhật
     * @param   y Tọa độ cạnh trên của vùng hình chữ nhật
     * @param   w Chiều rộng vùng hình chữ nhật
     * @param   h Chiều cao vùng hình chữ nhật
     * @param   color Màu viền, định dạng 0xBBGGRR
     * @param   width Độ dày nét viền
     * @param   dot_line Có phải nét đứt hay không
     * @param   alpha Độ mờ đục, 0~255
     * @param   radius Bán kính bo góc, nếu bằng 0 thì là hình chữ nhật góc vuông
     */
    virtual void DrawRectOutLine(int x, int y, int w, int h, unsigned long color, int width = 1, bool dot_line = false, unsigned char alpha = 255, int radius = 0) = 0;

    /**
     * @brief   Vẽ một bitmap
     * @param   hbitmap Handle của bitmap
     * @param   x Tọa độ cạnh trái của vùng hình chữ nhật
     * @param   y Tọa độ cạnh trên của vùng hình chữ nhật
     * @param   w Chiều rộng vùng hình chữ nhật
     * @param   h Chiều cao vùng hình chữ nhật
     * @param   stretch_mode Chế độ co giãn khi vẽ
     * @param   alpha Độ mờ đục, 0~255
     */
    virtual void DrawBitmap(void* hbitmap, int x, int y, int w, int h, StretchMode stretch_mode = StretchMode::STRETCH, unsigned char alpha = 255) = 0;

    /**
     * @brief	Vẽ một biểu tượng
     * @param	hIcon Handle của biểu tượng
     * @param   x Tọa độ cạnh trái của vùng hình chữ nhật
     * @param   y Tọa độ cạnh trên của vùng hình chữ nhật
     * @param   w Chiều rộng vùng hình chữ nhật
     * @param   h Chiều cao vùng hình chữ nhật
     */
    virtual void DrawIcon(void* hIcon, int x, int y, int w, int h) = 0;

    /**
     * @brief   Tính chiều rộng và chiều cao của một chuỗi
     * @param   lpszString Chuỗi
     * @param[out]  w Chiều rộng của chuỗi
     * @param[out]  h Chiều cao của chuỗi
     */
    virtual void GetTextExtent(const wchar_t* lpszString, int& w, int& h) = 0;

    /**
     * @brief   Vẽ một đoạn thẳng
     * @param   x1 Tọa độ x của điểm thứ 1
     * @param   y1 Tọa độ y của điểm thứ 1
     * @param   x2 Tọa độ x của điểm thứ 2
     * @param   y2 Tọa độ y của điểm thứ 2
     * @param   color Màu
     * @param   alpha Độ mờ đục
     */
    virtual void DrawLine(int x1, int y1, int x2, int y2, unsigned long color, int width = 1, bool dot_line = false, unsigned char alpha = 255) = 0;

    /**
     * @brief   Lấy handle vẽ GDI
     * @return  Handle định dạng HDC
     */
    virtual void* GetHDC() = 0;
};


/*
* Lưu ý: dll plugin cần export các hàm sau
* ITMPlugin* TMPluginGetInstance();
*   Hàm trả về con trỏ tới một đối tượng có giao diện ITMPlugin.
*   Đối tượng này thường nên là đối tượng toàn cục hoặc tĩnh, không được giải phóng trước khi chương trình kết thúc.
*/


/*
* Lịch sử cập nhật:
* ITMPlugin
* -------------------------------------------------------------------------
* API version |                       Nội dung cập nhật
* -------------------------------------------------------------------------
*     1       | Phiên bản đầu tiên
* -------------------------------------------------------------------------
*     2       | Bổ sung hàm ITMPlugin::GetTooltipInfo
* -------------------------------------------------------------------------
*     3       | Bổ sung hàm IPluginItem::GetItemWidthEx, IPluginItem::OnMouseEvent
* -------------------------------------------------------------------------
*     4       | Bổ sung hàm IPluginItem::OnKeboardEvent IPluginItem::OnItemInfo
* -------------------------------------------------------------------------
*     5       | Bổ sung hàm ITMPlugin::GetCommandName ITMPlugin::GetCommandIcon ITMPlugin::OnPluginCommand
* -------------------------------------------------------------------------
*     6       | Bổ sung hàm IPluginItem::GetResourceUsageGraphType IPluginItem::GetResourceUsageGraphValue
* -------------------------------------------------------------------------
*     7       | Bổ sung hàm ITMPlugin::OnInitialize
* -------------------------------------------------------------------------
*     8       | Bổ sung hàm IPluginItem::DrawItemEx, IPluginItem::IsDoubleLineExclusive
* -------------------------------------------------------------------------
*/
