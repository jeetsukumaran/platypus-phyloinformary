#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <tuple>
#include <platypus/model/datatable.hpp>
#include <platypus/utility/testing.hpp>
#include "platypus_testing.hpp"

int test_get_column() {
    int fails = 0;
    platypus::DataTable table;
    table.add_column<double>("v1");
    table.add_column<double>("v2");
    std::vector<double> exp1{22./7, 1.29, 1.41, -9.1, -0.03};
    std::vector<double> exp2{-2./7, 521.9, 0.41, -1.1, 4.12};
    for (unsigned int i = 0; i < exp1.size(); ++i) {
        auto & row = table.add_row();
        row << exp1[i] << exp2[i];
    }

    auto by_idx1 = table.get_column<double>(0);
    auto by_idx2 = table.get_column<double>(1);
    fails += platypus::testing::compare_equal(
            exp1,
            by_idx1,
            __FILE__,
            __LINE__,
            "Column 1");
    fails += platypus::testing::compare_equal(
            exp2,
            by_idx2,
            __FILE__,
            __LINE__,
            "Column 2");

    auto by_name1 = table.get_column<double>("v1");
    auto by_name2 = table.get_column<double>("v2");
    fails += platypus::testing::compare_equal(
            exp1,
            by_name1,
            __FILE__,
            __LINE__,
            "Column 1");
    fails += platypus::testing::compare_equal(
            exp2,
            by_name2,
            __FILE__,
            __LINE__,
            "Column 2");

    return fails;
}

int test_summarize() {
    int fails = 0;
    platypus::DataTable table;
    table.add_column<double>("v1");
    // std::vector<double> v1{87.26835, 40.78787, -28.22378, -54.65597, -51.51828, -0.5819756, 14.30884, -4.97701, -52.55527, -56.58851};
    std::vector<double> v1{10.24377,-5.8934286,14.068025,10.345747,19.058397,0.077046906,0.15296858,-2.0715523,6.8968938,-8.8573444};
    for (unsigned int i = 0; i < v1.size(); ++i) {
        auto & row = table.add_row();
        row << v1[i];
    }
    auto o1 = table.summarize_column<double>(0);
    fails += platypus::testing::compare_equal(v1.size()                 , o1.size                , __FILE__ , __LINE__ , "size");
    fails += platypus::testing::compare_almost_equal(44.020522985999996 , o1.sum                 , __FILE__ , __LINE__ , "sum");
    fails += platypus::testing::compare_almost_equal(4.402052298599999  , o1.mean                , __FILE__ , __LINE__ , "mean");
    fails += platypus::testing::compare_almost_equal(82.71037145898468  , o1.sample_variance     , __FILE__ , __LINE__ , "sample variance");
    fails += platypus::testing::compare_almost_equal(74.43933431308622  , o1.population_variance , __FILE__ , __LINE__ , "population variance");
    fails += platypus::testing::compare_almost_equal(-8.8573444         , o1.minimum             , __FILE__ , __LINE__ , "minimum");
    fails += platypus::testing::compare_almost_equal(19.058397          , o1.maximum             , __FILE__ , __LINE__ , "maximum");
    return fails;
}

int main() {
    int fails = 0;
    fails += test_get_column();
    fails += test_summarize();
    if (fails != 0) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}
