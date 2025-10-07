#pragma once
#include "MyEntityViewModel.g.h"

#include "mvvm_framework/delegate_command.h"
#include "mvvm_framework/delegate_command_builder.h"
#include "mvvm_framework/async_command_builder.h"

#include <atomic>
#include <memory>

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Windows.Foundation.Metadata.h>

#include "Models/MyEntity.h" // 引入 Model 层

namespace winrt::WinUI3MVVMSample1::implementation
{
    struct MyEntityViewModel
        : MyEntityViewModelT<MyEntityViewModel>
        , ::mvvm::ViewModel<MyEntityViewModel>
    {
        MyEntityViewModel();

        void SetDispatcherQueue(winrt::Microsoft::UI::Dispatching::DispatcherQueue const& dq) noexcept
        {
            m_ui = dq;
        }

        // 基本属性
        int32_t MyProperty();
        void    MyProperty(int32_t value);
        void    IncrementProperty();

        winrt::hstring FirstName();
        void           FirstName(winrt::hstring const& value);
        winrt::hstring LastName();
        void           LastName(winrt::hstring const& value);

        // FullName 交给 Model 组合
        winrt::hstring FullName();

        int32_t        Age();
        void           Age(int32_t v);
        winrt::hstring AgeText();
        void           AgeText(winrt::hstring const& v);

        winrt::hstring AgeErrorsText();
        void           AgeErrorsText(winrt::hstring const& v);

        // 命令
        winrt::Microsoft::UI::Xaml::Input::ICommand ResetCommand();
        void ResetCommand(winrt::Microsoft::UI::Xaml::Input::ICommand const& value);

        Microsoft::UI::Xaml::Input::ICommand AsyncCommand();
        void                                 AsyncCommand(Microsoft::UI::Xaml::Input::ICommand const& v);
        Microsoft::UI::Xaml::Input::ICommand CancelAsyncOpCommand();
        void                                 CancelAsyncOpCommand(Microsoft::UI::Xaml::Input::ICommand const& v);

        bool            IsValid();
        void            IsValid(bool v);
        bool            IsBusy();
        void            IsBusy(bool v);
        winrt::hstring  StatusText();
        void            StatusText(winrt::hstring const& v);

        // 与 Model 交互
        winrt::Windows::Foundation::IAsyncAction DoSaveAsync();
        void                                    CancelSave();

    private:
        // 业务服务
        std::shared_ptr<WinUI3MVVMSample1::Models::IMyEntityService> m_service{ std::make_shared<WinUI3MVVMSample1::Models::MyEntityService>() };
        WinUI3MVVMSample1::Models::MyEntity m_entity{}; // 持有实体

        int32_t m_myProperty{};
        winrt::Microsoft::UI::Xaml::Input::ICommand m_resetCommand{ nullptr };

        winrt::hstring m_firstName{ L"John" };
        winrt::hstring m_lastName{ L"Doe" };

        int32_t       m_age{ 18 };
        winrt::hstring m_ageText{ L"18" };
        winrt::hstring m_ageErrorsText{};

        Microsoft::UI::Xaml::Input::ICommand m_asyncCommand{ nullptr };
        Microsoft::UI::Xaml::Input::ICommand m_cancelAsyncOpCommand{ nullptr };
        bool          m_isValid{ true };
        bool          m_isBusy{ false };
        winrt::hstring m_statusText{};

        std::shared_ptr<std::atomic_bool> m_cancelRequested{ std::make_shared<std::atomic_bool>(false) };
        winrt::Microsoft::UI::Dispatching::DispatcherQueue m_ui{ nullptr };
    };
}

namespace winrt::WinUI3MVVMSample1::factory_implementation
{
    struct MyEntityViewModel
        : MyEntityViewModelT<MyEntityViewModel, implementation::MyEntityViewModel>
    {
    };
}
