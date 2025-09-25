#pragma once

#include "MyEntityViewModel.g.h"

#include "mvvm_framework/delegate_command.h"
#include "mvvm_framework/delegate_command_builder.h"

#include <winrt/Windows.Foundation.Metadata.h>

namespace wfmd = winrt::Windows::Foundation::Metadata;

namespace winrt::WinUI3MVVMSample1::implementation
{
    struct MyEntityViewModel
        : MyEntityViewModelT<MyEntityViewModel>
        , ::mvvm::view_model<MyEntityViewModel>
    {
        MyEntityViewModel()
        {
            // 初始化命令对象
            // 有多种方法可以初始化命令对象，比如这里的 4 种方法。

            // 方法1：创建命令对象，不绑定依赖属性和执行条件等，需要手动调用 raise_CanExecuteChanged
            /*m_resetCommand = winrt::make<mvvm::delegate_command<IInspectable>>(
                *this,
                [this](auto&&) { ExecuteResetCommand(); },
                [this](auto&&) { return MyProperty() > 0; });*/
            
            // 方法2：创建命令对象，绑定依赖属性和执行条件，自动调用 raise_CanExecuteChanged
            // 使用 std::vector 参数列表
            //m_resetCommand = winrt::make<mvvm::delegate_command<IInspectable>>(
            //    *this,
            //    [this](auto&&) { ExecuteResetCommand(); },
            //    [this](auto&&) { return MyProperty() > 0; },
            //    std::vector<mvvm::DependencyRegistration> {
            //        { L"MyProperty",
            //            [this](auto&&, auto&&) { return MyProperty() == 0 || MyProperty() == 1; },  // 值为0或1时重新检查命令执行条件
            //            [this](auto&&) { return MyProperty() >= 10; }   // 值大于等于10时强制执行命令
            //        }
            //    }
            //);

            // 方法3：先创建命令对象，然后再注册依赖属性和执行条件
            /*m_resetCommand = winrt::make<::mvvm::delegate_command<IInspectable>>(
                [this](auto&&) { ExecuteResetCommand(); },
                [this](auto&&) { return MyProperty() > 0; }
            );
            m_resetCommand.as<::mvvm::delegate_command<IInspectable>>()->AddRelayDependency(
                *this, 
                L"MyProperty", 
                [this](auto&&, auto&&) { return MyProperty() == 0 || MyProperty() == 1; });
            m_resetCommand.as<::mvvm::delegate_command<IInspectable>>()->AddAutoExecute(
                *this, 
                [this](auto&&) { return MyProperty() >= 10; });*/

            // 方法4：使用 DelegateCommandBuilder 来简化命令对象创建和初始化
            m_resetCommand = ::mvvm::DelegateCommandBuilder<IInspectable>(*this)
                .Execute([this](auto&&) { ExecuteResetCommand(); })
                .CanExecute([this](auto&&) { return MyProperty() > 0; })
                .DependsOn(L"MyProperty",
                    [this](auto&&, auto&&) { return MyProperty() == 0 || MyProperty() == 1; },
                    [this](auto&&) { return MyProperty() >= 10; })
                .Build();
            /*m_resetCommand = ::mvvm::DelegateCommandBuilder<IInspectable>(nullptr)
                .Execute([this](auto&&) { ExecuteResetCommand(); })
                .CanExecute([this](auto&&) { return MyProperty() > 0; })
                .DependsOn(L"")
                .Build();*/

            if (wfmd::ApiInformation::IsPropertyPresent(L"WinUI3MVVMSample1.MyEntityViewModel", L"MainViewModel"))
            {
                OutputDebugString(L"ICommand.ResetCommand is present\n");
            } else
            {
                OutputDebugString(L"ICommand.ResetCommand is not present\n");
            }

        };

        int32_t MyProperty() { return get_property(m_myProperty); }
        void MyProperty(int32_t value)
        {
            // 使用set_property来触发PropertyChanged事件
            if (set_property(m_myProperty, value, NAME_OF(MyEntityViewModel, MyProperty)))
            {
                OutputDebugString(L"MyProperty changed\n");
                // （方法1需要）手动调用 raise_CanExecuteChanged
                /*m_resetCommand.as<mvvm::delegate_command<winrt::Windows::Foundation::IInspectable>>()
                    ->raise_CanExecuteChanged();*/
            }
        }

        void IncrementProperty()
        {
            MyProperty(MyProperty() + 1);
        }

        // 命令逻辑
        void ExecuteResetCommand()
        {
            MyProperty(0);
        }

        bool CanExecuteResetCommand()
        {
            return MyProperty() != 0;
        }

        // 命令属性
        winrt::Microsoft::UI::Xaml::Input::ICommand ResetCommand()
        {
            return m_resetCommand;
        }

        void ResetCommand(winrt::Microsoft::UI::Xaml::Input::ICommand const& value)
        {
            m_resetCommand = value;
        }

    private:
        int32_t m_myProperty{};
        winrt::Microsoft::UI::Xaml::Input::ICommand m_resetCommand{ nullptr };
    };
}

namespace winrt::WinUI3MVVMSample1::factory_implementation
{
    struct MyEntityViewModel
        : MyEntityViewModelT<MyEntityViewModel, implementation::MyEntityViewModel>
    {
    };
}
