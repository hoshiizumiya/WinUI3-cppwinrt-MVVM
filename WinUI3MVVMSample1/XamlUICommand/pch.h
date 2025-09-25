#pragma once
#define NOMINMAX
#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>

// This is required otherwise VSDESIGNER will have linker errors.
#define _VSDESIGNER_DONT_LOAD_AS_DLL

#define DISABLE_XAML_GENERATED_MAIN

#include <wil/cppwinrt.h>  // �������Ȱ����������쳣��չ��
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

// Cpp ���ÿ�ͷ�ļ�
// ����
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>
#include <bit>

// �ַ���/�ı�
#include <string>
#include <string_view>
#include <charconv>     // ����<->�ı�����ת��
#include <format>       // C++20 ��ʽ��

// �������㷨
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
#include <span>         // ��ͼ����

// ��Դ/�ڴ�
#include <memory>
#include <new>

// ʱ��/����
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <stop_token>   // C++20 ��ȡ��
#include <semaphore>    // C++20

// ����
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

// XamlTypeInfo.g.cpp �ڱ����ڿ�����ʵ���ཫ�ᵼ�±������
//������ ��implementation �ĳ�Ա��
//��ӦΪ���ʽ���������͡�
//����ֻ�ܰ�ʵ����ͷ�ļ��ӵ� pch.h ��������� TU ���ܿ���ʵ���ࡣ
#include "Controls/NodeViewModel.h"
#include "Controls/EdgeViewModel.h"
#include "Controls/NodeGraphPanel.h"
#include "Controls/NodeInvokedEventArgs.h"