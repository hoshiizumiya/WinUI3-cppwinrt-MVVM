#pragma once
#ifndef MVVM_FRAMEWORK_CORE_H
#define MVVM_FRAMEWORK_CORE_H

//#include "Mvvm/Framework/Core/IObservablePropertyBase.g.h"
#include "Mvvm/Framework/Core/ObservableBoolean.g.h"
#include "Mvvm/Framework/Core/ObservableByte.g.h"
#include "Mvvm/Framework/Core/ObservableInt8.g.h"
#include "Mvvm/Framework/Core/ObservableInt16.g.h"
#include "Mvvm/Framework/Core/ObservableUInt16.g.h"
#include "Mvvm/Framework/Core/ObservableInt32.g.h"
#include "Mvvm/Framework/Core/ObservableUInt32.g.h"
#include "Mvvm/Framework/Core/ObservableInt64.g.h"
#include "Mvvm/Framework/Core/ObservableUInt64.g.h"
#include "Mvvm/Framework/Core/ObservableSingle.g.h"
#include "Mvvm/Framework/Core/ObservableDouble.g.h"
#include "Mvvm/Framework/Core/ObservableChar16.g.h"
#include "Mvvm/Framework/Core/ObservableString.g.h"

namespace winrt::Mvvm::Framework::Core::implementation
{
    struct ObservablePropertyBase
    {
        bool IsObservable() { return true; }
    };

    template <typename T>
    struct ObservableBaseT : ObservablePropertyBase
    {
        ObservableBaseT() = default;
        ObservableBaseT(T const& value) : m_value(value) {}

        T Value() const { return m_value; }
        void Value(T const& value) { m_value = value; }

    private:
        T m_value{};
    };

    struct ObservableBoolean : ObservableBooleanT<ObservableBoolean>, ObservableBaseT<bool>
    {
        // 在 C++11 中，引入了构造函数继承的特性，允许派生类通过简单的声明来继承基类的构造函数。
        // 这一特性主要解决了在继承体系中，派生类需要显式调用基类构造函数以完成初始化的问题。
        // 当基类拥有多个构造函数时，派生类不再需要为每个基类构造函数编写对应的“透传”构造函数，
        // 从而减少代码重复，提高代码的清晰度和可维护性。
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableByte : ObservableByteT<ObservableByte>, ObservableBaseT<uint8_t>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableInt8 : ObservableInt8T<ObservableInt8>, ObservablePropertyBase
    {
        ObservableInt8() = default;
        ObservableInt8(int8_t v) : m_value(static_cast<uint8_t>(v)) {}

        uint8_t RawValue() const { return m_value; }
        void RawValue(uint8_t v) { m_value = v; }
        int32_t SignedValue() const { return static_cast<int8_t>(m_value); }

        int8_t ValueSigned() const { return static_cast<int8_t>(m_value); }
        void ValueSigned(int8_t v) { m_value = static_cast<uint8_t>(v); }

    private:
        uint8_t m_value{};
    };

    struct ObservableInt16 : ObservableInt16T<ObservableInt16>, ObservableBaseT<int16_t>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableUInt16 : ObservableUInt16T<ObservableUInt16>, ObservableBaseT<uint16_t>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableInt32 : ObservableInt32T<ObservableInt32>, ObservableBaseT<int32_t>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableUInt32 : ObservableUInt32T<ObservableUInt32>, ObservableBaseT<uint32_t>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableInt64 : ObservableInt64T<ObservableInt64>, ObservableBaseT<int64_t>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableUInt64 : ObservableUInt64T<ObservableUInt64>, ObservableBaseT<uint64_t>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableSingle : ObservableSingleT<ObservableSingle>, ObservableBaseT<float>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableDouble : ObservableDoubleT<ObservableDouble>, ObservableBaseT<double>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableChar16 : ObservableChar16T<ObservableChar16>, ObservableBaseT<wchar_t>
    {
        using ObservableBaseT::ObservableBaseT;
    };

    struct ObservableString : ObservableStringT<ObservableString>, ObservableBaseT<hstring>
    {
        using ObservableBaseT::ObservableBaseT;
    };
}

namespace winrt::Mvvm::Framework::Core::factory_implementation
{
    struct ObservableBoolean : ObservableBooleanT<ObservableBoolean, implementation::ObservableBoolean> {};
    struct ObservableByte : ObservableByteT<ObservableByte, implementation::ObservableByte> {};
    struct ObservableInt8 : ObservableInt8T<ObservableInt8, implementation::ObservableInt8> {};
    struct ObservableInt16 : ObservableInt16T<ObservableInt16, implementation::ObservableInt16> {};
    struct ObservableUInt16 : ObservableUInt16T<ObservableUInt16, implementation::ObservableUInt16> {};
    struct ObservableInt32 : ObservableInt32T<ObservableInt32, implementation::ObservableInt32> {};
    struct ObservableUInt32 : ObservableUInt32T<ObservableUInt32, implementation::ObservableUInt32> {};
    struct ObservableInt64 : ObservableInt64T<ObservableInt64, implementation::ObservableInt64> {};
    struct ObservableUInt64 : ObservableUInt64T<ObservableUInt64, implementation::ObservableUInt64> {};
    struct ObservableSingle : ObservableSingleT<ObservableSingle, implementation::ObservableSingle> {};
    struct ObservableDouble : ObservableDoubleT<ObservableDouble, implementation::ObservableDouble> {};
    struct ObservableChar16 : ObservableChar16T<ObservableChar16, implementation::ObservableChar16> {};
    struct ObservableString : ObservableStringT<ObservableString, implementation::ObservableString> {};
}

#endif  // MVVM_FRAMEWORK_CORE_H