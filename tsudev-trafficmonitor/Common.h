#pragma once
#include "CommonData.h"
#include "CVariant.h"
#include <set>
#include <functional>

class CCommon
{
public:
    CCommon();
    ~CCommon();
    //Chuyển chuỗi const char* thành chuỗi ký tự rộng
    static wstring StrToUnicode(const char* str, bool utf8 = false);

    static string UnicodeToStr(const wchar_t* wstr, bool utf8 = false);

    static wstring AsciiToUnicode(const string& str);
    static string AsciiToStr(const std::wstring& wstr);

    static void StringNormalize(std::string& str);
    static void StringNormalize(std::wstring& str);

    //Tách một chuỗi thành nhiều chuỗi con (kiểu template chỉ có thể là string hoặc wstring)
    //str: chuỗi gốc
    //div_ch: ký tự dùng để tách
    //result: nhận kết quả sau khi tách
    static void StringSplit(const std::string& str, char div_ch, vector<std::string>& results, bool skip_empty = true, bool trim = true);
    static void StringSplit(const std::wstring& str, wchar_t div_ch, vector<std::wstring>& results, bool skip_empty = true, bool trim = true);

    static void StringSplit(const std::string& str, const std::string& div_str, vector<std::string>& results, bool skip_empty = true, bool trim = true);
    static void StringSplit(const std::wstring& str, const std::wstring& div_str, vector<std::wstring>& results, bool skip_empty = true, bool trim = true);

    static bool StringTransform(std::string& str, bool upper);
    static bool StringTransform(std::wstring& str, bool upper);

    //Đọc nội dung tệp
    static bool GetFileContent(const wchar_t* file_path, string& contents_buff, bool binary = true);

    //Đọc nội dung tệp
    //file_path: đường dẫn tệp
    //length: độ dài tệp
    //binary: có đọc ở chế độ nhị phân hay không
    //Giá trị trả về: thành công thì trả về con trỏ const char tới nội dung tệp đã đọc, sau khi dùng xong cần tự giải phóng bằng delete. Thất bại thì trả về nullptr
    static const char* GetFileContent(const wchar_t* file_path, size_t& length, bool binary = true);

    /*Chuyển kích thước dữ liệu thành chuỗi theo đơn vị KB, MB, GB
    size: số byte của dữ liệu
    Giá trị trả về: chuỗi sau khi chuyển đổi
    */
    static CString DataSizeToString(unsigned long long size, const PublicSettingData& cfg);

    /*Chuyển kích thước dữ liệu thành chuỗi theo đơn vị KB, MB, GB
    size: số byte của dữ liệu
    with_space: giá trị và đơn vị có cách nhau bằng khoảng trắng hay không
    Giá trị trả về: chuỗi sau khi chuyển đổi
    */
    static CString DataSizeToString(unsigned long long size, bool with_space = true);

    //Chuyển thông tin nhiệt độ thành chuỗi
    static CString TemperatureToString(float temperature, const PublicSettingData& cfg);

    //Chuyển mức sử dụng thành chuỗi
    static CString UsageToString(int usage, const PublicSettingData& cfg);
    static CString FreqToString(float usage, const PublicSettingData& cfg);
    //static CString KBytesToString(unsigned int kb_size);
    static CString KBytesToString(unsigned __int64 kb_size);

    //Trả về hiệu thời gian giữa hai cấu trúc FILETIME
    static __int64 CompareFileTime2(FILETIME time1, FILETIME time2);

    //Ghi thông tin nhật ký str_text vào tệp file_path
    static void WriteLog(const char* str_text, LPCTSTR file_path);
    static void WriteLog(const wchar_t* str_text, LPCTSTR file_path);

    /*
    Chức năng: tạo lối tắt cho tệp chỉ định trong thư mục chỉ định
    Tham số:
    lpszLnkFileDir  Thư mục chỉ định, không được là NULL.
    lpszFileName    Tệp chỉ định, NULL nghĩa là tệp EXE của tiến trình hiện tại.
    lpszLnkFileName Tên lối tắt, NULL nghĩa là dùng tên tệp EXE.
    wHotkey         0 nghĩa là không đặt phím tắt
    pszDescription  Ghi chú
    iShowCmd        Kiểu chạy, mặc định là cửa sổ thường
    */
    static BOOL CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName = NULL, LPCTSTR lpszLnkFileName = NULL, LPCTSTR lpszWorkDir = NULL, WORD wHotkey = 0, LPCTSTR lpszDescription = NULL, int iShowCmd = SW_SHOWNORMAL);

    //Lấy đường dẫn thư mục “Startup” trong “All Programs” của menu Start
    static wstring GetStartUpPath();

    //Lấy các tệp hoặc thư mục trong đường dẫn path và lưu tên vào container files.
    static void GetFiles(const wchar_t* path, vector<wstring>& files);

    //Lấy các tệp hoặc thư mục trong đường dẫn path, mỗi lần duyệt sẽ gọi đối tượng hàm func
    //path: đường dẫn tìm kiếm
    //func: có thể là đối tượng hàm hoặc biểu thức lambda, tham số là tên tệp hoặc thư mục được duyệt tới
    static void GetFiles(const wchar_t* path, std::function<void(const wstring&)> func);

    //Kiểm tra một tệp có tồn tại hay không
    static bool FileExist(LPCTSTR file_name);

    //Kiểm tra có phải thư mục hay không
    static bool IsFolder(const wstring& path);

    static bool MoveAFile(LPCTSTR exist_file, LPCTSTR new_file);

    //Tính hiệu thời gian giữa hai cấu trúc SYSTEMTIME (a-b, chỉ giữ giờ, phút, giây)
    static SYSTEMTIME CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b);

    //Lấy số mili giây từ ngày 1/1/1970 đến hiện tại
    static ULONGLONG GetCurrentTimeSinceEpochMilliseconds();

    //Lấy thư mục của chương trình hiện tại
    static wstring GetModuleDir();

    //Lấy đường dẫn thư mục system32
    static wstring GetSystemDir();

    //Lấy đường dẫn thư mục tạm
    static wstring GetTemplateDir();

    //Lấy thư mục Appdata/Local/tsudev-trafficmonitor, tự động tạo nếu chưa tồn tại
    static wstring GetAppDataConfigDir();

    //Vẽ văn bản tại vị trí chỉ định
    static void DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color);

    ////Thiết lập vùng cắt (clip) khi vẽ
    //static void SetDrawArea(CDC* pDC, CRect rect);

    /**
     * Kiểm tra cửa sổ tiền cảnh đang tương tác với người dùng có toàn màn hình hay không
     * @param[in] hMonitor Màn hình cần kiểm tra (nếu rỗng thì dùng màn hình chính)
     * @return 
     */
    static bool IsForegroundFullscreen(HMONITOR hMonitor = NULL);

    //Lưu một chuỗi vào clipboard
    static bool CopyStringToClipboard(const wstring& str);

    static wstring GetJsonValueSimple(const wstring& json_str, const wstring& name);

    //Lấy nội dung của URL
    //Bảo mật: giới hạn thứ tự tìm DLL của tiến trình (bỏ thư mục hiện hành và PATH) để chống DLL hijacking.
    //Gọi một lần, càng sớm càng tốt khi khởi động chương trình.
    static void HardenDllSearchOrder();

    //Bảo mật: nạp một DLL theo đường dẫn tuyệt đối, DLL phụ thuộc chỉ được tìm trong thư mục của DLL đó và các thư mục hệ thống
    static HMODULE SafeLoadLibrary(LPCWSTR full_path);

    //Kiểm tra URL có phải HTTPS hợp lệ hay không (bảo mật: GetURL chỉ chấp nhận HTTPS)
    static bool IsHttpsUrl(const wstring& url);
    //Kiểm tra URL là HTTPS và host nằm trong danh sách máy chủ được phép (FORK_TRUSTED_HOSTS)
    static bool IsTrustedHostUrl(const wstring& url);

    //Kiểm tra chuỗi có đúng dạng địa chỉ IPv4/IPv6 hay không
    static bool IsValidIpAddressString(const wstring& ip);

    //Lấy địa chỉ IP từ phản hồi của dịch vụ tra cứu IP: chấp nhận cả dạng JSON {"ip":"..."} lẫn văn bản thuần "1.2.3.4".
    //Trả về chuỗi rỗng nếu không hợp lệ.
    static wstring ParseIpServiceResponse(const wstring& response);

    static bool GetURL(const wstring& url, std::string& result, const wstring& user_agent = wstring());
    static bool GetURL(const wstring& url, wstring& result, bool utf8 = false, const wstring& user_agent = wstring());

    //Lấy địa chỉ IP công cộng (Internet) và vị trí địa lý của IP
    static void GetInternetIp(wstring& ip_address, wstring& ip_location, bool global);

    //Kết quả tra cứu IP công cộng
    enum class PublicIpResult
    {
        OK,                 //Lấy được địa chỉ IP
        NO_CONNECTION,      //Máy không có kết nối loại này (ví dụ: không có địa chỉ IPv6 toàn cầu) nên không cần truy vấn
        FAILED              //Có kết nối nhưng truy vấn thất bại (mạng lỗi, dịch vụ không phản hồi, dữ liệu không hợp lệ)
    };
    static PublicIpResult GetInternetIp2(wstring& ip_address, wstring& ip_location, bool ipv6);

    static void SetRect(CRect& rect, int x, int y, int width, int height);

    //Tải chuỗi từ tệp tài nguyên. front_str, back_str là chuỗi cần thêm vào trước hoặc sau khi tải
    static CString LoadText(const wchar_t* id, LPCTSTR back_str = nullptr);
    static CString LoadText(LPCTSTR front_str, const wchar_t* id, LPCTSTR back_str);

    //Định dạng chuỗi an toàn: thay các chuỗi dạng <%số thứ tự%> trong format_str bằng phần tử trong danh sách khởi tạo paras; phần tử hỗ trợ int/double/LPCTSTR/CString, số thứ tự bắt đầu từ 1
    static CString StringFormat(LPCTSTR format_str, const std::initializer_list<CVariant>& paras);

    //Tải chuỗi từ tệp tài nguyên và thay các chuỗi dạng <%số thứ tự%> bằng tham số trong danh sách tham số biến đổi
    static CString LoadTextFormat(const wchar_t* id, const std::initializer_list<CVariant>& paras);

    //Chuyển kiểu int thành chuỗi
    //n: giá trị cần chuyển đổi
    //thousand_separation: có dùng dấu phẩy phân cách mỗi 3 chữ số hay không
    //is_unsigned: giá trị có phải số không dấu hay không
    static CString IntToString(__int64 n, bool thousand_separation = false, bool is_unsigned = false);

    //Bỏ các chuỗi Bold, Light... ở cuối tên phông chữ và thiết lập độ đậm phông chữ theo các chuỗi đó
    static void NormalizeFont(LOGFONT& font);

    //Hàm sao chép chuỗi an toàn
    static void WStringCopy(wchar_t* str_dest, int dest_size, const wchar_t* str_source, int source_size = INT_MAX);

    //Thay thế chuỗi
    static bool StringReplace(wstring& str, const wstring& str_old, const wstring& str_new);

    /// <summary>
    /// Thuật toán độ tương đồng chuỗi - phương pháp khoảng cách chỉnh sửa (edit distance)
    /// </summary>
    /// <returns>Giá trị trả về trong khoảng 0~1, càng lớn thì càng giống nhau</returns>
    static double StringSimilarDegree_LD(const std::string& srcString, const std::string& matchString);
    static double StringSimilarDegree_LD(const std::wstring& srcString, const std::wstring& matchString);
    
    //Thiết lập ngôn ngữ cho luồng
    static void SetThreadLanguage(WORD language);

    //Thiết lập chế độ màu
    static void SetColorMode(ColorMode mode);

    //Qua thử nghiệm nhận thấy khi màu trong suốt có R và B bằng nhau thì khi nhấp chuột phải vào cửa sổ thanh tác vụ sẽ không hiện menu chuột phải của chương trình mà hiện menu chuột phải của thanh tác vụ hệ thống
    //Để giải quyết vấn đề này, cần chuyển đổi giá trị màu
    //Hàm này kiểm tra giá trị R và B của một màu có bằng nhau không; nếu có thì tăng giá trị B thêm 1 (nếu B==255 thì giảm 1)
    static void TransparentColorConvert(COLORREF& transparent_color);

    static void SetDialogFont(CWnd* pDlg, CFont* pFont);

    //Tải tài nguyên văn bản tùy chỉnh. id: ID tài nguyên, code_type: mã hóa văn bản: 0:ANSI, 1:UTF8, 2:UTF16
    static CString GetTextResource(UINT id, int code_type);

    //Đọc thời gian biên dịch gần nhất từ tệp tài nguyên
    static CString GetLastCompileTime();

    //Tải một biểu tượng từ tài nguyên
    static HICON LoadIconResource(UINT id, int size);

    //Lấy số thứ tự của một mục menu
    static int GetMenuItemPosition(CMenu* pMenu, UINT id);

    //Tải một menu từ tệp tài nguyên và xử lý dịch văn bản
    static void LoadMenuResource(CMenu& menu, UINT res_id);

    static bool IsColorSimilar(COLORREF color1, COLORREF color2);

    //Đếm số bit 1 trong biểu diễn nhị phân
    static int CountOneBits(unsigned int value);

    //Thiết lập một bit nào đó của một số
    static void SetNumberBit(unsigned int& num, int bit, bool value);

    //Lấy một bit nào đó của một số
    static bool GetNumberBit(unsigned int num, int bit);

    //Xóa phần tử trùng lặp trong vector mà không sắp xếp
    template<class T>
    static void RemoveVectorDuplicateItem(vector<T>& vec)
    {
        std::set<T> si;
        for (auto it = vec.begin(); it != vec.end();)
        {
            if (si.count(*it) == 0)//Kiểm tra phần tử hiện tại đã xuất hiện chưa; nếu chưa thì giữ lại phần tử và đánh dấu
            {
                si.insert(*it);
                ++it;
            }
            else//Nếu đã xuất hiện trước đó thì xóa phần tử hiện tại
            {
                it = vec.erase(it);//Xóa phần tử mà it đang trỏ tới
            }
        }
    }

    /**
     * @brief   Giới hạn phạm vi của một số
     * @param   value Giá trị cần giới hạn phạm vi
     * @param   min_value Giá trị nhỏ nhất
     * @param   max_value Giá trị lớn nhất
     */
    template<class T>
    static void ValidatValue(T& value, const T& min_value, const T& max_value)
    {
        if (value < min_value)
            value = min_value;
        if (value > max_value)
            value = max_value;
    }

    //Lấy màu chủ đề Windows
    static COLORREF GetWindowsThemeColor();

    static CString GetErrorMessage(DWORD error_code);
};

/**
 * @brief Lấy độ dài mảng tại thời điểm biên dịch
 *
 * @tparam T Kiểu phần tử mảng
 * @tparam N Độ dài mảng suy ra lúc biên dịch
 * @return constexpr std::size_t Độ dài mảng suy ra lúc biên dịch
 */
template <typename T, std::size_t N>
constexpr std::size_t GetArrayLength(const T (&)[N]) noexcept
{
    return N;
}

/**
 * @brief Hàm mặc định chạy trước khi hủy đối tượng bọc StaticVariableWrapper, thực tế không làm gì
 *
 * @tparam T
 */
template <class T>
class CDefaultStaticVariableWrapperDtor
{
public:
    void operator()(T*){};
};
/**
 * @brief Thiết kế cho lớp bọc biến tĩnh, dùng để tùy biến hành vi sau khởi tạo mặc định và trước khi hủy biến
 *
 * @tparam T Kiểu cần được bọc
 * @tparam DTOR Hành vi tùy biến trước khi chạy hàm hủy
 */
template <class T, class DTOR = CDefaultStaticVariableWrapperDtor<T>>
class CStaticVariableWrapper : private DTOR
{
private:
    T m_content;

public:
    /**
     * @brief Tạo một StaticVariableWrapper
     *
     * @tparam CTOR Kiểu hàm tùy biến chạy sau khi biến được khởi tạo mặc định
     * @param ctor Hành vi tùy biến sau khi biến được khởi tạo mặc định, nhận con trỏ biến làm tham số
     * @param dtor Hành vi tùy biến trước khi biến chạy hàm hủy, nhận con trỏ biến làm tham số
     */
    template <class CTOR>
    CStaticVariableWrapper(CTOR ctor, DTOR dtor = {})
        : DTOR{dtor}
    {
        ctor(std::addressof(m_content));
    }
    ~CStaticVariableWrapper()
    {
        (*static_cast<DTOR*>(this))(std::addressof(m_content));
    }
    T& Get() noexcept
    {
        return m_content;
    }
    const T& Get() const noexcept
    {
        return m_content;
    }
};
/**
 * @brief Hàm tạo lớp bọc biến tĩnh
 *
 * @tparam T Kiểu cần được bọc
 * @tparam CTOR Kiểu hàm tùy biến chạy sau khi biến được khởi tạo mặc định
 * @tparam DTOR Kiểu hàm tùy biến chạy trước khi biến gọi hàm hủy
 * @param ctor Hành vi tùy biến sau khi biến được khởi tạo mặc định, nhận con trỏ biến làm tham số
 * @param dtor Hành vi tùy biến trước khi biến chạy hàm hủy, nhận con trỏ biến làm tham số
 * @return CStaticVariableWrapper<T, DTOR> Biến đã được bọc và khởi tạo
 */
template <class T, class CTOR, class DTOR = CDefaultStaticVariableWrapperDtor<T>>
auto MakeStaticVariableWrapper(CTOR ctor, DTOR dtor = {})
    -> CStaticVariableWrapper<T, DTOR>
{
    return {ctor, dtor};
}

/**
 * @brief Gọi hàm hủy theo đúng kiểu của đối tượng mà con trỏ trỏ tới
 *
 * @tparam T Kiểu được truyền vào sau khi bỏ con trỏ
 * @param p_memory Con trỏ tới đối tượng cần chạy hàm hủy
 */
template <class T>
void Destroy(T* p_memory)
{
    p_memory->~T();
}

template <class T, class... Args>
void EmplaceAt(T* p_memory, Args&&... args)
{
    ::new (p_memory) T(std::forward<Args>(args)...);
}
