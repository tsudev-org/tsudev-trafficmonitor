#include "stdafx.h"
#include "Common.h"
#include "AdapterCommon.h"
#include "TrafficMonitor.h"


CCommon::CCommon()
{
}


CCommon::~CCommon()
{
}

wstring CCommon::StrToUnicode(const char* str, bool utf8)
{
    wstring result;
    int size;
    size = MultiByteToWideChar((utf8 ? CP_UTF8 : CP_ACP), 0, str, -1, NULL, 0);
    if (size <= 0) return wstring();
    wchar_t* str_unicode = new wchar_t[size + 1];
    MultiByteToWideChar((utf8 ? CP_UTF8 : CP_ACP), 0, str, -1, str_unicode, size);
    result.assign(str_unicode);
    delete[] str_unicode;
    return result;
}

string CCommon::UnicodeToStr(const wchar_t* wstr, bool utf8)
{
    string result;
    int size{ 0 };
    size = WideCharToMultiByte((utf8 ? CP_UTF8 : CP_ACP), 0, wstr, -1, NULL, 0, NULL, NULL);
    if (size <= 0) return string();
    char* str = new char[size + 1];
    WideCharToMultiByte((utf8 ? CP_UTF8 : CP_ACP), 0, wstr, -1, str, size, NULL, NULL);
    result.assign(str);
    delete[] str;
    return result;
}

wstring CCommon::AsciiToUnicode(const string& str)
{
    std::wstring result;
    result.resize(str.size());
    for (size_t i{}; i < str.size(); i++)
        result[i] = str[i];
    return result;
}

string CCommon::AsciiToStr(const std::wstring& wstr)
{
    std::string result;
    result.resize(wstr.size());
    for (size_t i{}; i < wstr.size(); i++)
        result[i] = static_cast<char>(wstr[i]);
    return result;
}

template<class T>
static void _StringNormalize(T& str)
{
    if (str.empty()) return;

    int size = static_cast<int>(str.size());  //Độ dài chuỗi
    if (size < 0) return;
    int index1 = 0;     //Vị trí đầu tiên trong chuỗi không phải khoảng trắng hoặc ký tự điều khiển
    int index2 = size - 1;  //Vị trí cuối cùng trong chuỗi không phải khoảng trắng hoặc ký tự điều khiển
    while (index1 < size && str[index1] >= 0 && str[index1] <= 32)
        index1++;
    while (index2 >= 0 && str[index2] >= 0 && str[index2] <= 32)
        index2--;
    if (index1 > index2)    //Nếu index1 > index2 nghĩa là chuỗi toàn là khoảng trắng hoặc ký tự điều khiển
        str.clear();
    else if (index1 == 0 && index2 == size - 1) //Nếu index1 và index2 lần lượt là 0 và size - 1 nghĩa là chuỗi không có khoảng trắng hay ký tự điều khiển ở đầu và cuối, trả về luôn
        return;
    else
        str = str.substr(index1, index2 - index1 + 1);
}

void CCommon::StringNormalize(std::string& str)
{
    _StringNormalize(str);
}

void CCommon::StringNormalize(std::wstring& str)
{
    _StringNormalize(str);
}

template<class T>
static void _StringSplit(const T& str, typename T::value_type div_ch, vector<T>& results, bool skip_empty = true, bool trim = true)
{
    results.clear();
    size_t split_index = -1;
    size_t last_split_index = -1;
    while (true)
    {
        split_index = str.find(div_ch, split_index + 1);
        T split_str = str.substr(last_split_index + 1, split_index - last_split_index - 1);
        if (trim)
            _StringNormalize(split_str);
        if (!split_str.empty() || !skip_empty)
            results.push_back(split_str);
        if (split_index == wstring::npos)
            break;
        last_split_index = split_index;
    }
}

void CCommon::StringSplit(const std::string& str, char div_ch, vector<std::string>& results, bool skip_empty, bool trim)
{
    _StringSplit(str, div_ch, results, skip_empty, trim);
}

void CCommon::StringSplit(const std::wstring& str, wchar_t div_ch, vector<std::wstring>& results, bool skip_empty, bool trim)
{
    _StringSplit(str, div_ch, results, skip_empty, trim);
}

template<class T>
static void _StringSplit(const T& str, const T& div_str, vector<T>& results, bool skip_empty = true, bool trim = true)
{
    results.clear();
    size_t split_index = 0 - div_str.size();
    size_t last_split_index = 0 - div_str.size();
    while (true)
    {
        split_index = str.find(div_str, split_index + div_str.size());
        T split_str = str.substr(last_split_index + div_str.size(), split_index - last_split_index - div_str.size());
        if (trim)
            _StringNormalize(split_str);
        if (!split_str.empty() || !skip_empty)
            results.push_back(split_str);
        if (split_index == wstring::npos)
            break;
        last_split_index = split_index;
    }
}

void CCommon::StringSplit(const std::string& str, const std::string& div_str, vector<std::string>& results, bool skip_empty, bool trim)
{
    _StringSplit(str, div_str, results, skip_empty, trim);
}

void CCommon::StringSplit(const std::wstring& str, const std::wstring& div_str, vector<std::wstring>& results, bool skip_empty, bool trim)
{
    _StringSplit(str, div_str, results, skip_empty, trim);
}

template<class T>
static bool _StringTransform(T& str, bool upper)
{
    if (str.empty()) return false;
    if (upper)
    {
        for (auto& ch : str)
        {
            {
                if (ch >= 'a' && ch <= 'z')
                    ch -= 32;
            }
        }
    }
    else
    {
        for (auto& ch : str)
        {
            if (ch >= 'A' && ch <= 'Z')
                ch += 32;
        }
    }
    return true;
}

bool CCommon::StringTransform(std::string& str, bool upper)
{
    return _StringTransform(str, upper);
}

bool CCommon::StringTransform(std::wstring& str, bool upper)
{
    return _StringTransform(str, upper);
}

bool CCommon::GetFileContent(const wchar_t* file_path, string& contents_buff, bool binary /*= true*/)
{
    std::ifstream file{ file_path, (binary ? std::ios::binary : std::ios::in) };
    if (file.fail())
        return false;
    //Lấy độ dài tệp; tellg trả về -1 khi lỗi (trước đây bị ép thành size_t rất lớn rồi cấp phát)
    file.seekg(0, file.end);
    const std::streamoff end_pos = file.tellg();
    if (end_pos < 0)
        return false;
    file.seekg(0, file.beg);

    contents_buff.assign(static_cast<size_t>(end_pos), '\0');
    if (end_pos > 0)
        file.read(&contents_buff[0], end_pos);
    contents_buff.resize(static_cast<size_t>(file.gcount()));
    return true;
}

const char* CCommon::GetFileContent(const wchar_t* file_path, size_t& length, bool binary /*= true*/)
{
    std::ifstream file{ file_path, (binary ? std::ios::binary : std::ios::in) };
    length = 0;
    if (file.fail())
        return nullptr;
    //Lấy độ dài tệp; tellg trả về -1 khi lỗi
    file.seekg(0, file.end);
    const std::streamoff end_pos = file.tellg();
    if (end_pos < 0)
        return nullptr;
    file.seekg(0, file.beg);

    char* buff = new char[static_cast<size_t>(end_pos) + 1];
    file.read(buff, end_pos);
    length = static_cast<size_t>(file.gcount());
    buff[length] = '\0';
    return buff;
}

CString CCommon::DataSizeToString(unsigned long long size, const PublicSettingData& cfg)
{
    //CString str;
    CString value_str, unit_str;
    if (!cfg.unit_byte)     //Nếu dùng đơn vị bit thì nhân giá trị với 8
    {
        size *= 8;
    }
    switch (cfg.speed_unit)
    {
    case SpeedUnit::AUTO:
        if (cfg.speed_short_mode)
        {
            if (size < 1024 * 10)                   //Dưới 10KB thì dùng đơn vị KB, giữ 1 chữ số thập phân
            {
                value_str.Format(_T("%.1f"), size / 1024.0f);
                unit_str = _T("K");
            }
            else if (size < 1024 * 1000)            //Dưới 1000KB thì dùng đơn vị KB, làm tròn số nguyên
            {
                value_str.Format(_T("%.0f"), size / 1024.0f);
                unit_str = _T("K");
            }
            else if (size < 1024 * 1024 * 1000)     //Dưới 1000MB thì dùng đơn vị MB, giữ 1 chữ số thập phân
            {
                value_str.Format(_T("%.1f"), size / 1024.0f / 1024.0f);
                unit_str = _T("M");
            }
            else
            {
                value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f / 1024.0f);
                unit_str = _T("G");
            }
        }
        else
        {
            if (size < 1024 * 10)                   //Dưới 10KB thì dùng đơn vị KB, giữ 2 chữ số thập phân
            {
                value_str.Format(_T("%.2f"), size / 1024.0f);
                unit_str = _T("KB");
            }
            else if (size < 1024 * 1000)            //Dưới 1000KB thì dùng đơn vị KB, giữ 1 chữ số thập phân
            {
                value_str.Format(_T("%.1f"), size / 1024.0f);
                unit_str = _T("KB");
            }
            else if (size < 1024 * 1024 * 1000)     //Dưới 1000MB thì dùng đơn vị MB, giữ 2 chữ số thập phân
            {
                value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f);
                unit_str = _T("MB");
            }
            else
            {
                value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f / 1024.0f);
                unit_str = _T("GB");
            }
        }
        break;
    case SpeedUnit::KBPS:
        if (cfg.speed_short_mode)
        {
            if (size < 1024 * 10)                   //Dưới 10KB giữ 1 chữ số thập phân
                value_str.Format(_T("%.1f"), size / 1024.0f);
            else                    //Từ 10KB trở lên làm tròn số nguyên
                value_str.Format(_T("%.0f"), size / 1024.0f);
            if (!cfg.hide_unit)
                unit_str = _T("K");
        }
        else
        {
            if (size < 1024 * 10)                   //Dưới 10KB giữ 2 chữ số thập phân
                value_str.Format(_T("%.2f"), size / 1024.0f);
            else            //Từ 10KB trở lên giữ 1 chữ số thập phân
                value_str.Format(_T("%.1f"), size / 1024.0f);
            if (!cfg.hide_unit)
                unit_str = _T("KB");
        }
        break;
    case SpeedUnit::MBPS:
        if (cfg.speed_short_mode)
        {
            value_str.Format(_T("%.1f"), size / 1024.0f / 1024.0f);
            if (!cfg.hide_unit)
                unit_str = _T("M");
        }
        else
        {
            value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f);
            if (!cfg.hide_unit)
                unit_str = _T("MB");
        }
        break;
    }
    CString str;
    if (cfg.separate_value_unit_with_space && !cfg.hide_unit)
        str = value_str + _T(' ') + unit_str;
    else
        str = value_str + unit_str;
    if (!cfg.unit_byte)
    {
        if (cfg.speed_short_mode && !cfg.hide_unit)
            str += _T('b');     //Nếu dùng đơn vị bit thì vẫn hiển thị “b” kể cả khi đã bật chế độ tốc độ mạng rút gọn
        else
            str.Replace(_T('B'), _T('b'));  //Nếu dùng đơn vị bit thì thay B bằng b
    }
    return str;
}

CString CCommon::DataSizeToString(unsigned long long size, bool with_space)
{
    CString str;
    if (size < 1024 * 10)                   //Dưới 10KB thì dùng đơn vị KB, giữ 2 chữ số thập phân
        str.Format(_T("%.2f KB"), size / 1024.0);
    else if (size < 1024 * 1024)            //Dưới 1MB thì dùng đơn vị KB, giữ 1 chữ số thập phân
        str.Format(_T("%.1f KB"), size / 1024.0);
    else if (size < 1024 * 1024 * 1024)     //Dưới 1GB thì dùng đơn vị MB, giữ 2 chữ số thập phân
        str.Format(_T("%.2f MB"), size / 1024.0 / 1024.0);
    else if (size < 1024ll * 1024 * 1024 * 1024)
        str.Format(_T("%.2f GB"), size / 1024.0 / 1024.0 / 1024.0);
    else
        str.Format(_T("%.2f TB"), size / 1024.0 / 1024.0 / 1024.0 / 1024.0);
    if (!with_space)
        str.Remove(_T(' '));
    return str;
}

CString CCommon::TemperatureToString(float temperature, const PublicSettingData& cfg)
{
    CString str_val;
    if (temperature <= 0)
        str_val = _T("--");
    else
        str_val.Format(_T("%d"), static_cast<int>(temperature));
    if (cfg.separate_value_unit_with_space)
        str_val += _T(' ');
    str_val += _T("°C");
    return str_val;
}

CString CCommon::UsageToString(int usage, const PublicSettingData& cfg)
{
    CString str_val;
    if (usage < 0)
        str_val = _T("--");
    else
        str_val.Format(_T("%d"), usage);
    if (!cfg.hide_percent)
    {
        if (cfg.separate_value_unit_with_space)
            str_val += _T(' ');
        str_val += _T('%');
    }
    return str_val;
}

CString CCommon::FreqToString(float freq, const PublicSettingData& cfg)
{
    CString str_val;
    if (freq < 0)
        str_val = _T("--");
    else
        str_val.Format(_T("%.2f"), freq);
    if (cfg.separate_value_unit_with_space)
        str_val += _T(' ');
    str_val += _T("GHz");
    return str_val;
}
//CString CCommon::KBytesToString(unsigned int kb_size)
//{
//  CString k_bytes_str;
//  if (kb_size < 1024)
//      k_bytes_str.Format(_T("%d KB"), kb_size);
//  else if (kb_size < 1024 * 1024)
//      k_bytes_str.Format(_T("%.2f MB"), kb_size / 1024.0);
//  else if (kb_size < 1024 * 1024 * 1024)
//      k_bytes_str.Format(_T("%.2f GB"), kb_size / 1024.0 / 1024.0);
//  else
//      k_bytes_str.Format(_T("%.2f TB"), kb_size / 1024.0 / 1024.0 / 1024.0);
//  return k_bytes_str;
//}

CString CCommon::KBytesToString(unsigned __int64 kb_size)
{
    CString k_bytes_str;
    if (kb_size < 1024)
        k_bytes_str.Format(_T("%d KB"), kb_size);
    else if (kb_size < 1024 * 1024)
        k_bytes_str.Format(_T("%.2f MB"), kb_size / 1024.0);
    else if (kb_size < 1024 * 1024 * 1024)
        k_bytes_str.Format(_T("%.2f GB"), kb_size / 1024.0 / 1024.0);
    else
        k_bytes_str.Format(_T("%.2f TB"), kb_size / 1024.0 / 1024.0 / 1024.0);
    return k_bytes_str;
}

__int64 CCommon::CompareFileTime2(FILETIME time1, FILETIME time2)
{
    __int64 a = static_cast<__int64>(time1.dwHighDateTime) << 32 | time1.dwLowDateTime;
    __int64 b = static_cast<__int64>(time2.dwHighDateTime) << 32 | time2.dwLowDateTime;
    return b - a;
}

void CCommon::WriteLog(const char* str_text, LPCTSTR file_path)
{
    SYSTEMTIME cur_time;
    GetLocalTime(&cur_time);
    char buff[32];
    sprintf_s(buff, "%d/%.2d/%.2d %.2d:%.2d:%.2d.%.3d: ", cur_time.wYear, cur_time.wMonth, cur_time.wDay,
        cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
    ofstream file{ file_path, std::ios::app };  //Mở tệp nhật ký ở chế độ ghi nối tiếp
    file << buff;
    file << str_text << std::endl;
}

void CCommon::WriteLog(const wchar_t* str_text, LPCTSTR file_path)
{
    SYSTEMTIME cur_time;
    GetLocalTime(&cur_time);
    char buff[32];
    sprintf_s(buff, "%d/%.2d/%.2d %.2d:%.2d:%.2d.%.3d: ", cur_time.wYear, cur_time.wMonth, cur_time.wDay,
        cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
    ofstream file{ file_path, std::ios::app };  //Mở tệp nhật ký ở chế độ ghi nối tiếp
    file << buff;
    file << UnicodeToStr(str_text).c_str() << std::endl;
}

BOOL CCommon::CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName, LPCTSTR lpszLnkFileName, LPCTSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd)
{
    if (lpszLnkFileDir == NULL)
        return FALSE;

    HRESULT hr;
    IShellLink* pLink;  //Con trỏ đối tượng IShellLink
    IPersistFile* ppf; //Con trỏ đối tượng IPersisFile

                         //Tạo đối tượng IShellLink
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
    if (FAILED(hr))
        return FALSE;

    //Lấy giao diện IPersistFile từ đối tượng IShellLink
    hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
    if (FAILED(hr))
    {
        pLink->Release();
        return FALSE;
    }

    TCHAR file_path[MAX_PATH];
    GetModuleFileName(NULL, file_path, MAX_PATH);

    //Đích
    if (lpszFileName == NULL)
        pLink->SetPath(file_path);
    else
        pLink->SetPath(lpszFileName);

    //Thư mục làm việc
    if (lpszWorkDir != NULL)
    {
        pLink->SetWorkingDirectory(lpszWorkDir);
    }
    else
    {
        //Đặt thư mục làm việc là thư mục chứa đích của lối tắt
        TCHAR workDirBuf[MAX_PATH]{};
        if (lpszFileName == NULL)
            //wcscpy_s(workDirBuf, file_path);
            WStringCopy(workDirBuf, 260, file_path, 260);
        else
            //wcscpy_s(workDirBuf, lpszFileName);
            WStringCopy(workDirBuf, 260, lpszFileName);
        LPTSTR pstr = wcsrchr(workDirBuf, _T('\\'));
        *pstr = _T('\0');
        pLink->SetWorkingDirectory(workDirBuf);
    }

    //Phím tắt
    if (wHotkey != 0)
        pLink->SetHotkey(wHotkey);

    //Ghi chú
    if (lpszDescription != NULL)
        pLink->SetDescription(lpszDescription);

    //Kiểu hiển thị
    pLink->SetShowCmd(iShowCmd);


    //Đường dẫn + tên của lối tắt
    wchar_t szBuffer[MAX_PATH];
    if (lpszLnkFileName != NULL) //Đã chỉ định tên lối tắt
        swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, lpszLnkFileName);
    else
    {
        //Không chỉ định tên thì lấy tên tệp của tệp chỉ định làm tên lối tắt.
        const wchar_t* pstr;
        if (lpszFileName != NULL)
            pstr = wcsrchr(lpszFileName, L'\\');
        else
            pstr = wcsrchr(file_path, L'\\');

        if (pstr == NULL)
        {
            ppf->Release();
            pLink->Release();
            return FALSE;
        }
        //Lưu ý đổi phần mở rộng từ .exe thành .lnk
        swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, pstr);
        int nLen = static_cast<int>(wcslen(szBuffer));
        szBuffer[nLen - 3] = L'l';
        szBuffer[nLen - 2] = L'n';
        szBuffer[nLen - 1] = L'k';
    }
    //Lưu lối tắt vào thư mục chỉ định
    //WCHAR  wsz[MAX_PATH];  //Khai báo chuỗi Unicode
    //MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);

    hr = ppf->Save(szBuffer, TRUE);

    ppf->Release();
    pLink->Release();
    return SUCCEEDED(hr);
}

wstring CCommon::GetStartUpPath()
{
    LPITEMIDLIST ppidl;
    TCHAR pszStartUpPath[MAX_PATH]{};
    if (SHGetSpecialFolderLocation(NULL, CSIDL_STARTUP, &ppidl) == S_OK)
    {
        SHGetPathFromIDList(ppidl, pszStartUpPath);
        CoTaskMemFree(ppidl);
    }
    return wstring(pszStartUpPath);
}

void CCommon::GetFiles(const wchar_t* path, vector<wstring>& files)
{
    //Handle tệp
    intptr_t hFile = 0;
    //Thông tin tệp (lưu dạng Unicode dùng _wfinddata_t, bộ ký tự đa byte dùng _finddata_t)
    _wfinddata_t fileinfo;
    wstring file_name;
    if ((hFile = _wfindfirst(path, &fileinfo)) != -1)
    {
        do
        {
            file_name.assign(fileinfo.name);
            if (file_name != L"." && file_name != L"..")
                //files.push_back(wstring(path) + L"\\" + file_name);  //Lưu tên tệp (bỏ qua "." và "..")
                files.push_back(L"\\" + file_name);  //Lưu tên tệp (bỏ qua "." và "..")
        } while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}

void CCommon::GetFiles(const wchar_t* path, std::function<void(const wstring&)> func)
{
    //Handle tệp
    intptr_t hFile = 0;
    _wfinddata_t fileinfo;
    wstring file_name;
    if ((hFile = _wfindfirst(path, &fileinfo)) != -1)
    {
        do
        {
            file_name.assign(fileinfo.name);
            if (file_name != L"." && file_name != L"..")
                func(file_name);
        } while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}

bool CCommon::FileExist(LPCTSTR file_name)
{
    return (PathFileExists(file_name) != 0);
}

bool CCommon::IsFolder(const wstring& path)
{
    DWORD dwAttrib = GetFileAttributes(path.c_str());
    return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool CCommon::MoveAFile(LPCTSTR exist_file, LPCTSTR new_file)
{
    if (!FileExist(exist_file))
        return false;
    //if (FileExist(new_file))      //Nếu tệp đích đã tồn tại thì xóa trước
    //  DeleteFile(new_file);
    return (MoveFile(exist_file, new_file) != 0);
}

SYSTEMTIME CCommon::CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b)
{
    SYSTEMTIME result{};
    short hour = a.wHour - b.wHour;
    short minute = a.wMinute - b.wMinute;
    short second = a.wSecond - b.wSecond;

    if (second < 0)
    {
        second += 60;
        minute--;
    }

    if (minute < 0)
    {
        minute += 60;
        hour--;
    }

    if (hour < 0)
    {
        hour += 24;
    }
    result.wHour = hour;
    result.wMinute = minute;
    result.wSecond = second;
    return result;
}

ULONGLONG CCommon::GetCurrentTimeSinceEpochMilliseconds()
{
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);  // Lấy thời gian hệ thống hiện tại

    // Chuyển FILETIME sang ULARGE_INTEGER để tính toán
    ULARGE_INTEGER uli;
    uli.LowPart = fileTime.dwLowDateTime;
    uli.HighPart = fileTime.dwHighDateTime;

    // Số khoảng 100 nano giây từ ngày 1/1/1601 đến ngày 1/1/1970
    const ULONGLONG EPOCH_OFFSET = 116444736000000000ULL;

    // Chuyển thành số mili giây tính từ ngày 1/1/1970
    ULONGLONG millisecondsSince1970 = (uli.QuadPart - EPOCH_OFFSET) / 10000;
    return millisecondsSince1970;
}

wstring CCommon::GetModuleDir()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    size_t index;
    wstring current_path{ path };
    index = current_path.find_last_of(L'\\');
    current_path = current_path.substr(0, index + 1);
    return current_path;
}

wstring CCommon::GetSystemDir()
{
    wchar_t buff[MAX_PATH];
    GetSystemDirectory(buff, MAX_PATH);
    return wstring(buff);
}

wstring CCommon::GetTemplateDir()
{
    wstring result;
    wchar_t buff[MAX_PATH];
    GetTempPath(MAX_PATH, buff);        //Lấy đường dẫn thư mục tạm
    result = buff;
    if (result.back() != L'\\' && result.back() != L'/')        //Đảm bảo đường dẫn có dấu gạch chéo ở cuối
        result.push_back(L'\\');
    return result;
}

wstring CCommon::GetAppDataConfigDir()
{
    LPITEMIDLIST ppidl;
    TCHAR pszAppDataPath[MAX_PATH];
    if (SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &ppidl) == S_OK)
    {
        SHGetPathFromIDList(ppidl, pszAppDataPath);
        CoTaskMemFree(ppidl);
    }
    wstring app_data_path{ pszAppDataPath };        //Lấy đường dẫn C:/User/<tên người dùng>/AppData/Roaming
    CreateDirectory(app_data_path.c_str(), NULL);       //Nếu Roaming chưa tồn tại thì tạo mới
    app_data_path += L"\\TrafficMonitor\\";
    CreateDirectory(app_data_path.c_str(), NULL);       //Nếu C:/User/<tên người dùng>/AppData/Roaming/TrafficMonitor chưa tồn tại thì tạo mới

    return app_data_path;
}

void CCommon::DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color)
{
    pDC->SetTextColor(color);
    //m_pDC->SetBkMode(TRANSPARENT);
    //Tô vùng hình chữ nhật bằng màu nền
    pDC->FillSolidRect(rect, back_color);
    pDC->DrawText(lpszString, rect, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

}

//void CCommon::SetDrawArea(CDC * pDC, CRect rect)
//{
//  CRgn rgn;
//  rgn.CreateRectRgnIndirect(rect);
//  pDC->SelectClipRgn(&rgn);
//}


bool CCommon::IsForegroundFullscreen(HMONITOR hMonitor)
{
    if (hMonitor == NULL)
        hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
    bool bFullscreen{ false };      //Dùng để cho biết cửa sổ tiền cảnh có đang toàn màn hình hay không
    HWND hWnd{};
    RECT rcApp{};

    // Lấy thông tin màn hình
    MONITORINFOEX monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(hMonitor, &monitorInfo);
    RECT monitorRect = monitorInfo.rcMonitor;

    hWnd = GetForegroundWindow();   //Lấy handle cửa sổ tiền cảnh đang tương tác với người dùng
    TCHAR buff[256];
    GetClassName(hWnd, buff, 256);      //Lấy tên lớp của cửa sổ tiền cảnh
    CString class_name{ buff };
    if (hWnd != GetDesktopWindow() && class_name != _T("WorkerW") && hWnd != GetShellWindow())//Nếu cửa sổ tiền cảnh không phải cửa sổ desktop và cũng không phải cửa sổ console
    {
        GetWindowRect(hWnd, &rcApp);    //Lấy tọa độ cửa sổ tiền cảnh
        if (rcApp.left <= monitorRect.left && //Nếu tọa độ cửa sổ tiền cảnh che phủ hoàn toàn cửa sổ desktop thì cửa sổ tiền cảnh đang ở chế độ toàn màn hình
            rcApp.top <= monitorRect.top &&
            rcApp.right >= monitorRect.right &&
            rcApp.bottom >= monitorRect.bottom)
        {
            bFullscreen = true;
        }
    }//Nếu cửa sổ tiền cảnh là cửa sổ desktop hoặc cửa sổ console thì trả về ngay là không toàn màn hình
    return bFullscreen;
}

bool CCommon::CopyStringToClipboard(const wstring& str)
{
    if (OpenClipboard(NULL))
    {
        HGLOBAL clipbuffer;
        EmptyClipboard();
        size_t size = (str.size() + 1) * 2;
        clipbuffer = GlobalAlloc(GMEM_DDESHARE, size);
        memcpy_s(GlobalLock(clipbuffer), size, str.c_str(), size);
        GlobalUnlock(clipbuffer);
        if (SetClipboardData(CF_UNICODETEXT, clipbuffer) == NULL)
            return false;
        CloseClipboard();
        return true;
    }
    else return false;
}


wstring CCommon::GetJsonValueSimple(const wstring& json_str, const wstring& name)
{
    wstring str_name{ L"\"" };
    str_name += name;
    str_name += L'\"';
    size_t index = json_str.find(str_name);
    if (index == wstring::npos)
        return wstring();
    index = json_str.find(L':', index + 1);
    if (index == wstring::npos)
        return wstring();
    index = json_str.find_first_not_of(L"\" ", index + 1);
    if (index == wstring::npos)     //Bảo mật: dữ liệu không hợp lệ/bị cắt cụt thì trả về rỗng thay vì ném ngoại lệ out_of_range
        return wstring();
    size_t index_end = json_str.find_first_of(L"\",]}\r\n", index);
    wstring result = json_str.substr(index, index_end - index);
    return result;
}

//Phòng khi biên dịch với _WIN32_WINNT cũ (ví dụ cấu hình Windows XP) mà SDK không định nghĩa các cờ này
#ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR 0x00000100
#endif
#ifndef LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
#define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS 0x00001000
#endif

void CCommon::HardenDllSearchOrder()
{
    //Bỏ thư mục hiện hành khỏi thứ tự tìm DLL
    ::SetDllDirectoryW(L"");

    //SetDefaultDllDirectories có từ Windows 8 (Windows 7 cần bản cập nhật KB2533623), nên lấy địa chỉ hàm động
    //để chương trình vẫn chạy được trên hệ thống cũ.
    typedef BOOL(WINAPI* pfSetDefaultDllDirectories)(DWORD);
    HMODULE kernel32 = ::GetModuleHandleW(L"kernel32.dll");
    if (kernel32 != NULL)
    {
        auto set_default_dll_directories = reinterpret_cast<pfSetDefaultDllDirectories>(::GetProcAddress(kernel32, "SetDefaultDllDirectories"));
        if (set_default_dll_directories != nullptr)
        {
            //Chỉ tìm DLL trong: thư mục chương trình, System32 và các thư mục được thêm bằng AddDllDirectory
            set_default_dll_directories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        }
    }
}

HMODULE CCommon::SafeLoadLibrary(LPCWSTR full_path)
{
    if (full_path == nullptr || full_path[0] == L'\0')
        return NULL;
    HMODULE module = ::LoadLibraryExW(full_path, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (module == NULL && ::GetLastError() == ERROR_INVALID_PARAMETER)
    {
        //Hệ thống cũ không hỗ trợ các cờ LOAD_LIBRARY_SEARCH_*: dùng cách nạp truyền thống (đường dẫn vẫn là tuyệt đối)
        module = ::LoadLibraryExW(full_path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    }
    return module;
}

bool CCommon::IsHttpsUrl(const wstring& url)
{
    //Chỉ chấp nhận URL bắt đầu bằng "https://" (không phân biệt hoa thường) và không chứa ký tự điều khiển/khoảng trắng
    if (url.size() <= 8 || url.size() > 2048 || _wcsnicmp(url.c_str(), L"https://", 8) != 0)
        return false;
    for (wchar_t ch : url)
    {
        if (ch <= L' ' || ch == 0x7f)
            return false;
    }
    return true;
}

bool CCommon::IsTrustedHostUrl(const wstring& url)
{
    //URL phải là HTTPS và host (không phân biệt hoa thường) khớp đúng một mục trong FORK_TRUSTED_HOSTS.
    //Từ chối URL có thông tin đăng nhập ("user@host") hoặc cổng khác mặc định.
    if (!IsHttpsUrl(url))
        return false;
    size_t start = 8;   //sau "https://"
    size_t end = url.find_first_of(L"/?#", start);
    wstring authority = url.substr(start, end == wstring::npos ? wstring::npos : end - start);
    if (authority.empty() || authority.find_first_of(L"@:\\") != wstring::npos)
        return false;
    std::vector<wstring> hosts;
    StringSplit(FORK_TRUSTED_HOSTS, L'|', hosts);
    for (const auto& host : hosts)
    {
        if (!host.empty() && _wcsicmp(host.c_str(), authority.c_str()) == 0)
            return true;
    }
    return false;
}

bool CCommon::GetURL(const wstring& url, std::string& result, const wstring& user_agent)
{
    //Bảo mật: chỉ cho phép tải qua HTTPS. CInternetSession::OpenURL còn hỗ trợ file:// và ftp://,
    //nếu URL bị thay đổi có thể bị lợi dụng để đọc tệp cục bộ hoặc truyền dữ liệu không mã hóa.
    if (!IsHttpsUrl(url))
    {
        if (theApp.m_debug_log)
            CCommon::WriteLog((L"GetURL: blocked non-HTTPS URL: " + url).c_str(), theApp.m_log_path.c_str());
        return false;
    }

    //Giới hạn kích thước phản hồi (1 MB), tránh cạn kiệt bộ nhớ khi máy chủ/kẻ tấn công trả về dữ liệu quá lớn
    const size_t MAX_RESPONSE_BYTES = 1024 * 1024;
    //Thời gian chờ kết nối/nhận dữ liệu (mili giây), tránh luồng bị treo vô thời hạn
    const DWORD NETWORK_TIMEOUT_MS = 15000;

    bool succeed{ false };
    CInternetSession* pSession{};
    CHttpFile* pfile{};
    try
    {
        pSession = new CInternetSession(user_agent.empty() ? APP_NAME : user_agent.c_str());
        pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, NETWORK_TIMEOUT_MS);
        pSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, NETWORK_TIMEOUT_MS);
        pSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, NETWORK_TIMEOUT_MS);
        //INTERNET_FLAG_RELOAD/NO_CACHE_WRITE: luôn lấy dữ liệu mới từ máy chủ, không dùng/ghi bộ nhớ đệm (tránh thông tin phiên bản cũ hoặc bị đầu độc cache)
        //INTERNET_FLAG_NO_COOKIES: không gửi/nhận cookie. Không dùng bất kỳ cờ IGNORE_CERT_* nào: chứng chỉ TLS luôn được kiểm tra.
        const DWORD flags = INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_SECURE;
        CStdioFile* pRawFile = pSession->OpenURL(url.c_str(), 1, flags);
        pfile = DYNAMIC_DOWNCAST(CHttpFile, pRawFile);      //Dùng cơ chế runtime-class của MFC
        if (pfile == nullptr)       //Không phải kết nối HTTP(S) thì từ chối
        {
            if (theApp.m_debug_log)
                CCommon::WriteLog((L"GetURL: response is not an HTTP file: " + url).c_str(), theApp.m_log_path.c_str());
            if (pRawFile != nullptr)
            {
                pRawFile->Close();
                delete pRawFile;
            }
            pSession->Close();
            SAFE_DELETE(pSession);
            return false;
        }
        //Bảo mật: WinINet tự theo chuyển hướng (redirect). Kiểm tra URL cuối cùng vẫn là HTTPS và thuộc máy chủ
        //được phép (FORK_TRUSTED_HOSTS), tránh dữ liệu bị lấy từ máy chủ lạ khi một chuyển hướng bị lợi dụng.
        wchar_t final_url[2048]{};
        DWORD final_url_len = sizeof(final_url);
        if (!pfile->QueryOption(INTERNET_OPTION_URL, final_url, &final_url_len) || !IsTrustedHostUrl(final_url))
        {
            if (theApp.m_debug_log)
                CCommon::WriteLog((L"GetURL: redirected to untrusted URL: " + wstring(final_url) + L" (from " + url + L")").c_str(), theApp.m_log_path.c_str());
            pfile->Close();
            SAFE_DELETE(pfile);
            pSession->Close();
            SAFE_DELETE(pSession);
            return false;
        }
        DWORD dwStatusCode{};
        pfile->QueryInfoStatusCode(dwStatusCode);
        if (dwStatusCode == HTTP_STATUS_OK)
        {
            //Đọc nội dung dạng byte thô. Không dùng ReadString vào CString: trong bản Unicode, ReadString gói các byte
            //vào từng ký tự 2 byte, khi phản hồi có số byte lẻ sẽ kéo theo 1 byte rác ở cuối chuỗi.
            std::string content;
            char buffer[4096];
            UINT bytes_read{};
            bool too_large{ false };
            while ((bytes_read = pfile->Read(buffer, sizeof(buffer))) > 0)
            {
                content.append(buffer, bytes_read);
                if (content.size() > MAX_RESPONSE_BYTES)
                {
                    too_large = true;
                    break;
                }
            }
            if (!too_large)
            {
                //Cắt tại byte 0 đầu tiên (nếu có) để giữ hành vi như chuỗi C trước đây
                size_t nul_pos = content.find('\0');
                if (nul_pos != std::string::npos)
                    content.resize(nul_pos);
                result = std::move(content);
                succeed = true;
            }
            else if (theApp.m_debug_log)
            {
                CCommon::WriteLog((L"GetURL: response too large: " + url).c_str(), theApp.m_log_path.c_str());
            }
        }
        else if (theApp.m_debug_log)
        {
            CCommon::WriteLog((L"GetURL: HTTP status " + std::to_wstring(dwStatusCode) + L": " + url).c_str(), theApp.m_log_path.c_str());
        }
        pfile->Close();
        delete pfile;
        pfile = nullptr;
        pSession->Close();
    }
    catch (CInternetException* e)
    {
        //Ghi nhật ký lỗi
        if (theApp.m_debug_log)
        {
            CString info = CCommon::LoadTextFormat(IDS_GET_URL_ERROR_LOG_INFO, { url, static_cast<size_t>(e->m_dwError) });
            CCommon::WriteLog(info, theApp.m_log_path.c_str());
        }
        if (pfile != nullptr)
        {
            pfile->Close();
            delete pfile;
        }
        if (pSession != nullptr)
            pSession->Close();
        succeed = false;
        e->Delete();        //Thiếu câu lệnh này sẽ gây rò rỉ bộ nhớ
        SAFE_DELETE(pSession);
    }
    SAFE_DELETE(pSession);
    return succeed;
}

bool CCommon::GetURL(const wstring& url, wstring& result, bool utf8, const wstring& user_agent)
{
    std::string str_result;
    bool succeed = GetURL(url, str_result, user_agent);
    if (succeed)
    {
        result = CCommon::StrToUnicode(str_result.c_str(), utf8);
    }
    return succeed;
}


void CCommon::GetInternetIp(wstring& ip_address, wstring& ip_location, bool global)
{
    //Bản Việt hóa: không còn trích xuất dữ liệu từ trang web ip.cn (phụ thuộc giao diện HTML của bên thứ ba).
    //Dùng chung dịch vụ với GetInternetIp2 (IPv4).
    UNREFERENCED_PARAMETER(global);
    GetInternetIp2(ip_address, ip_location, false);
}

bool CCommon::IsValidIpAddressString(const wstring& ip)
{
    //Chỉ chấp nhận chuỗi giống địa chỉ IPv4/IPv6: chữ số thập lục phân, '.', ':' và độ dài hợp lý
    if (ip.size() < 2 || ip.size() > 45)
        return false;
    for (wchar_t ch : ip)
    {
        if (!iswxdigit(ch) && ch != L'.' && ch != L':')
            return false;
    }
    return true;
}

wstring CCommon::ParseIpServiceResponse(const wstring& response)
{
    wstring ip;
    if (response.find(L"\"ip\"") != wstring::npos)
    {
        ip = GetJsonValueSimple(response, L"ip");     //Dạng JSON: {"ip":"1.2.3.4"}
    }
    else
    {
        //Dạng văn bản thuần: bỏ khoảng trắng/xuống dòng ở hai đầu
        const wchar_t* whitespace = L" \t\r\n";
        size_t first = response.find_first_not_of(whitespace);
        if (first != wstring::npos)
        {
            size_t last = response.find_last_not_of(whitespace);
            ip = response.substr(first, last - first + 1);
        }
    }
    if (!IsValidIpAddressString(ip))
        ip.clear();
    return ip;
}

CCommon::PublicIpResult CCommon::GetInternetIp2(wstring& ip_address, wstring& ip_location, bool ipv6)
{
    //Bảo mật & quyền riêng tư: dùng dịch vụ mã nguồn mở ipify (HTTPS, chỉ trả về địa chỉ IP, không trả về vị trí)
    //thay cho dịch vụ bên thứ ba trước đây. Chỉ được gọi khi người dùng chủ động yêu cầu trong hộp thoại "Chi tiết kết nối".
    ip_address.clear();
    ip_location.clear();

    //Máy không có địa chỉ IPv6 toàn cầu (nhà mạng/router không cấp IPv6) thì không thể có IP công cộng IPv6:
    //báo "không có kết nối IPv6" thay vì gửi truy vấn chắc chắn thất bại rồi báo lỗi.
    if (ipv6 && !CAdapterCommon::HasGlobalIpv6Address())
        return PublicIpResult::NO_CONNECTION;

    wstring raw_string;
    wstring user_agent{ L"TrafficMonitor-VN/" };
    user_agent += VERSION;
    //Lưu ý: phải có "/" trước "?". Với "https://api.ipify.org?format=json", WinINet làm mất phần query
    //nên ipify trả về văn bản thuần thay vì JSON.
    if (!GetURL((ipv6 ? PUBLIC_IP_V6_URL : PUBLIC_IP_V4_URL), raw_string, true, user_agent))
        return PublicIpResult::FAILED;

    //Phân tích phản hồi và kiểm tra định dạng trước khi hiển thị
    ip_address = ParseIpServiceResponse(raw_string);
    if (ip_address.empty())
    {
        if (theApp.m_debug_log)
            CCommon::WriteLog((L"GetInternetIp2: invalid response: " + raw_string.substr(0, 200)).c_str(), theApp.m_log_path.c_str());
        return PublicIpResult::FAILED;
    }
    return PublicIpResult::OK;
}


void CCommon::SetRect(CRect& rect, int x, int y, int width, int height)
{
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
}

CString CCommon::LoadText(const wchar_t* id, LPCTSTR back_str)
{
    CString str = theApp.m_str_table.LoadText(id).c_str();
    if (back_str != nullptr)
        str += back_str;
    return str;
}

CString CCommon::LoadText(LPCTSTR front_str, const wchar_t* id, LPCTSTR back_str)
{
    CString str = theApp.m_str_table.LoadText(id).c_str();
    if (back_str != nullptr)
        str += back_str;
    if (front_str != nullptr)
        str = front_str + str;
    return str;
}

CString CCommon::StringFormat(LPCTSTR format_str, const std::initializer_list<CVariant>& paras)
{
    CString str_rtn = format_str;
    int index = 1;
    for (const auto& para : paras)
    {
        CString para_str = para.ToString();
        CString format_para;
        format_para.Format(_T("<%%%d%%>"), index);
        str_rtn.Replace(format_para, para_str);

        index++;
    }
    return str_rtn;
}

CString CCommon::LoadTextFormat(const wchar_t* id, const std::initializer_list<CVariant>& paras)
{
    CString str = theApp.m_str_table.LoadText(id).c_str();
    return StringFormat(str.GetString(), paras);
}

CString CCommon::IntToString(__int64 n, bool thousand_separation, bool is_unsigned)
{
    wstring str = std::to_wstring(is_unsigned ? static_cast<unsigned __int64>(n) : n);
    int count{};
    if (thousand_separation)
    {
        int length{ static_cast<int>(str.size()) };
        for (int i{ length - 1 }; i > 0; i--)
        {
            count++;
            if (count % 3 == 0)
                str.insert(i, L",");
        }
    }
    return str.c_str();
}

void CCommon::NormalizeFont(LOGFONT& font)
{
    wstring name;
    wstring style;
    name = font.lfFaceName;
    if (name.empty())
        return;
    if (name.back() == L' ')
        name.pop_back();
    size_t index = name.rfind(L' ');
    if (index == wstring::npos)
        return;
    style = name.substr(index + 1);
    bool style_acquired = false;
    if (style == L"Light")
    {
        font.lfWeight = FW_LIGHT;
        style_acquired = true;
    }
    else if (style == L"Semilight")
    {
        font.lfWeight = 350;
        style_acquired = true;
    }
    else if (style == L"Semibold")
    {
        font.lfWeight = FW_SEMIBOLD;
        style_acquired = true;
    }
    else if (style == L"Bold")
    {
        font.lfWeight = FW_BOLD;
        style_acquired = true;
    }
    else if (style == L"Black")
    {
        font.lfWeight = FW_BLACK;
        style_acquired = true;
    }

    if (style_acquired)
    {
        name = name.substr(0, index);
    }
    //wcsncpy_s(font.lfFaceName, name.c_str(), 32);
    WStringCopy(font.lfFaceName, 32, name.c_str(), static_cast<int>(name.size()));
}

void CCommon::WStringCopy(wchar_t* str_dest, int dest_size, const wchar_t* str_source, int source_size)
{
    if (dest_size <= 0)
        return;
    if (source_size <= 0 || str_source == nullptr)
    {
        str_dest[0] = L'\0';
        return;
    }
    int i;
    for (i = 0; i < dest_size && i < source_size && str_source[i] != L'\0'; i++)
        str_dest[i] = str_source[i];
    //Đảm bảo chuỗi đích có \0 ở cuối
    int copy_cnt = i;
    if (copy_cnt < dest_size)
        str_dest[copy_cnt] = L'\0';
    else
        str_dest[dest_size - 1] = L'\0';
}

bool CCommon::StringReplace(wstring& str, const wstring& str_old, const wstring& str_new)
{
    if (str.empty())
        return false;
    bool replaced{ false };
    size_t pos = 0;
    while ((pos = str.find(str_old, pos)) != std::wstring::npos)
    {
        str.replace(pos, str_old.length(), str_new);
        replaced = true;
        pos += str_new.length();    // Tiến tới cuối chuỗi đã thay thế
    }
    return replaced;
}

template<class T>
static double _StringSimilarDegree_LD(const T& srcString, const T& matchString)
{
    int n = static_cast<int>(srcString.size());
    int m = static_cast<int>(matchString.size());
    //int[, ] d = new int[n + 1, m + 1]; // matrix
    vector<vector<int>> d(n + 1, vector<int>(m + 1));
    int cost; // cost
    // Step 1 (nếu một trong hai chuỗi có độ dài 0 thì độ tương đồng là 1)?
    //if (n == 0) return (double)m / max(srcString.size(), matchString.size());
    //if (m == 0) return (double)n / max(srcString.size(), matchString.size());
    if (n == 0 || m == 0) return 0.0;   //Nếu một trong hai chuỗi có độ dài 0 thì độ tương đồng là 0
    // Step 2
    for (int i = 0; i <= n; i++)
    {
        d[i][0] = i;
    }
    for (int j = 0; j <= m; j++)
    {
        d[0][j] = j;
    }
    // Step 3
    for (int i = 1; i <= n; i++)
    {
        //Step 4
        for (int j = 1; j <= m; j++)
        {
            // Step 5
            cost = (matchString.substr(j - 1, 1) == srcString.substr(i - 1, 1) ? 0 : 1);
            // Step 6
            d[i][j] = min(min(d[i - 1][j] + 1, d[i][j - 1] + 1), d[i - 1][j - 1] + cost);
        }
    }

    // Step 7
    double ds = 1 - (double)d[n][m] / max(srcString.size(), matchString.size());

    return ds;
}


double CCommon::StringSimilarDegree_LD(const std::string& srcString, const std::string& matchString)
{
    return _StringSimilarDegree_LD(srcString, matchString);
}

double CCommon::StringSimilarDegree_LD(const std::wstring& srcString, const std::wstring& matchString)
{
    return _StringSimilarDegree_LD(srcString, matchString);
}

void CCommon::SetThreadLanguage(WORD language)
{
    if (language != 0)
        SetThreadUILanguage(language);
}

void CCommon::SetColorMode(ColorMode mode)
{
    switch (mode)
    {
    case ColorMode::Default:
        //Trên Win8/8.1 màu nền và màu trong suốt không dùng màu đen thuần
        if (theApp.m_win_version.IsWindows8Or8point1())
        {
            CTrafficMonitorApp::self->m_taskbar_data.dft_back_color = RGB(0, 0, 1);
            CTrafficMonitorApp::self->m_taskbar_data.dft_transparent_color = RGB(0, 0, 1);
        }
        else
        {
            CTrafficMonitorApp::self->m_taskbar_data.dft_back_color = 0;
            CTrafficMonitorApp::self->m_taskbar_data.dft_transparent_color = 0;
        }
        CTrafficMonitorApp::self->m_taskbar_data.dft_status_bar_color = 0x005A5A5A;
        CTrafficMonitorApp::self->m_taskbar_data.dft_text_colors = 0x00ffffffU;
        CTrafficMonitorApp::self->m_cfg_data.m_dft_notify_icon = 0;
        break;
    case ColorMode::Light:
        CTrafficMonitorApp::self->m_taskbar_data.dft_back_color = 0x00D3D2D2;
        CTrafficMonitorApp::self->m_taskbar_data.dft_transparent_color = 0x00D3D2D2;
        CTrafficMonitorApp::self->m_taskbar_data.dft_status_bar_color = 0x00A5A5A5;
        CTrafficMonitorApp::self->m_taskbar_data.dft_text_colors = 0x00000000U;
        CTrafficMonitorApp::self->m_cfg_data.m_dft_notify_icon = 4;
        break;
    default:
        break;
    }
}

void CCommon::TransparentColorConvert(COLORREF& transparent_color)
{
    if (transparent_color == 0)
        return;
    BYTE r = GetRValue(transparent_color);
    BYTE g = GetGValue(transparent_color);
    BYTE b = GetBValue(transparent_color);
    if (r == b)
    {
        if (b >= 255)
            b--;
        else
            b++;
        transparent_color = RGB(r, g, b);
    }
}

void CCommon::SetDialogFont(CWnd* pDlg, CFont* pFont)
{
    if (pDlg->GetSafeHwnd() != NULL)
    {
        CWnd* pWndChild;
        pWndChild = pDlg->GetWindow(GW_CHILD);
        while (pWndChild)
        {
            pWndChild->SetFont(pFont);
            pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
        }
    }
}

CString CCommon::GetTextResource(UINT id, int code_type)
{
    CString res_str;
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(id), _T("TEXT"));
    if (hRes != NULL)
    {
        DWORD resSize = SizeofResource(NULL, hRes);  // Lấy kích thước tài nguyên
        HGLOBAL hglobal = LoadResource(NULL, hRes);
        if (hglobal != NULL)
        {
            LPVOID pResourceData = LockResource(hglobal);  // Lấy con trỏ tới dữ liệu tài nguyên
            if (code_type == 2)
            {
                // Tài nguyên là chuỗi ký tự rộng (wide)
                res_str = CString((const wchar_t*)pResourceData, resSize / sizeof(wchar_t));
            }
            else
            {
                // Tài nguyên là chuỗi ký tự hẹp (narrow)
                std::string strData((const char*)pResourceData, resSize);
                res_str = CCommon::StrToUnicode(strData.c_str(), (code_type != 0)).c_str();
            }
        }
    }
    return res_str;
}

CString CCommon::GetLastCompileTime()
{
    CString str_compile_time = GetTextResource(IDR_COMPILE_TIME, 0);
    str_compile_time.Replace(_T("\r\n"), _T(""));
    str_compile_time.Delete(str_compile_time.GetLength() - 1, 1);
    return str_compile_time;
}

HICON CCommon::LoadIconResource(UINT id, int size)
{
    return (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(id), IMAGE_ICON, size, size, 0);
}

int CCommon::GetMenuItemPosition(CMenu* pMenu, UINT id)
{
    int pos = -1;
    int item_count = pMenu->GetMenuItemCount();
    for (int i = 0; i < item_count; i++)
    {
        if (pMenu->GetMenuItemID(i) == id)
        {
            pos = i;
            break;
        }
    }
    return pos;
}

// Duyệt đệ quy các mục menu và xử lý dịch đa ngôn ngữ
static void TranslateMenuItems(CMenu& menu)
{
    // Duyệt các mục menu
    for (int i = 0; i < menu.GetMenuItemCount(); ++i)
    {
        UINT menuItemID = menu.GetMenuItemID(i);
        CString menuText;
        menu.GetMenuString(i, menuText, MF_BYPOSITION);

        // Kiểm tra văn bản mục menu có bắt đầu bằng TXT_ hay không
        if (menuText.Left(4) == _T("TXT_"))
        {
            // Lấy văn bản đã dịch
            std::wstring key(menuText);
            const std::wstring& translatedText = theApp.m_str_table.LoadMenuText(key);

            // Cập nhật văn bản mục menu
            menu.ModifyMenu(i, MF_BYPOSITION | MF_STRING, menuItemID, translatedText.c_str());
        }

        if (menuItemID == -1)
        {
            // Đây là menu bật lên (menu con), xử lý đệ quy
            CMenu* pSubMenu = menu.GetSubMenu(i);
            if (pSubMenu)
            {
                TranslateMenuItems(*pSubMenu); // Gọi đệ quy
            }
        }
    }
}

void CCommon::LoadMenuResource(CMenu& menu, UINT res_id)
{
    // Tải tài nguyên menu
    menu.LoadMenu(res_id);

    // Xử lý dịch mục menu
    TranslateMenuItems(menu);
}

bool CCommon::IsColorSimilar(COLORREF color1, COLORREF color2)
{
    const int DIFF{ 24 };
    return (std::abs(GetRValue(color1) - GetRValue(color2)) < DIFF
        && std::abs(GetGValue(color1) - GetGValue(color2)) < DIFF
        && std::abs(GetBValue(color1) - GetBValue(color2)) < DIFF);
}

int CCommon::CountOneBits(unsigned int value)
{
    int count = 0;
    while (value != 0)
    {
        if (value % 2 == 1)
        {
            count++;
        }
        value = value >> 1;
    }
    return count;
}

void CCommon::SetNumberBit(unsigned int& num, int bit, bool value)
{
    if (value)
    {
        num |= (1 << bit);
    }
    else
    {
        num &= ~(1 << bit);
    }
}

bool CCommon::GetNumberBit(unsigned int num, int bit)
{
    return (num & (1 << bit)) != 0;
}

COLORREF CCommon::GetWindowsThemeColor()
{
    DWORD crColorization;
    BOOL fOpaqueBlend;
    COLORREF theme_color{};
    HRESULT result = DwmGetColorizationColor(&crColorization, &fOpaqueBlend);
    if (result == S_OK)
    {
        BYTE r, g, b;
        r = (crColorization >> 16) % 256;
        g = (crColorization >> 8) % 256;
        b = crColorization % 256;
        theme_color = RGB(r, g, b);
    }
    return theme_color;
}

CString CCommon::GetErrorMessage(DWORD error_code)
{
    CString error_msg;
    if (error_code != 0)
    {
        LPVOID lpMsgBuf = nullptr;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error_code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL);

        if (lpMsgBuf != nullptr)
            error_msg = (LPCTSTR)lpMsgBuf;

        LocalFree(lpMsgBuf);
    }
    return error_msg;
}
