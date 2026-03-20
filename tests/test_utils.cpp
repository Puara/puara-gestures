#include <catch2/catch_all.hpp>

#include <puara/utils.h>
#include <thread>

using namespace Catch;
using namespace puara_gestures::utils;

TEST_CASE("arrayAverage works for ranges", "[utils]")
{
    const int data[] = {1, 2, 3, 4, 5};

    REQUIRE(arrayAverage(data, 0, 5) == Approx(3.0f));
    REQUIRE(arrayAverage(data, 1, 4) == Approx(3.0f));
    REQUIRE(arrayAverage(data, 2, 3) == Approx(3.0f));
    REQUIRE(arrayAverage(data, 0, 0) == Approx(0.0f));
    REQUIRE(arrayAverage(data, 3, 3) == Approx(0.0f));
}

TEST_CASE("arrayAverageZero ignores zeros", "[utils]")
{
    double data1[] = {0.0, 0.0, 0.0};
    REQUIRE(arrayAverageZero(data1, 3) == Approx(0.0));

    double data2[] = {1.0, 0.0, 3.0, 0.0, 5.0};
    REQUIRE(arrayAverageZero(data2, 5) == Approx((1.0 + 3.0 + 5.0) / 3.0));

    double data3[] = {2.0, 4.0, 6.0};
    REQUIRE(arrayAverageZero(data3, 3) == Approx((2.0 + 4.0 + 6.0) / 3.0));
}

TEST_CASE("getCurrentTimeMicroseconds is monotonic", "[utils]")
{
    auto t0 = getCurrentTimeMicroseconds();
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    auto t1 = getCurrentTimeMicroseconds();

    REQUIRE(t1 >= t0);
    REQUIRE(t1 - t0 >= 0);
}
