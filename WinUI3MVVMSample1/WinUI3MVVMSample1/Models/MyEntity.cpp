#include "pch.h"
#include "MyEntity.h"

namespace winrt::WinUI3MVVMSample1::Models
{
    bool MyEntityService::ValidateAge(int32_t age) const noexcept
    {
        return (age >= 0 && age <= 130);
    }

    winrt::hstring MyEntityService::FullName(
        winrt::hstring const& first, 
        winrt::hstring const& last) const
    {
        if (first.empty() && last.empty()) return L"";
        if (first.empty()) return last;
        if (last.empty())  return first;
        return first + L" " + last;
    }

    winrt::Windows::Foundation::IAsyncAction 
    MyEntityService::SaveAsync(
        std::shared_ptr<std::atomic_bool> cancel
    )
    {
        auto keep = std::move(cancel);

        for (int i = 0; i < 10; ++i)
        {
            co_await winrt::resume_after(std::chrono::milliseconds(200));
            if (keep->load(std::memory_order_relaxed))
                throw winrt::hresult_canceled();
        }
    }
}