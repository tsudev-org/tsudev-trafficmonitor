#pragma once
class CWindowsSettingHelper
{
public:
    static bool IsWindows10LightTheme();				//Kiểm tra Windows có đang dùng chủ đề sáng không
    static void CheckWindows10LightTheme();
    static bool IsDotNetFramework4Point5Installed();	//Kiểm tra đã cài .NET Framework 4.5 chưa (https://learn.microsoft.com/dotnet/framework/migration-guide/how-to-determine-which-versions-are-installed)
    static bool IsTaskbarShowingInAllDisplays();        //Có hiển thị thanh tác vụ trên mọi màn hình hay không

    //Thiết lập liên quan đến Windows 11
    static bool IsTaskbarWidgetsBtnShown();             //Thanh tác vụ có hiển thị nút “Tiện ích” (Widgets) hay không
    static bool IsTaskbarCenterAlign();                  //Thanh tác vụ có căn giữa hay không

private:
    static LONG GetDWORDRegKeyData(HKEY hKey, const wstring& strValueName, DWORD& dwValueData);
    static bool GetDWORDRegKeyData(HKEY keyParent, const wstring& strKeyName, const wstring& strValueName, DWORD& dwValueData);

private:
    static 	bool m_light_theme;
};

