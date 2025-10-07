#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <string>
#include <atomic>

namespace winrt::WinUI3MVVMSample1::Models
{
    struct MyEntity
    {
        winrt::hstring FirstName{ L"John" };
        winrt::hstring LastName{ L"Doe" };
        int32_t Age{ 18 };
    };

    struct IMyEntityService
    {
        virtual ~IMyEntityService() = default;

        virtual bool ValidateAge(int32_t age) const noexcept = 0;

        virtual winrt::hstring FullName(
            winrt::hstring const& first,
            winrt::hstring const& last) const = 0;

        virtual winrt::Windows::Foundation::IAsyncAction SaveAsync(
            std::shared_ptr<std::atomic_bool> cancel) = 0;
    };
  
    struct MyEntityService final : IMyEntityService
    {
        bool ValidateAge(int32_t age) const noexcept override;

        winrt::hstring FullName(
            winrt::hstring const& first,
            winrt::hstring const& last) const override;

        winrt::Windows::Foundation::IAsyncAction
            SaveAsync(std::shared_ptr<std::atomic_bool> cancel) override;
    };
}
