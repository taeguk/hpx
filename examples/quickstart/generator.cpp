//  Copyright (c) 2017 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This example demonstrates the use of a one_element_channel to construct a
// generic generator interface.

#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/lcos.hpp>

#include <iostream>
#include <type_traits>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
// template <typename T, typename F>
// struct generator
// {
//     template <typename ... Ts>
//     bool operator()(Ts && ... ts) const
//     {
//         f_(std::forward<Ts>(ts)...);
//         return true;
//     }
//
//     typedef typename std::decay<F>::type function_type;
//     function_type f_;
// };
//
// template <typename T, typename F>
// generator<T, F> make_generator(F && f)
// {
//     return generator<T, F>(std::forward<F>(f));
// }

///////////////////////////////////////////////////////////////////////////////
void sync_generator()
{
    hpx::lcos::local::one_element_channel<int> gen;

    hpx::future<void> f =
        hpx::async([=]() mutable
        {
            for (int i = 0; i != 10; ++i)
                gen.set(i);
            gen.close();
        });

    for (int val : gen)
        std::cout << val << '\n';

    f.get();
}

void async_generator()
{
    hpx::lcos::local::one_element_channel<int> gen;

    hpx::future<void> f =
        hpx::async([=]() mutable
        {
            for (int i = 0; i != 10; ++i)
                gen.set(i);
            gen.close();
        });

    for (hpx::future<int> val : gen.range(hpx::launch::async))
        std::cout << val.get() << '\n';

    f.get();
}

int main(int argc, char* argv[])
{
    sync_generator();
    async_generator();

    return 0;
}
