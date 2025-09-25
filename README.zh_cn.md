# 适用于 C++/WinRT 的 MVVM
[MVVM 设计模式](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93viewmodel) 在基于 XAML 的 UI 应用中非常常见。该库（名为 **mvvm-winrt**）提供了一组非常轻量的类与宏，用于简化 MVVM 类的实现。

**mvvm-winrt** 是一个纯头文件的模板库，采用[静态（编译期）多态](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern#Static_polymorphism)，在启用优化编译时能获得更高的效率。

## 视图模型类

### ```mvvm::notify_property_changed<Derived>```
```cpp
#include <mvvm/notify_property_changed.h>
```

#### 声明
```cpp
template <typename Derived>
struct __declspec(empty_bases) notify_property_changed
```

#### 模板类型参数
```Derived``` 
: 最派生类；即你正在实现的那个类型。

#### 说明

该文件实现了 ```notify_property_changed<Derived>``` 类模板，提供了 ```INotifyPropertyChanged``` 的基础实现。尽管该接口只声明了一个成员（```PropertyChanged``` 事件），该类还额外提供了受保护的方法，用于在最常见场景下大幅简化属性的 setter 与 getter ：

- 返回一个 ```bool``` 指示属性值是否发生变化的 setter 实现。
: 参见返回 ```bool``` 的 ```set_property``` 重载：
```cpp
template <typename Value>    // 以下各方法均如此

bool set_property(Value& valueField, Value const& newValue);
bool set_property(Value& valueField, Value const& newValue, Value& oldValue);
bool set_property(Value& valueField, Value const& newValue, std::wstring_view const& propertyName);
bool set_property(Value& valueField, Value const& newValue, Value& oldValue, std::wstring_view const& propertyName);
bool set_property(Value& valueField, Value const& newValue, std::initializer_list<const std::wstring_view> propertyNames);
bool set_property(Value& valueField, Value const& newValue, Value& oldValue, std::initializer_list<const std::wstring_view> propertyNames);
```

- 返回更新前字段旧值的 setter 实现。
参见带 ```oldValue``` 引用参数的 ```set_property*``` 重载：
```cpp
template <typename Value>    // 以下各方法均如此

bool set_property(Value& valueField, Value const& newValue, Value& oldValue);
bool set_property(Value& valueField, Value const& newValue, Value& oldValue, std::wstring_view const& propertyName);
bool set_property(Value& valueField, Value const& newValue, Value& oldValue, std::initializer_list<const std::wstring_view> propertyNames);
void set_property_no_compare(Value& valueField, Value const& newValue, Value& oldValue, std::wstring_view const& propertyName);
void set_property_no_compare(Value& valueField, Value const& newValue, Value& oldValue, std::initializer_list<const std::wstring_view> propertyNames);
void set_property_no_compare_no_notify(Value& valueField, Value const& newValue, Value& oldValue);
```

- 无论值是否变化，始终引发 ```PropertyChanged``` 事件的 setter 实现：
: 参见 ```set_property_no_compare*``` 方法：
```cpp
template <typename Value>    // 以下各方法均如此

void set_property_no_compare(Value& valueField, Value const& newValue, std::wstring_view const& propertyName);
void set_property_no_compare(Value& valueField, Value const& newValue, Value& oldValue, std::wstring_view const& propertyName);
void set_property_no_compare(Value& valueField, Value const& newValue, std::initializer_list<const std::wstring_view> propertyNames);
void set_property_no_compare(Value& valueField, Value const& newValue, Value& oldValue, std::initializer_list<const std::wstring_view> propertyNames);
void set_property_no_compare_no_notify(Value& valueField, Value const& newValue);
void set_property_no_compare_no_notify(Value& valueField, Value const& newValue, Value& oldValue);
```

- 永不引发 ```PropertyChanged``` 事件的 setter 实现：
```cpp
template <typename Value>    // 以下各方法均如此

bool set_property(Value& valueField, Value const& newValue);
bool set_property(Value& valueField, Value const& newValue, Value& oldValue);
void set_property_no_compare_no_notify(Value& valueField, Value const& newValue);
void set_property_no_compare_no_notify(Value& valueField, Value const& newValue, Value& oldValue);
```

- 仅为单个属性名引发 ```PropertyChanged``` 事件的 setter 实现：
```cpp
template <typename Value>    // 以下各方法均如此

bool set_property(Value& valueField, Value const& newValue, std::wstring_view const& propertyName);
bool set_property(Value& valueField, Value const& newValue, Value& Value, std::wstring_view const& propertyName);
void set_property_no_compare(Value& valueField, Value const& newValue, std::wstring_view const& propertyName);
void set_property_no_compare(Value& valueField, Value const& newValue, Value& oldValue, std::wstring_view const& propertyName);
```

- 为一组属性名引发 ```PropertyChanged``` 事件的 setter 实现：
```cpp
template <typename Value>    // 以下各方法均如此

bool set_property(Value& valueField, Value const& newValue, std::initializer_list<const std::wstring_view> propertyNames);
bool set_property(Value& valueField, Value const& newValue, Value& Value, std::initializer_list<const std::wstring_view> propertyNames);
void set_property_no_compare(Value& valueField, Value const& newValue, std::initializer_list<const std::wstring_view> propertyNames);
void set_property_no_compare(Value& valueField, Value const& newValue, Value& oldValue, std::initializer_list<const std::wstring_view> propertyNames);
```

- 可扩展性：允许派生类提供自定义的线程同步风格。
: 相关讨论参见后文的 ```view_model_base``` 类。

### ```mvvm::view_model_base<Derived>```
```cpp
#include <mvvm/view_model_base.h>
```

该类对 ```Derived``` 类中的 ```Dispatcher``` 属性有依赖。注意本类并不声明或实现该属性，需由派生类自行提供。下面的其他类给出了相应实现。

```get_property_core``` 与 ```set_property_core``` 方法负责访问与设置底层字段的逻辑。应当在派生类场景所需的线程同步上下文内调用这些基础方法。其中一种派生实现是 ```view_model_base``` 类模板。它通过重写 ```get_property_override``` 与 ```set_property_override```，在 ```CoreDispatcher``` 上下文中调用 ```get_property_core``` 与 ```set_property_core```。

### ```mvvm::view_model<Derived>```
```cpp
#include <mvvm/view_model.h>
```

该类添加了其基类 ```view_model_base<Derived>``` 所使用的 ```Dispatcher``` 属性。

## 视图类

### ```mvvm::view<Derived, ViewModel>```
```cpp
#include <mvvm/view.h>
```

该类应作为一个 ```UIElement```（通常为 ```UserControl```）派生类的 mix-in。它还添加了一个由成员变量支撑的 ```ViewModel``` 属性。

### ```mvvm::view_sync_data_context```
```cpp
#include <mvvm/view_sync_data_context.h>
```

该类应作为一个 ```UIElement```（通常为 ```UserControl```）派生类的 mix-in。它添加了一个 ```ViewModel``` 属性，并将其与作为 mix-in 目标的 ```UIElement``` 的 ```DataContext``` 属性保持同步。在 XAML 中需要让 ```DataContext``` 与 ```ViewModel``` 属性保持一致时，这有时很有用。

## 命令类

### ```mvvm::delegate_command```
```cpp
#include <mvvm/delegate_command.h>
```

该类实现了命令模式，通过将调用延迟到提供给构造函数的 lambda 来实现 ```ICommand```。由于 ```{x:Bind}``` 能直接绑定到方法，这种模式现在使用得更少。但在需要 ```CanExecute``` 功能时仍然有用。该类也为 MVVM 基础库的完整性而提供。

## 宏

###  ```NAME_OF``` 与 ```NAME_OF_NARROW```
```cpp
#include <mvvm/name_of.h>
```
#### 声明
```cpp
NAME_OF(typeName, propertyName)
NAME_OF_NARROW(typeName, propertyName)
```
#### 宏参数

```typeName``` 
: 实现 ```propertyName``` 所指成员的类型名。不是字符串，只是类型的文本名称。

```propertyName``` 
: 该类型实现的成员（通常为属性）的名称。不是字符串。

#### 说明

```NAME_OF```（以及用于 8 位字符的 ```NAME_OF_NARROW```）可将属性名生成为 ```constexpr``` 的 ```wstring_view``` 或 ```string_view```。这在接收 ```PropertyChanged``` 事件时检查变更属性、以及在视图模型中引发该事件时很常见。它会在编译期强制检查 ```propertyName``` 是否是 ```typeName``` 的成员，然后将 ```propertyName``` 以 ```constexpr wstring_view```（或 ```string_view```）返回。在启用 O1 或 O2 优化时，微软编译器会去重字符串字面量，因此与直接硬编码字符串或放在全局变量相比，使用该宏在运行时没有额外开销。该宏通过三目运算符实现；编译器能在编译期判断条件恒为 false，最终只留下指向 ```propertyName``` 的 ```string_view```。由于其返回 ```wstring_view``` 或 ```string_view```，返回值可直接与 ```wchar_t*``` 或 ```char*```（无论是否 ```const```）进行比较。

### 事件宏

```cpp
DEFINE_EVENT(type, name)
```

```type```
: 事件处理程序类型，例如 ```RoutedEventHandler```。

```name```
: 事件名称。

### 简单/常见属性宏
```cpp
#include <mvvm/property_macros.h>

DEFINE_PROPERTY(type, name, defaulValue)
DEFINE_PROPERTY_PRIVATE_SET(type, name, defaulValue)
DEFINE_PROPERTY_PROTECTED_SET(type, name, defaulValue)
DEFINE_PROPERTY_READONLY(type, name, defaulValue)
```

```type```
: 属性的类型。

```name```
: 属性名称。建议使用下文的 ```NAME_OF``` 宏。

```defaultValue```
: 若需类型默认值请使用 ```{}```。

### 带值变更回调的属性宏
```cpp
#include <mvvm/property_macros.h>

DEFINE_PROPERTY_CALLBACK(type, name, defaulValue)
DEFINE_PROPERTY_CALLBACK_PRIVATE_SET(type, name, defaulValue)
DEFINE_PROPERTY_CALLBACK_PROTECTED_SET(type, name, defaulValue)
```

这些宏会声明一个受保护的实例方法，命名与签名遵循以下模式，其中 ```##name##``` 为属性名。类应在其 CPP 文件或类定义下方实现该方法：
```cpp
void On##name##Changed(type const& oldValue, type const& newValue);
```

### 不引发 ```PropertyChanged``` 事件的属性宏
```cpp
#include <mvvm/property_macros.h>

DEFINE_PROPERTY_NO_NOTIFY(type, name, defaulValue)
DEFINE_PROPERTY_NO_NOTIFY_PRIVATE_SET(type, name, defaulValue)
DEFINE_PROPERTY_NO_NOTIFY_PROTECTED_SET(type, name, defaulValue)
```

### 无论值是否变化都引发 ```PropertyChanged``` 事件的属性宏
```cpp
#include <mvvm/property_macros.h>

DEFINE_PROPERTY_NO_COMPARE(type, name, defaulValue)
DEFINE_PROPERTY_NO_COMPARE_PRIVATE_SET(type, name, defaulValue)
DEFINE_PROPERTY_NO_COMPARE_PROTECTED_SET(type, name, defaulValue)
```
