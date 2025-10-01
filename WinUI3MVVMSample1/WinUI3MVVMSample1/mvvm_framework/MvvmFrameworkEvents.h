#pragma once

#ifndef MVVM_FRAMEWORK_CORE_H
#define MVVM_FRAMEWORK_CORE_H

#include "Mvvm/Framework/Core/CanExecuteRequestedEventArgs.g.h"
#include "Mvvm/Framework/Core/CanExecuteCompletedEventArgs.g.h"
#include "Mvvm/Framework/Core/ExecuteRequestedEventArgs.g.h"
#include "Mvvm/Framework/Core/ExecuteCompletedEventArgs.g.h"

namespace winrt::Mvvm::Framework::Core::implementation
{
    struct CanExecuteRequestedEventArgs : CanExecuteRequestedEventArgsT<CanExecuteRequestedEventArgs>
    {
        CanExecuteRequestedEventArgs(winrt::Windows::Foundation::IInspectable const& parameter)
            : m_parameter(parameter)
        {
        }

        winrt::Windows::Foundation::IInspectable Parameter() const { return m_parameter; }
        bool Handled() const { return m_handled; }
        void Handled(bool value) { m_handled = value; }

    private:
        winrt::Windows::Foundation::IInspectable m_parameter{ nullptr };
        bool m_handled{ false };
    };

    struct CanExecuteCompletedEventArgs : CanExecuteCompletedEventArgsT<CanExecuteCompletedEventArgs>
    {
        CanExecuteCompletedEventArgs(winrt::Windows::Foundation::IInspectable const& parameter, bool result)
            : m_parameter(parameter), m_result(result)
        {
        }

        winrt::Windows::Foundation::IInspectable Parameter() const { return m_parameter; }
        bool Result() const { return m_result; }

    private:
        winrt::Windows::Foundation::IInspectable m_parameter{ nullptr };
        bool m_result{ false };
    };

    struct ExecuteRequestedEventArgs : ExecuteRequestedEventArgsT<ExecuteRequestedEventArgs>
    {
        ExecuteRequestedEventArgs(winrt::Windows::Foundation::IInspectable const& parameter)
            : m_parameter(parameter)
        {
        }

        winrt::Windows::Foundation::IInspectable Parameter() const { return m_parameter; }

    private:
        winrt::Windows::Foundation::IInspectable m_parameter{ nullptr };
    };

    struct ExecuteCompletedEventArgs : ExecuteCompletedEventArgsT<ExecuteCompletedEventArgs>
    {
        ExecuteCompletedEventArgs(winrt::Windows::Foundation::IInspectable const& parameter, int32_t hresult)
            : m_parameter(parameter), m_error(hresult), m_succeeded(hresult >= 0)
        {
        }

        winrt::Windows::Foundation::IInspectable Parameter() const { return m_parameter; }
        bool Succeeded() const { return m_succeeded; }
        int32_t Error() const { return m_error; }

    private:
        winrt::Windows::Foundation::IInspectable m_parameter{ nullptr };
        int32_t m_error{ 0 };
        bool m_succeeded{ false };
    };
}

namespace winrt::Mvvm::Framework::Core::factory_implementation
{
    struct CanExecuteRequestedEventArgs : CanExecuteRequestedEventArgsT<CanExecuteRequestedEventArgs, implementation::CanExecuteRequestedEventArgs>
    {
    };

    struct CanExecuteCompletedEventArgs : CanExecuteCompletedEventArgsT<CanExecuteCompletedEventArgs, implementation::CanExecuteCompletedEventArgs>
    {
    };

    struct ExecuteRequestedEventArgs : ExecuteRequestedEventArgsT<ExecuteRequestedEventArgs, implementation::ExecuteRequestedEventArgs>
    {
    };

    struct ExecuteCompletedEventArgs : ExecuteCompletedEventArgsT<ExecuteCompletedEventArgs, implementation::ExecuteCompletedEventArgs>
    {
    };
}

#endif // MVVM_FRAMEWORK_CORE_H