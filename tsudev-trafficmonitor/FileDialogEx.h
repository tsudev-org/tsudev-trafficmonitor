#pragma once

class CFileDialogEx {
public:
    CFileDialogEx(BOOL bOpenFileDialog, LPCWSTR lpszDefExt = NULL, LPCWSTR lpszFilter = NULL);
    ~CFileDialogEx();

    // Hiển thị hộp thoại
    int DoModal(HWND hWndOwner = NULL);

    // Lấy đường dẫn và tên tệp đã chọn
    std::wstring GetPathName() const;
    std::wstring GetFileName() const;

private:
    void ParseFilter(LPCWSTR lpszFilter);

private:
    IFileDialog* pFileDialog;
    std::wstring m_filePath;
    BOOL m_bOpenFileDialog;
    std::vector<COMDLG_FILTERSPEC> m_filterSpecs;  // Lưu mô tả và mẫu của bộ lọc
    std::vector<std::wstring> m_filterDescriptions;  // Lưu mô tả bộ lọc
    std::vector<std::wstring> m_filterSpecsStrings;  // Lưu mẫu bộ lọc
};
