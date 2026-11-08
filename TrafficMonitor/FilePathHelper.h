#pragma once
class CFilePathHelper
{
public:
	CFilePathHelper(const wstring& file_path);
	CFilePathHelper(){}
	~CFilePathHelper();

	void SetFilePath(const wstring& file_path) { m_file_path = file_path; }

	wstring GetFileExtension(bool upper = false, bool width_dot = false) const;		//Lấy phần mở rộng của tệp (upper: có viết hoa không; width_dot: có gồm “.” không)
	wstring GetFileName() const;							//Lấy tên tệp
	wstring GetFileNameWithoutExtension() const;			//Lấy tên tệp (không gồm phần mở rộng)
	wstring GetFolderName() const;							//Lấy tên thư mục
	wstring GetDir() const;									//Lấy thư mục
	wstring GetParentDir() const;							//Lấy thư mục cha
	wstring GetFilePath() const { return m_file_path; }		//Lấy đường dẫn đầy đủ
	const wstring& ReplaceFileExtension(const wchar_t* new_extension);		//Thay phần mở rộng của tệp, trả về đường dẫn đầy đủ
    wstring GetFilePathWithoutExtension() const;            //Lấy đường dẫn tệp (không gồm phần mở rộng)
protected:
	wstring m_file_path;
};

