#pragma once
class CWinVersionHelper
{
public:
	CWinVersionHelper();
	~CWinVersionHelper();

	bool IsWindows11OrLater() const;			//Kiểm tra phiên bản Windows hiện tại có phải Win11 hoặc mới hơn không
	bool IsWindows10FallCreatorOrLater() const;		//Kiểm tra phiên bản Windows hiện tại có phải Win10 Fall Creators Update hoặc mới hơn không
	bool IsWindows7() const;					//Kiểm tra phiên bản Windows có phải Windows 7 không
	bool IsWindows8Or8point1() const;			//Kiểm tra phiên bản Windows có phải Windows 8 hoặc Windows 8.1 không
    bool IsWindows8Point1OrLater() const;       //Kiểm tra phiên bản Windows có từ Windows 8.1 trở lên không
    bool IsWindows8OrLater() const;
    bool IsWindows10OrLater() const;
	bool IsWine() const;						//Kiểm tra có đang trong môi trường Wine không

	int GetMajorVersion() const { return m_major_version; }
	int GetMinorVersion() const { return m_minor_version; }
	int GetBuildNumber() const { return m_build_number; }
protected:
	int m_major_version{};
	int m_minor_version{};
	int m_build_number{};
	bool m_is_wine{};
};
