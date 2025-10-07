#include "pch.h"
#include "ViewModels/Locator.h"
#include "Models/MyEntity.h"

using namespace winrt;

namespace winrt::WinUI3MVVMSample1::ViewModels
{
    WinUI3MVVMSample1::MyEntityViewModel Locator::MyEntity()
    {
        // 持久化一个 VM；如需每次新建可自行调整策略
        static WinUI3MVVMSample1::MyEntityViewModel s_vm;
        return s_vm;
    }

    void Locator::Reset()
    {
        // 如果需要丢弃缓存、重建 VM，可在此实现
    }
}
