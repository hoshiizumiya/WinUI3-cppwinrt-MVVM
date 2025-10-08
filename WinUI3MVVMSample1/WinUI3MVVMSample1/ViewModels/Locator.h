#pragma once
#include "ViewModels/MyEntityViewModel.h"

namespace winrt::WinUI3MVVMSample1::ViewModels
{
    struct Locator
    {
        // 提供给 View 的投影类型
        static WinUI3MVVMSample1::MyEntityViewModel MyEntity();

        // 重置 ViewModel
        static void ResetViewModel(winrt::Windows::Foundation::IInspectable const& viewModel);

    private:
        static winrt::WinUI3MVVMSample1::MyEntityViewModel s_myEntityViewModel;
    };
}
