#pragma once
#include "ViewModels/MyEntityViewModel.h"

namespace winrt::WinUI3MVVMSample1::ViewModels
{
    struct Locator
    {
        // 提供给 View 的投影类型（非 implementation）
        static WinUI3MVVMSample1::MyEntityViewModel MyEntity();

        // 可选：测试/切换场景下重置（这里留空实现）
        static void Reset();
    };
}
