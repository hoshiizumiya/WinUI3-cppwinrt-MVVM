#pragma once
#define NOMINMAX
#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>

// This is required otherwise VSDESIGNER will have linker errors.
#define _VSDESIGNER_DONT_LOAD_AS_DLL

#define DISABLE_XAML_GENERATED_MAIN

#include <wil/cppwinrt.h>  // 必须最先包含以启用异常扩展点
#include <wil/cppwinrt_helpers.h>

// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.UI.Xaml.Interop.h> //For using xaml_typename
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Dispatching.h>

// Cpp 常用库头文件
// 基本
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>
#include <bit>

// 字符串/文本
#include <string>
#include <string_view>
#include <charconv>     // 数字<->文本高速转换
#include <format>       // C++20 格式化

// 容器与算法
#include <vector>
#include <array>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <span>         // 视图类型

// 资源/内存
#include <memory>
#include <new>

// 时间/并发
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <stop_token>   // C++20 可取消
#include <semaphore>    // C++20

// 工具
#include <functional>
#include <optional>
#include <variant>
#include <tuple>
#include <filesystem>
#include <random>
#include <stdexcept>
#include <system_error>


#ifndef WINRT_EX_FUNCTIONS
#define WINRT_EX_FUNCTIONS
namespace winrt
{
    template <typename D, typename I>
    D& get_self_ref(I const& from) noexcept
    {
        return *get_self<D>(from);
    }
}
#endif  // WINRT_EX_FUNCTIONS

// XamlTypeInfo.g.cpp 在编译期看不到实现类将会导致编译错误：
//“不是 …implementation 的成员”
//“应为表达式而不是类型”
//我们只能把实现类头文件加到 pch.h 里，这样所有 TU 都能看到实现类。
#include "Controls/NodeViewModel.h"
#include "Controls/EdgeViewModel.h"
#include "Controls/NodeGraphPanel.h"
#include "Controls/NodeInvokedEventArgs.h"