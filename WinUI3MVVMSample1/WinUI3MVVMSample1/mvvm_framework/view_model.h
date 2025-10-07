//*********************************************************
//
//    Copyright (c) Millennium R&D Team. All rights reserved.
//    This code is licensed under the MIT License.
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
//    ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
//    TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//    PARTICULAR PURPOSE AND NONINFRINGEMENT.
//
//*********************************************************
#pragma once
#ifndef __MVVM_CPPWINRT_VIEW_MODEL_H_INCLUDED
#define __MVVM_CPPWINRT_VIEW_MODEL_H_INCLUDED

#include "view_model_base.h"
#include <winrt/Microsoft.UI.Dispatching.h>

namespace mvvm
{
    template <typename Derived>
    struct __declspec(empty_bases)ViewModel : ViewModelBase<Derived>
    {
        friend typename Derived;

        ViewModel(winrt::Microsoft::UI::Dispatching::DispatcherQueue const& dispatcher)
        {
            if (dispatcher)
            {
                m_dispatcher = dispatcher;
            }
            else
            {
                m_dispatcher = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
                //if (!m_dispatcher)
                //{
                //    // 尝试从窗口获取 DispatcherQueue
                //    if (auto window = winrt::Microsoft::UI::Xaml::Window::Current())
                //    {
                //        m_dispatcher = window.DispatcherQueue();
                //    }
                //}
                if (!m_dispatcher)
                {
                    throw winrt::hresult_wrong_thread(L"ViewModels must be instantiated on a UI thread."sv);
                }
            }
        }

        winrt::Microsoft::UI::Dispatching::DispatcherQueue Dispatcher() const { return m_dispatcher; }

        winrt::Microsoft::UI::Dispatching::DispatcherQueue GetDispatcherOverride() { return m_dispatcher; }

    private:
        ViewModel() : ViewModel(nullptr)
        {
            // Default constructor is private to ensure that the ViewModel is always constructed with a dispatcher.
            // This prevents issues with UI thread access.
            static_assert(!std::is_same_v<Derived, ViewModel>, "Default constructor is not allowed for ViewModel.");
            static_assert(std::is_base_of_v<ViewModel<Derived>, Derived>, "Derived class must inherit from ViewModel");
        }

    protected:
        winrt::Microsoft::UI::Dispatching::DispatcherQueue m_dispatcher{ nullptr };
    };
}

#endif // __MVVM_CPPWINRT_VIEW_MODEL_H_INCLUDED
