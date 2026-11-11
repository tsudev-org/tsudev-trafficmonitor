#pragma once
#include "stdafx.h"
#include <memory>
#include "IDrawCommon.h"
#include "DrawCommon.h"
#include "TaskBarDlgDrawCommon.h"
#include "DrawCommonHelper.h"

template <class T>
struct StackObjectDeleter
{
    using pointer = std::add_pointer_t<T>;
    void operator()(pointer p_stack_object) const noexcept
    {
        if (p_stack_object)
        {
            Destroy(p_stack_object);
        }
    }
};
using UniqueIDrawCommon = std::unique_ptr<void, StackObjectDeleter<IDrawCommon>>;
using UniqueIDrawBuffer = std::unique_ptr<void, StackObjectDeleter<IDrawBuffer>>;

/**
 * @brief Sau khi thêm kiểu bộ kết xuất mới, hãy thêm kiểu mới vào tham số template của kiểu dưới đây
 *
 */
using DrawCommonUnionStorage = AlignedUnionStorage<CDrawCommon, CTaskBarDlgDrawCommon>;
/**
 * @brief Sau khi thêm kiểu bộ đệm mới (tức đối tượng gửi nội dung vẽ của cửa sổ tới hệ thống khi bị hủy), hãy thêm kiểu mới vào tham số template của kiểu dưới đây
 *
 */
using DrawBufferUnionStorage = AlignedUnionStorage<CDrawDoubleBuffer, CTaskBarDlgDrawBuffer, CTaskBarDlgDrawBufferUseDComposition>;
struct InvolvedDrawCommonStorages
{
    DrawBufferUnionStorage m_draw_buffer_union_storage{};
    DrawCommonUnionStorage m_draw_common_union_storage{};
};
/**
 * @brief Khối bộ nhớ stack chứa DrawCommon và DrawBuffer cùng với con trỏ độc quyền (unique) tương ứng của chúng,
 Lưu ý: nó sẽ hủy DrawCommon trước rồi mới hủy DrawBuffer
 *
 */
struct AllInvolvedDrawCommonObjectsStorage
{
    InvolvedDrawCommonStorages m_storage{};
    // Hủy DrawCommon trước rồi mới hủy DrawBuffer
    UniqueIDrawBuffer m_unique_draw_buffer{};
    UniqueIDrawCommon m_unique_draw_common{};

    AllInvolvedDrawCommonObjectsStorage() = default;
    ~AllInvolvedDrawCommonObjectsStorage() = default;
    // Cấm sao chép và di chuyển
    AllInvolvedDrawCommonObjectsStorage(const AllInvolvedDrawCommonObjectsStorage&) = delete;
    AllInvolvedDrawCommonObjectsStorage& operator=(const AllInvolvedDrawCommonObjectsStorage&) = delete;
};
using AllInvolvedDrawCommonObjectsInitializer = std::function<void(IDrawBuffer*, IDrawCommon*)>;
using TaggedAllInvolvedDrawCommonObjectsInitializer =
    std::pair<DrawCommonHelper::RenderType, AllInvolvedDrawCommonObjectsInitializer>;

/**
 * @brief Khởi tạo bộ kết xuất và bộ đệm từ bộ nhớ stack bằng hàm tương ứng với RenderType
 *
 * @param ref_object_storage Đối tượng bộ nhớ stack, chứa con trỏ có quyền sở hữu độc quyền vùng nhớ đó
 * @param render_type Enum kiểu bộ kết xuất
 * @param initializer_list Tập hợp tổng gồm enum kiểu bộ kết xuất và hàm khởi tạo tương ứng
 * @return std::tuple<IDrawBuffer*, IDrawCommon*> Con trỏ thô không có quyền sở hữu
 */
auto GetInterfaceFromAllInvolvedDrawCommonObjects(
    AllInvolvedDrawCommonObjectsStorage& ref_object_storage,
    DrawCommonHelper::RenderType render_type,
    std::initializer_list<TaggedAllInvolvedDrawCommonObjectsInitializer> initializer_list)
    -> std::tuple<IDrawBuffer*, IDrawCommon*>;
