#pragma once
#include <dxgi1_3.h>
#include "HResultException.h"
#include "RenderAPISupport.h"

#pragma comment(lib, "DXGI.lib")

class CDxgiException final : public CHResultException
{
    using CHResultException::CHResultException;
};

template <class SwapChain>
class CDxgiSwapChainResource : public CDeviceResourceBase<SwapChain>
{
public:
    using Base = CDeviceResourceBase<SwapChain>;
    using Base::Base;
    using typename Base::DeviceType;

    /**
     * @brief Hàm này được gọi khi chuẩn bị Resize swap chain, dùng để giải phóng mọi tài nguyên liên quan đến swap chain.\n
     Lưu ý: hàm này có thể được gọi nhiều lần, và số lần gọi không nhất thiết khớp với số lần gọi OnSwapChainResizeEnd,
     Vì vậy phải đảm bảo tài nguyên có thể được giải phóng lặp lại nhiều lần
     *
     */
    virtual void OnSwapChainResizeBegin() noexcept = 0;
    /**
     * @brief Hàm này được gọi sau khi swap chain hoàn tất Resize, dùng để tạo lại mọi tài nguyên liên quan đến swap chain
     *
     */
    virtual void OnSwapChainResizeEnd(DeviceType p_resized_swap_chain) noexcept = 0;
};

/**
 * @brief Tài nguyên tương ứng với lớp này là CDxgiSwapChainResource<CDxgiSwapChain1>, không phải CDeviceResource<CDxgiSwapChain1>
 *
 */
class CDxgiSwapChain1
{
public:
    using Resource = CDxgiSwapChainResource<CDxgiSwapChain1>;
    using Type = Microsoft::WRL::ComPtr<IDXGISwapChain1>;
    using Storage = storage_t<CResourceTracker<Resource>>;

private:
    Type m_p_swap_chain1{};
    CResourceTracker<Resource> m_resource_tracker{std::make_shared<Storage>()};

public:
    void Recreate(Microsoft::WRL::ComPtr<IUnknown> p_device, const DXGI_SWAP_CHAIN_DESC1& ref_desc1, IDXGIOutput* p_output = nullptr);
    auto GetStorage()
        -> std::shared_ptr<Storage>;
    void Resize(std::uint32_t width, std::uint32_t height);
};

class CDxgi1Support2
{
public:
    static bool CheckSupport();
    static IDXGIFactory2* GetFactory();
};
