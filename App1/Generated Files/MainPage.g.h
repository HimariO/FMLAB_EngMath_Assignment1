﻿#pragma once
//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------

#include "XamlBindingInfo.g.h"

namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Controls {
                ref class ListView;
                ref class AutoSuggestBox;
            }
        }
    }
}

namespace App1
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class MainPage : public ::Windows::UI::Xaml::Controls::Page, 
        public ::Windows::UI::Xaml::Markup::IComponentConnector,
        public ::Windows::UI::Xaml::Markup::IComponentConnector2
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, ::Platform::Object^ target);
        virtual ::Windows::UI::Xaml::Markup::IComponentConnector^ GetBindingConnector(int connectionId, ::Platform::Object^ target);
    
    private:
        bool _contentLoaded;
        class MainPage_obj5_Bindings;
        class MainPage_obj9_Bindings;
        class MainPage_obj12_Bindings;
    
        ::XamlBindingInfo::XamlBindings^ Bindings;
        private: ::Windows::UI::Xaml::Controls::ListView^ DirList;
        private: ::Windows::UI::Xaml::Controls::ListView^ CenterllistView;
        private: ::Windows::UI::Xaml::Controls::AutoSuggestBox^ CommandLine;
        private: ::Windows::UI::Xaml::Controls::ListView^ listView1;
    };
}

