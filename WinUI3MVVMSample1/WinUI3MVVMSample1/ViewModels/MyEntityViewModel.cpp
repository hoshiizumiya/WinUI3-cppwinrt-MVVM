#include "pch.h"
#include "MyEntityViewModel.h"
#if __has_include("MyEntityViewModel.g.cpp")
#include "MyEntityViewModel.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Input;

namespace winrt::WinUI3MVVMSample1::implementation
{
    MyEntityViewModel::MyEntityViewModel()
    {
        // 初始化命令对象
        // 有多种方法可以初始化命令对象，比如这里的 4 种方法。

        // 方法1：创建命令对象，不绑定依赖属性和执行条件等，需要手动调用 RaiseCanExecuteChangedEvent
        /*
            m_resetCommand = winrt::make<mvvm::DelegateCommand<IInspectable>>(
                *this,
                [this](auto&&) { ExecuteResetCommand(); },
                [this](auto&&) { return MyProperty() > 0; });
        */

        // 方法2：创建命令对象，绑定依赖属性和执行条件，自动调用 RaiseCanExecuteChangedEvent
        /* 
            // 使用 std::vector 参数列表
            m_resetCommand = winrt::make<mvvm::DelegateCommand<IInspectable>>(
                *this,
                [this](auto&&) { ExecuteResetCommand(); },
                [this](auto&&) { return MyProperty() > 0; },
                std::vector<mvvm::DependencyRegistration> {
                    { L"MyProperty",
                        [this](auto&&, auto&&) { return MyProperty() == 0 || MyProperty() == 1; },  // 值为0或1时重新检查命令执行条件
                        [this](auto&&) { return MyProperty() >= 10; }   // 值大于等于10时强制执行命令
                    }
                }
            );
        */

        // 方法3：先创建命令对象，然后再注册依赖属性和执行条件
        /*
            m_resetCommand = winrt::make<::mvvm::DelegateCommand<IInspectable>>(
                [this](auto&&) { ExecuteResetCommand(); },
                [this](auto&&) { return MyProperty() > 0; }
            );
            m_resetCommand.as<::mvvm::DelegateCommand<IInspectable>>()->AddRelayDependency(
                *this,
                L"MyProperty",
                [this](auto&&, auto&&) { return MyProperty() == 0 || MyProperty() == 1; });
            m_resetCommand.as<::mvvm::DelegateCommand<IInspectable>>()->AddAutoExecute(
                *this,
                [this](auto&&) { return MyProperty() >= 10; });
        */

        // 方法中也可以使用 winrt::make_self 

        // 方法4：使用 DelegateCommandBuilder 来简化命令对象创建和初始化
        // ---- 命令注册 ----
        m_resetCommand = ::mvvm::DelegateCommandBuilder<winrt::Windows::Foundation::IInspectable>(*this)
            .Execute([this](auto&&) { MyProperty(0); })
            .CanExecute([this](auto&&) { return MyProperty() > 0; })
            .DependsOn(L"MyProperty",
                [this](auto&&, auto&&) { return MyProperty() == 0 || MyProperty() == 1; },
                [this](auto&&) { return MyProperty() >= 10; })
            .Build();

        m_asyncCommand = ::mvvm::AsyncCommandBuilder<void>(*this)
            .ExecuteAsync([this]() -> IAsyncAction { co_await DoSaveAsync(); })
            .CanExecute([this]() { return IsValid() && !IsBusy(); })
            .DependsOn(L"IsValid")
            .DependsOn(L"IsBusy")
            .Build();

        m_cancelAsyncOpCommand = ::mvvm::DelegateCommandBuilder<winrt::Windows::Foundation::IInspectable>(*this)
            .Execute([this](auto&&) { CancelSave(); })
            .CanExecute([this](auto&&)
                {
                    auto cmd = m_asyncCommand.try_as< ::mvvm::AsyncDelegateCommand<> >();
                    return IsBusy() && cmd && cmd->IsRunning();
                })
            .DependsOn(L"IsBusy")
            .Build();

        // 注册基于属性依赖的多属性更改通知：FullName 依赖 First/Last。当 First/Last 变化时，FullName 也会变化。
        RegisterDependency(L"FirstName", { L"FullName" });
        RegisterDependency(L"LastName", { L"FullName" });

        // 校验器：由 Model Service 完成实际的校验验证逻辑。VM 仅负责状态和命令的编排（即，数据转换）
        AddValidator<int>(L"Age", [this](int v)->std::optional<hstring>
            {
                if (!m_service->ValidateAge(v))
                    return hstring{ L"Age must be in [0, 130]." };
                return std::nullopt;
            });

        // 订阅事件
        auto dc = m_resetCommand.as<::mvvm::DelegateCommand<winrt::Windows::Foundation::IInspectable>>();

        dc->CanExecuteRequested(
            [](auto&& sender, auto&& args)
            {
                OutputDebugString(L"[Test] CanExecuteRequested fired\n");
                if (auto param = args.Parameter())
                {
                    OutputDebugString((L"    Parameter: " + param.as<winrt::hstring>() + L"\n").c_str());
                }

                // 我们可以通过设置 handled 为 true 来禁用命令
                // args.Handled(true);
            });

        dc->CanExecuteCompleted(
            [](auto&&, auto&& args)
            {
                std::wstring msg = L"[Test] CanExecuteCompleted fired, result = "
                    + std::to_wstring(args.Result()) + L"\n";
                OutputDebugString(msg.c_str());
            });

        dc->ExecuteRequested(
            [](auto&&, auto&& args)
            {
                OutputDebugString(L"[Test] ExecuteRequested fired\n");
                if (auto param = args.Parameter())
                {
                    OutputDebugString((L"    Parameter: " + param.as<winrt::hstring>() + L"\n").c_str());
                }
            });

        dc->ExecuteCompleted(
            [](auto&&, auto&& args)
            {
                std::wstring msg = L"[Test] ExecuteCompleted fired, succeeded = "
                    + std::to_wstring(args.Succeeded()) + L"\n";
                OutputDebugString(msg.c_str());

                if (!args.Succeeded())
                {
                    std::wstring err = L"    Error: " + std::to_wstring(args.Error()) + L"\n";
                    OutputDebugString(err.c_str());
                }
            });

        if (auto ac = m_asyncCommand.try_as< ::mvvm::AsyncDelegateCommand<> >())
        {
            auto weak = get_weak();
            ac->ExecuteCompleted([weak](auto&&, winrt::Mvvm::Framework::Core::ExecuteCompletedEventArgs const& args)
                {
                    const auto canceledHr = static_cast<int32_t>(winrt::hresult_canceled().code().value);
                    if (args.Error() != canceledHr)
                        return;

                    if (auto self = weak.get(); self)
                    {
                        winrt::Microsoft::UI::Dispatching::DispatcherQueue ui = self->m_ui;
                        if (!ui) return;

                        ui.TryEnqueue([weak]()
                            {
                                if (auto s = weak.get())
                                {
                                    s->StatusText(L"Save cancelled.");
                                    s->IsBusy(false);
                                }
                            });
                    }
                });
        }
    }

    // ------------ 简单属性 ------------
    int32_t MyEntityViewModel::MyProperty() { return GetProperty(m_myProperty); }
    void    MyEntityViewModel::MyProperty(int32_t value)
    {
        // 使用SetProperty来触发PropertyChanged事件
        if (SetProperty(m_myProperty, value, NAME_OF(MyEntityViewModel, MyProperty)))
        {
            OutputDebugString(L"MyProperty changed\n");
            // （方法1需要）手动调用 RaiseCanExecuteChangedEvent
            /*m_resetCommand.as<mvvm::delegate_command<winrt::Windows::Foundation::IInspectable>>()
                ->RaiseCanExecuteChangedEvent();*/
        }
    }
    void MyEntityViewModel::IncrementProperty() { MyProperty(MyProperty() + 1); }

    hstring MyEntityViewModel::FirstName() { return GetProperty(m_firstName); }
    void    MyEntityViewModel::FirstName(hstring const& v)
    {
        SetProperty(m_firstName, v, NAME_OF(MyEntityViewModel, FirstName));
        m_entity.FirstName = v;
    }

    hstring MyEntityViewModel::LastName() { return GetProperty(m_lastName); }
    void    MyEntityViewModel::LastName(hstring const& v)
    {
        SetProperty(m_lastName, v, NAME_OF(MyEntityViewModel, LastName));
        m_entity.LastName = v;
    }

    hstring MyEntityViewModel::FullName()
    {
        return m_service->FullName(m_firstName, m_lastName);
    }

    int32_t MyEntityViewModel::Age() { return GetProperty(m_age); }

    void MyEntityViewModel::Age(int32_t v)
    {
        if (!SetPropertyValidate(m_age, v, NAME_OF(MyEntityViewModel, Age)))
        {
            auto errs = GetValidateErrors(L"Age");
            std::wstring msg;
            for (auto const& e : errs)
            {
                if (!msg.empty())
                    msg += L"; ";

                msg += e.c_str();
            }
            AgeErrorsText(hstring{ msg });
            return;
        }
        m_entity.Age = v;
        AgeErrorsText(L"");
    }

    hstring MyEntityViewModel::AgeText() { return GetProperty(m_ageText); }
    void    MyEntityViewModel::AgeText(hstring const& v)
    {
        if (!SetProperty(m_ageText, v, NAME_OF(MyEntityViewModel, AgeText))) return;
        try
        {
            int val = std::stoi(std::wstring{ v });
            Age(val);
        }
        catch (...)
        {
            AgeErrorsText(L"Age is not a valid integer.");
        }
    }

    hstring MyEntityViewModel::AgeErrorsText() { return GetProperty(m_ageErrorsText); }
    void    MyEntityViewModel::AgeErrorsText(hstring const& v)
    {
        SetProperty(m_ageErrorsText, v, NAME_OF(MyEntityViewModel, AgeErrorsText));
    }

    // ------------ 命令属性 ------------
    Microsoft::UI::Xaml::Input::ICommand MyEntityViewModel::ResetCommand() { return m_resetCommand; }
    void MyEntityViewModel::ResetCommand(Microsoft::UI::Xaml::Input::ICommand const& v) { m_resetCommand = v; }
    Microsoft::UI::Xaml::Input::ICommand MyEntityViewModel::AsyncCommand() { return m_asyncCommand; }
    void MyEntityViewModel::AsyncCommand(Microsoft::UI::Xaml::Input::ICommand const& v) { m_asyncCommand = v; }
    Microsoft::UI::Xaml::Input::ICommand MyEntityViewModel::CancelAsyncOpCommand() { return m_cancelAsyncOpCommand; }
    void MyEntityViewModel::CancelAsyncOpCommand(Microsoft::UI::Xaml::Input::ICommand const& v)
    {
        m_cancelAsyncOpCommand = v;
    }

    bool MyEntityViewModel::IsValid() { return GetProperty(m_isValid); }
    void MyEntityViewModel::IsValid(bool v) { SetProperty(m_isValid, v, NAME_OF(MyEntityViewModel, IsValid)); }
    bool MyEntityViewModel::IsBusy() { return GetProperty(m_isBusy); }
    void MyEntityViewModel::IsBusy(bool v) { SetProperty(m_isBusy, v, NAME_OF(MyEntityViewModel, IsBusy)); }
    hstring MyEntityViewModel::StatusText() { return GetProperty(m_statusText); }
    void    MyEntityViewModel::StatusText(hstring const& v)
    { 
        SetProperty(m_statusText, v, NAME_OF(MyEntityViewModel, StatusText));
    }

    // ------------ 异步保存：交由 Model 执行，VM 仅做状态编排 ------------
    IAsyncAction MyEntityViewModel::DoSaveAsync()
    {
        m_cancelRequested->store(false, std::memory_order_relaxed);

        if (m_ui) co_await wil::resume_foreground(m_ui);
        IsBusy(true);
        StatusText(L"Saving...");

        co_await m_service->SaveAsync(m_cancelRequested);

        if (m_cancelRequested->load(std::memory_order_relaxed))
        {
            if (m_ui) co_await wil::resume_foreground(m_ui);
            StatusText(L"Save cancelled.");
            IsBusy(false);
            co_return;
        }

        if (m_ui) co_await wil::resume_foreground(m_ui);
        StatusText(L"Save completed.");
        IsBusy(false);
    }

    void MyEntityViewModel::CancelSave()
    {
        auto cmd = m_asyncCommand.try_as< ::mvvm::AsyncDelegateCommand<> >();
        if (!cmd) return;

        const bool running = cmd->IsRunning();
        if (!(running || IsBusy())) return;

        m_cancelRequested->store(true, std::memory_order_relaxed);
        if (m_ui) m_ui.TryEnqueue([weak = get_weak()]()
            {
                if (auto self = weak.get()) self->StatusText(L"Cancelling...");
            });
        cmd->Cancel();
    }
}
