#pragma once
#include "CommonData.h"

#define TASKBAR_DEFAULT_STYLE_NUM 4
#define TASKBAR_DEFAULT_LIGHT_STYLE_INDEX 3
#define TASKBAR_DEFAULT_LIGHT_STYLE(i) (i == 2 || i == 3)       //Định nghĩa mẫu 3 và mẫu 4 là mẫu màu cho chế độ sáng

class CTaskbarDefaultStyle
{
public:
	struct TaskBarStyleData
	{
        std::map<CommonDisplayItem, TaskbarItemColor> text_colors{};
		COLORREF back_color{};
		COLORREF transparent_color{};
		COLORREF status_bar_color{};
		bool specify_each_item_color{};
	};

	CTaskbarDefaultStyle();
	~CTaskbarDefaultStyle();

	void LoadConfig();
	void SaveConfig() const;

	void ApplyDefaultStyle(int index, TaskBarSettingData& data) const;		//Áp dụng một mẫu màu
	//static void ApplyDefaultLightStyle(TaskBarSettingData& data);		//Áp dụng mẫu màu mặc định cho chế độ sáng
	void ModifyDefaultStyle(int index, TaskBarSettingData& data);     //Lưu thiết lập màu hiện tại vào một mẫu màu

    static bool IsTaskBarStyleDataValid(const TaskBarStyleData& data);     //Kiểm tra một chế độ màu có hợp lệ không

private:
	TaskBarStyleData m_default_style[TASKBAR_DEFAULT_STYLE_NUM];    //Kiểu mẫu có sẵn

};
