#pragma once
#include "Common.h"

//Lưu thông tin của một kết nối mạng
struct NetWorkConection
{
	int index{};			//Chỉ số của kết nối này trong MIB_IFTABLE
	string description;		//Mô tả mạng (lấy từ GetAdapterInfo)
	string description_2;	//Mô tả mạng (lấy từ GetIfTable)
	unsigned __int64 in_bytes{};	//Số byte đã nhận lúc khởi tạo (bộ đếm 64-bit)
	unsigned __int64 out_bytes{};	//Số byte đã gửi lúc khởi tạo (bộ đếm 64-bit)
	wstring ip_address{ L"-.-.-.-" };	//Địa chỉ IP
	wstring subnet_mask{ L"-.-.-.-" };	//Mặt nạ mạng con (subnet mask)
	wstring default_gateway{ L"-.-.-.-" };	//Cổng mặc định (default gateway)
};

class CAdapterCommon
{
public:
	CAdapterCommon();
	~CAdapterCommon();

	//Lấy danh sách kết nối mạng, điền mô tả mạng, địa chỉ IP, mặt nạ mạng con, cổng mặc định
	static void GetAdapterInfo(vector<NetWorkConection>& adapters);

	//Làm mới địa chỉ IP, mặt nạ mạng con, cổng mặc định trong danh sách kết nối mạng
	static void RefreshIpAddress(vector<NetWorkConection>& adapters);

	//Lấy chỉ số trong MIB_IFTABLE, số byte đã nhận/gửi lúc khởi tạo của từng kết nối trong danh sách mạng
	static void GetIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE* pIfTable);

	//Thêm trực tiếp tất cả kết nối trong MIB_IFTABLE vào container adapters
	static void GetAllIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE* pIfTable);

	//Lấy số byte đã nhận/gửi của một giao diện mạng bằng bộ đếm 64-bit (GetIfEntry2).
	//MIB_IFROW chỉ có bộ đếm 32-bit (dwInOctets/dwOutOctets) nên quay vòng về 0 sau mỗi 4 GiB.
	//Trả về false nếu không lấy được (khi đó in_bytes/out_bytes là giá trị 32-bit của row).
	static bool GetInterfaceOctets(const MIB_IFROW& row, unsigned __int64& in_bytes, unsigned __int64& out_bytes);

	//Lấy tốc độ liên kết (bit/giây) bằng giá trị 64-bit; MIB_IFROW::dwSpeed bị giới hạn ở ~4,29 Gbps
	static unsigned __int64 GetInterfaceSpeed(const MIB_IFROW& row);

	//Lấy mô tả của giao diện mạng; bDescr không bắt buộc kết thúc bằng ký tự '\0' nên phải giới hạn theo dwDescrLen
	static string GetIfRowDescription(const MIB_IFROW& row);

	//Kiểm tra máy có địa chỉ IPv6 toàn cầu (có thể ra Internet) trên một card mạng đang hoạt động hay không.
	//Không tính địa chỉ loopback, link-local (fe80::/10) và ULA (fc00::/7).
	static bool HasGlobalIpv6Address();
private:
	//Kiểm tra một mô tả kết nối mạng có trong danh sách IfTable hay không, trả về chỉ số, không tìm thấy thì trả về -1
	static int FindConnectionInIfTable(string connection, MIB_IFTABLE* pIfTable);

	//Kiểm tra một mô tả kết nối mạng có trong danh sách IfTable hay không, trả về chỉ số, không tìm thấy thì trả về -1. Chỉ cần khớp một phần
	static int FindConnectionInIfTableFuzzy(string connection, MIB_IFTABLE* pIfTable);
};

