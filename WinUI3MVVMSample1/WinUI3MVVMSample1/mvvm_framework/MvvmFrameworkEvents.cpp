#include "pch.h"
#include "MvvmFrameworkEvents.h"

/*
    #include "Mvvm/Framework/Core/CanExecuteRequestedEventArgs.g.h"
    #include "Mvvm/Framework/Core/CanExecuteCompletedEventArgs.g.h"
    #include "Mvvm/Framework/Core/ExecuteRequestedEventArgs.g.h"
    #include "Mvvm/Framework/Core/ExecuteCompletedEventArgs.g.h"
*/
#if __has_include("Mvvm/Framework/Core/CanExecuteRequestedEventArgs.g.cpp")
#include "Mvvm/Framework/Core/CanExecuteRequestedEventArgs.g.cpp"
#endif
#if __has_include("Mvvm/Framework/Core/CanExecuteCompletedEventArgs.g.cpp")
#include "Mvvm/Framework/Core/CanExecuteCompletedEventArgs.g.cpp"
#endif
#if __has_include("Mvvm/Framework/Core/ExecuteRequestedEventArgs.g.cpp")
#include "Mvvm/Framework/Core/ExecuteRequestedEventArgs.g.cpp"
#endif
#if __has_include("Mvvm/Framework/Core/ExecuteCompletedEventArgs.g.cpp")
#include "Mvvm/Framework/Core/ExecuteCompletedEventArgs.g.cpp"
#endif


namespace winrt::Mvvm::Framework::Core::implementation
{
    // 模板基类已在头文件中实现，这里不需要额外方法实现
}