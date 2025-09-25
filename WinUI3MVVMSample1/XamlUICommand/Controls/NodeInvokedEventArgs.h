#pragma once
#include "NodeInvokedEventArgs.g.h"

namespace winrt::XamlUICommand::implementation
{
    struct NodeInvokedEventArgs : NodeInvokedEventArgsT<NodeInvokedEventArgs>
    {
        NodeInvokedEventArgs() = default;

        XamlUICommand::NodeViewModel Node() const { return m_node; }
        void Node(XamlUICommand::NodeViewModel const& value) { m_node = value; }

    private:
        XamlUICommand::NodeViewModel m_node{ nullptr };
    };
}

namespace winrt::XamlUICommand::factory_implementation
{
    struct NodeInvokedEventArgs : NodeInvokedEventArgsT<NodeInvokedEventArgs, implementation::NodeInvokedEventArgs> {};
}
