#include "stdafx.h"
#include "FileDialogEx.h"
#include <sstream>

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCWSTR lpszDefExt, LPCWSTR lpszFilter)
    : pFileDialog(nullptr), m_bOpenFileDialog(bOpenFileDialog)
{
    // Tạo thực thể IFileDialog
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));

    if (pFileDialog)
    {
        // Thiết lập tùy chọn hộp thoại
        DWORD dwOptions;
        pFileDialog->GetOptions(&dwOptions);
        pFileDialog->SetOptions(dwOptions | FOS_FORCEFILESYSTEM);  // Chỉ các mục của hệ thống tệp

        if (bOpenFileDialog)
        {
            pFileDialog->SetOptions(dwOptions | FOS_FILEMUSTEXIST); // Khi mở tệp thì tệp phải tồn tại
        }

        // Thiết lập phần mở rộng mặc định
        if (lpszDefExt)
        {
            pFileDialog->SetDefaultExtension(lpszDefExt);
        }

        // Phân tích và thiết lập bộ lọc
        if (lpszFilter)
        {
            ParseFilter(lpszFilter);
            for (size_t i = 0; i < m_filterDescriptions.size() && i < m_filterSpecsStrings.size(); i++)
            {
                COMDLG_FILTERSPEC filterSpec = { m_filterDescriptions[i].c_str(), m_filterSpecsStrings[i].c_str()};
                m_filterSpecs.push_back(filterSpec);
            }
            pFileDialog->SetFileTypes(static_cast<UINT>(m_filterSpecs.size()), m_filterSpecs.data());
        }
    }
}

CFileDialogEx::~CFileDialogEx()
{
    if (pFileDialog)
    {
        pFileDialog->Release();
    }
}

int CFileDialogEx::DoModal(HWND hWndOwner)
{
    if (!pFileDialog) return -1;

    // Hiển thị hộp thoại
    HRESULT hr = pFileDialog->Show(hWndOwner);
    if (FAILED(hr))
        return -1;

    // Lấy đường dẫn tệp đã chọn
    IShellItem* pItem = nullptr;
    hr = pFileDialog->GetResult(&pItem);
    if (SUCCEEDED(hr))
    {
        PWSTR pszFilePath = nullptr;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (SUCCEEDED(hr))
        {
            m_filePath = pszFilePath;
            CoTaskMemFree(pszFilePath);
        }
        pItem->Release();
    }

    return (SUCCEEDED(hr)) ? 1 : 0;
}

std::wstring CFileDialogEx::GetPathName() const
{
    return m_filePath;
}

std::wstring CFileDialogEx::GetFileName() const
{
    // Lấy tên tệp
    size_t pos = m_filePath.find_last_of(L"\\");
    if (pos != std::wstring::npos) {
        return m_filePath.substr(pos + 1);
    }
    return m_filePath;
}

void CFileDialogEx::ParseFilter(LPCWSTR lpszFilter)
{
    m_filterSpecs.clear();
    m_filterDescriptions.clear();
    m_filterSpecsStrings.clear();

    // Dùng | để tách chuỗi bộ lọc
    std::wstringstream filterStream(lpszFilter);
    std::wstring description, spec;

    while (std::getline(filterStream, description, L'|'))
    {
        if (std::getline(filterStream, spec, L'|'))
        {
            // Lưu mô tả và mẫu vào m_filterDescriptions và m_filterSpecsStrings
            m_filterDescriptions.push_back(description);
            m_filterSpecsStrings.push_back(spec);
        }
    }
}
