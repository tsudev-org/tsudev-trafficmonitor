#pragma once
#include "CommonData.h"
#include "SkinFile.h"

class CSkinManager
{
public:
    void Init();
    static CSkinManager& Instance();
    std::wstring GetSkinName(int index) const;
    int FindSkinIndex(const std::wstring& skin_name) const;
    bool GetSkinSettingDataByIndex(int index, SkinSettingData& skin_data);
    const vector<wstring>& GetSkinNames() const;
    int Size() const;
    void AddSkinSettingData(const std::wstring& skin_name, const SkinSettingData& data);

    void Save();

    //Lấy đối tượng SkinSettingData từ một tệp skin
    static void SkinSettingDataFronSkin(SkinSettingData& skin_setting_data, const CSkinFile& skin_file);

    //Bỏ dấu gạch chéo ở đầu tên skin
    static void SkinNameNormalize(std::wstring& skin_name);

private:
    CSkinManager()
    {}

private:
    vector<wstring> m_skins;    //Lưu đường dẫn tệp skin
    std::map<std::wstring, SkinSettingData> m_skin_setting_data_map;    //Lưu thiết lập phông chữ, văn bản hiển thị của từng skin

    static CSkinManager m_instance;
    bool m_init{ false };
};

