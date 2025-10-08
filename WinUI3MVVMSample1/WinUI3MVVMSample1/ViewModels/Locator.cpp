#include "pch.h"
#include "ViewModels/Locator.h"
#include "Models/MyEntity.h"

using namespace winrt;

namespace winrt::WinUI3MVVMSample1::ViewModels
{
    winrt::WinUI3MVVMSample1::MyEntityViewModel Locator::s_myEntityViewModel = nullptr;

    winrt::WinUI3MVVMSample1::MyEntityViewModel Locator::MyEntity()
    {
        // 持久化一个 VM；如需每次新建可自行调整策略
        if(!s_myEntityViewModel)
        {
            s_myEntityViewModel = WinUI3MVVMSample1::MyEntityViewModel{};
        }
        return s_myEntityViewModel;
    }

    void Locator::ResetViewModel(winrt::Windows::Foundation::IInspectable const& viewModel)
    {
        // 如果需要丢弃缓存、重建 VM，可在此实现
        if (viewModel)
        {
            if (auto vmc = viewModel.try_as<Mvvm::Framework::Core::IViewModelCleanup>())
            {
                vmc.FrameworkCleanup();
            }

            //viewModel = nullptr;        // 丢弃缓存
        }
    }
}
