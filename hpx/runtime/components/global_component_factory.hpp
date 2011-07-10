//  Copyright (c) 2007-2011 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_65762D5B_C8F6_41DC_98AD_804B77311AA7)
#define HPX_65762D5B_C8F6_41DC_98AD_804B77311AA7

#include <boost/mpl/for_each.hpp>

#include <hpx/runtime/components/component_factory.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components
{
    template <void F()>
    struct free_function
    {
        void operator() (void) const
        { F(); }
    };

    template <typename Component, typename StartFunctions, typename StopFunctions>
    struct global_component_factory : component_factory<Component>
    {
        typedef component_factory<Component> base_type;

        typedef StartFunctions start_functions_type; 
        typedef StopFunctions start_functions_type; 

        struct invoker
        {
            template <typename F>
            void operator() (F f) const
            { f(); } 
        };

        global_component_factory(util::section const* global,
                                 util::section const* local)
          : base_type(global, local)
        {
            boost::mpl::for_each<start_functions_type>(invoker());
        }

        ~global_component_factory()
        {
            boost::mpl::for_each<stop_functions_type>(invoker());
        }
    };
}}

#define HPX_REGISTER_GLOBAL_COMPONENT_FACTORY(type, starts, stops, name)      \
        HPX_REGISTER_COMPONENT_FACTORY(                                       \
            hpx::components::global_component_factory<                        \
                type, starts, stops>,                                         \
            name);                                                            \
        HPX_DEF_UNIQUE_COMPONENT_NAME(                                        \
            hpx::components::global_component_factory<                        \
                type, starts, stops>,                                         \
            name)                                                             \
        template struct hpx::components::global_component_factory<            \
            type, starts, stops>;                                             \
        HPX_REGISTER_MINIMAL_COMPONENT_REGISTRY(type, name)                   \
    /**/

#endif // HPX_65762D5B_C8F6_41DC_98AD_804B77311AA7

