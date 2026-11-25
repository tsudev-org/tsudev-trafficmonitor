//Lớp đọc/ghi ini
//Khi dùng, truyền đường dẫn tệp ini qua tham số của hàm khởi tạo
//Khi ghi dữ liệu vào tệp ini, cần gọi hàm Save() ở cuối để lưu thay đổi vào tệp
//Mặc định lưu với mã hóa UTF8_BOM; nếu muốn lưu dạng ANSI hãy gọi SetSaveAsUTF8(false);
#pragma once

class CIniHelper
{
public:
	// Tải tệp ini từ ổ đĩa
	// file_path: đường dẫn tệp
	// force_utf8: nếu là true thì bắt buộc phân tích theo mã hóa UTF8, ngược lại chỉ phân tích theo UTF8 khi có BOM UTF8
	CIniHelper(const wstring& file_path, bool force_utf8 = false);
    // Tải ini từ tệp tài nguyên (chỉ đọc)
    CIniHelper(UINT id, bool is_utf8 = true);
    CIniHelper();
	~CIniHelper();

    void FromDirectString(const wstring& str_content);

	void SetSaveAsUTF8(bool utf8);

	void WriteString(const wchar_t* AppName, const wchar_t* KeyName, const wstring& str);
	wstring GetString(const wchar_t* AppName, const wchar_t* KeyName, const wchar_t* default_str) const;
	bool GetString(const wchar_t* AppName, const wchar_t* KeyName, wstring& str) const;
	void WriteInt(const wchar_t * AppName, const wchar_t * KeyName, int value);
	int GetInt(const wchar_t * AppName, const wchar_t * KeyName, int default_value) const;
	void WriteBool(const wchar_t * AppName, const wchar_t * KeyName, bool value);
	bool GetBool(const wchar_t * AppName, const wchar_t * KeyName, bool default_value) const;
	void WriteIntArray(const wchar_t * AppName, const wchar_t * KeyName, const int* values, int size);		//Ghi một mảng int có size phần tử
	void GetIntArray(const wchar_t * AppName, const wchar_t * KeyName, int* values, int size, int default_value = 0) const;		//Đọc một mảng int, lưu vào values, số phần tử là size
	void WriteBoolArray(const wchar_t * AppName, const wchar_t * KeyName, const bool* values, int size);
	void GetBoolArray(const wchar_t * AppName, const wchar_t * KeyName, bool* values, int size, bool default_value = false) const;
    void WriteStringList(const wchar_t* AppName, const wchar_t* KeyName, const vector<wstring>& values);      //Ghi một danh sách chuỗi; vì khi lưu vào tệp ini chuỗi sẽ được thêm dấu ngoặc kép ở đầu và cuối nên chuỗi không được chứa dấu ngoặc kép
    void GetStringList(const wchar_t* AppName, const wchar_t* KeyName, vector<wstring>& values, const vector<wstring>& default_value) const;

    // Lấy tất cả AppName có tiền tố chỉ định (không gồm tiền tố)
    vector<wstring> GetAllAppName(const wstring& prefix) const;
    // Lấy tất cả cặp khóa-giá trị trong một AppName
    void GetAllKeyValues(const wstring& AppName, std::map<wstring, wstring>& map) const;

	//Xóa một section
	bool RemoveSection(const wstring& AppName);

	bool Save();		//Lưu ini vào tệp, thành công thì trả về true

protected:
	wstring m_file_path;
	wstring m_ini_str;
	bool m_save_as_utf8{ true };		//Có lưu với mã hóa UTF8 hay không

    static void UnEscapeString(wstring& str);
    void _WriteString(const wchar_t* AppName, const wchar_t* KeyName, const wstring& str);
	bool _GetString(const wchar_t* AppName, const wchar_t* KeyName, wstring& str) const;

    static wstring MergeStringList(const vector<wstring>& values);
    static void SplitStringList(vector<wstring>& values, const wstring& str_value);
};
