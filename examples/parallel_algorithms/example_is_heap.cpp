///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2017 Taeguk Kwon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/include/parallel_is_heap.hpp>
#include <hpx/include/parallel_generate.hpp>
#include <hpx/util/high_resolution_clock.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/random.hpp>

#include <cstddef>
#include <list>
#include <mutex>
#include <set>
#include <vector>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
struct random_fill
{
    random_fill()
        : gen(std::rand()),
        dist(0, RAND_MAX)
    {}

    int operator()()
    {
        return dist(gen);
    }

    boost::random::mt19937 gen;
    boost::random::uniform_int_distribution<> dist;

    template <typename Archive>
    void serialize(Archive& ar, unsigned)
    {}
};

double run_is_heap_until_element_benchmark_seq(int test_count, std::vector<int> const& v)
{
    std::cout << "--- run_is_heap_until_element_benchmark_seq ---" << std::endl;
    std::uint64_t time = hpx::util::high_resolution_clock::now();

    for (int i = 0; i != test_count; ++i)
    {
        /* auto result = */std::is_heap_until(std::begin(v), std::end(v));
    }

    time = hpx::util::high_resolution_clock::now() - time;

    return (time * 1e-9) / test_count;
}

///////////////////////////////////////////////////////////////////////////////
double run_is_heap_until_element_benchmark_par(int test_count, std::vector<int> const& v)
{
    std::cout << "--- run_is_heap_until_element_benchmark_par ---" << std::endl;
    std::uint64_t time = hpx::util::high_resolution_clock::now();

    for (int i = 0; i != test_count; ++i)
    {
        using namespace hpx::parallel;
        /* auto result = */is_heap_until(execution::par, std::begin(v), std::end(v));
    }

    time = hpx::util::high_resolution_clock::now() - time;

    return (time * 1e-9) / test_count;
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    unsigned int seed = (unsigned int)std::time(nullptr);
    if (vm.count("seed"))
        seed = vm["seed"].as<unsigned int>();

    std::cout << "using seed: " << seed << std::endl;
    std::srand(seed);

    // pull values from cmd
    std::size_t size = vm["vector_size"].as<std::size_t>();
    //bool csvoutput = vm.count("csv_output") != 0;
    int test_count = vm["test_count"].as<int>();

    std::vector<int> v(size);

    // initialize data
    using namespace hpx::parallel;
    generate(execution::par, std::begin(v), std::end(v), random_fill());
    std::make_heap(std::begin(v), std::end(v));

    double time_seq = run_is_heap_until_element_benchmark_seq(test_count, v);
    double time_par = run_is_heap_until_element_benchmark_par(test_count, v);

    std::cout << "is_heap_until (seq) " << test_count << ", " << time_seq << std::endl;
    std::cout << "is_heap_until (par)" << test_count << ", " << time_par << std::endl;

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;
    options_description desc_commandline(
        "usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
        ("vector_size",
            boost::program_options::value<std::size_t>()->default_value(10000),
            "size of vector (default: 10000)")
        ("test_count",
            boost::program_options::value<int>()->default_value(1),
            "number of tests to be averaged (default: 1)")
        ("csv_output",
            "print results in csv format")
        ("seed,s", boost::program_options::value<unsigned int>(),
            "the random number generator seed to use for this run")
        ;

    // initialize program
    std::vector<std::string> const cfg = {
        "hpx.os_threads=all"
        //"hpx.run_hpx_main=1"
    };


    std::cout << "--- asdf ---" << std::endl;

    // Initialize and run HPX
    HPX_TEST_EQ_MSG(hpx::init(desc_commandline, argc, argv, cfg), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
