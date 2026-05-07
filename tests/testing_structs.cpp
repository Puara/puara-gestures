#include <catch2/catch_all.hpp>

#include <puara/structs.h>

using namespace Catch;

TEST_CASE("Spherical alias fields work as expected", "[structs]")
{
    puara_gestures::Spherical s{};

    s.azimuth = 1.1;
    s.elevation = 2.2;
    s.distance = 3.3;

    REQUIRE(s.yaw == Approx(1.1));
    REQUIRE(s.pitch == Approx(2.2));
    REQUIRE(s.r == Approx(3.3));

    s.yaw = -0.5;
    s.pitch = -1.25;
    s.r = 9.75;

    REQUIRE(s.azimuth == Approx(-0.5));
    REQUIRE(s.elevation == Approx(-1.25));
    REQUIRE(s.distance == Approx(9.75));
}

TEST_CASE("DiscreteArray and MinMax basic behavior", "[structs]")
{
    puara_gestures::DiscreteArray da(4);
    REQUIRE(da.arr.size() == 4);

    for (int i = 0; i < 4; i++) {
        REQUIRE(da.arr[i] == 0);
    }

    da.arr[1] = 42;
    REQUIRE(da.arr[1] == 42);

    puara_gestures::MinMax<int> mm{.min = -10, .max = 10};
    REQUIRE(mm.min == -10);
    REQUIRE(mm.max == 10);
}

TEST_CASE("Coord1D/Coord2D/Coord3D values are assignable", "[structs]")
{
    puara_gestures::Coord1D c1{5.5};
    puara_gestures::Coord2D c2{1.2, 2.3};
    puara_gestures::Coord3D c3{9.8, 7.6, 5.4};

    REQUIRE(c1.x == Approx(5.5));
    REQUIRE(c2.x == Approx(1.2));
    REQUIRE(c2.y == Approx(2.3));
    REQUIRE(c3.x == Approx(9.8));
    REQUIRE(c3.y == Approx(7.6));
    REQUIRE(c3.z == Approx(5.4));
}
