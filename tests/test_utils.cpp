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
    REQUIRE(arrayAverageWithoutZero(data1, 3) == Approx(0.0));

    double data2[] = {1.0, 0.0, 3.0, 0.0, 5.0};
    REQUIRE(arrayAverageWithoutZero(data2, 5) == Approx((1.0 + 3.0 + 5.0) / 3.0));

    double data3[] = {2.0, 4.0, 6.0};
    REQUIRE(arrayAverageWithoutZero(data3, 3) == Approx((2.0 + 4.0 + 6.0) / 3.0));
}

TEST_CASE("getCurrentTimeMicroseconds is monotonic", "[utils]")
{
    auto t0 = getCurrentTimeMicroseconds();
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    auto t1 = getCurrentTimeMicroseconds();

    REQUIRE(t1 >= t0);
    REQUIRE(t1 - t0 >= 0);
}

TEST_CASE("polar_to_cartesian and cartesian_to_polar round trip", "[utils]")
{
    using namespace puara_gestures::utils::convert;

    puara_gestures::Spherical p0{};
    p0.azimuth = M_PI/2.0;
    p0.elevation = M_PI/2.0;
    p0.distance = 1.0;
    auto c0 = spheric_to_cartesian(p0);
    REQUIRE(c0.x == Approx(0.0).margin(1e-15));
    REQUIRE(c0.y == Approx(1.0));
    REQUIRE(c0.z == Approx(0.0).margin(1e-15));

    puara_gestures::Spherical p1{};
    p1.azimuth = M_PI;
    p1.elevation = M_PI;
    p1.distance = 1.0;
    auto c1 = spheric_to_cartesian(p1);
    REQUIRE(c1.x == Approx(0.0).margin(1e-15));
    REQUIRE(c1.y == Approx(0.0).margin(1e-15));
    REQUIRE(c1.z == Approx(-1.0));

    puara_gestures::Spherical p2{};
    p2.azimuth = M_PI;
    p2.elevation = M_PI/4;
    p2.distance = 1.0;
    auto c2 = spheric_to_cartesian(p2);
    REQUIRE(c2.x == Approx(-0.7071));
    REQUIRE(c2.y == Approx(0.0).margin(1e-15));
    REQUIRE(c2.z == Approx(0.7071));

    puara_gestures::Coord3D c3{0.0, 1.0, 0.0};
    auto p3 = cartesian_to_spheric(c3);
    REQUIRE(p3.r == Approx(1.0));
    REQUIRE(p3.elevation == Approx(M_PI/2.0));
    REQUIRE(p3.azimuth == Approx(M_PI/2.0));

    puara_gestures::Coord3D c4{1.0, 1.0, 1.0};
    auto p4 = cartesian_to_spheric(c4);
    REQUIRE(p4.r == Approx(sqrt(3.0)));
    REQUIRE(p4.elevation == Approx(atan(sqrt(2.0))));
    REQUIRE(p4.azimuth == Approx(M_PI/4.0));

    // round-trip conversion
    puara_gestures::Spherical p5{};
    p5.azimuth = 2.0;
    p5.elevation = M_PI/6.0;
    p5.distance = M_PI/4.0;
    auto c5 = spheric_to_cartesian(p5);
    auto p5rt = cartesian_to_spheric(c5);
    REQUIRE(p5rt.r == Approx(p5.r));
    REQUIRE(p5rt.elevation == Approx(p5.elevation));
    REQUIRE(p5rt.azimuth == Approx(p5.azimuth));
}

TEST_CASE("unit conversion helpers", "[utils]")
{
    using namespace puara_gestures::utils::convert;

    REQUIRE(g_to_ms2(1.0) == Approx(9.80665));
    REQUIRE(ms2_to_g(9.80665) == Approx(1.0));

    REQUIRE(dps_to_rads(180.0) == Approx(M_PI));
    REQUIRE(rads_to_dps(M_PI) == Approx(180.0));

    REQUIRE(gauss_to_utesla(10000.0) == Approx(1.0));
    REQUIRE(utesla_to_gauss(1.0) == Approx(10000.0));
}

TEST_CASE("bitShiftArrayL works as shift register with carry from next cell", "[utils]")
{
    int orig[] = {1, 1, 0, 0};
    int shifted[4] = {0};

    bitShiftArrayL(orig, shifted, 4, 1);

    REQUIRE(shifted[0] == ((orig[0] << 1) | (orig[1] >> 7)));
    REQUIRE(shifted[1] == ((orig[1] << 1) | (orig[2] >> 7)));
    REQUIRE(shifted[2] == ((orig[2] << 1) | (orig[3] >> 7)));
    REQUIRE(shifted[3] == (orig[3] << 1));

    int shifted2[4] = {0};
    bitShiftArrayL(orig, shifted2, 4, 2);

    int oneShift[4] = {0};
    bitShiftArrayL(orig, oneShift, 4, 1);

    int expected2[4];
    for (int i = 0; i < 4; ++i) {
        if (i == 3) {
            expected2[i] = (oneShift[i] << 1);
        } else {
            expected2[i] = (oneShift[i] << 1) | (oneShift[i + 1] >> 7);
        }
    }

    for (int i = 0; i < 4; ++i) {
        REQUIRE(shifted2[i] == expected2[i]);
    }
}
