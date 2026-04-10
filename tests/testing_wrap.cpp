#include <catch2/catch_all.hpp>

#include <puara/utils/wrap.h>
#include <cmath>

using namespace Catch;
using namespace puara_gestures::utils;

TEST_CASE("Wrap keeps values inside the interval and wraps values above and below", "[utils][wrap]")
{
    Wrap w;
    w.min = -M_PI;
    w.max = M_PI;

    REQUIRE(w.wrap(0.0) == Approx(0.0));
    REQUIRE(w.wrap(M_PI) == Approx(-M_PI));
    REQUIRE(w.wrap(-M_PI) == Approx(-M_PI));

    REQUIRE(w.wrap(M_PI + 0.1) == Approx(-M_PI + 0.1));
    REQUIRE(w.wrap(3.0 * M_PI + 0.2) == Approx(-M_PI + 0.2));
    REQUIRE(w.wrap(-M_PI - 0.1) == Approx(M_PI - 0.1));
}

TEST_CASE("Wrap works for a nonzero interval", "[utils][wrap]")
{
    Wrap w;
    w.min = 10.0;
    w.max = 20.0;

    REQUIRE(w.wrap(10.0) == Approx(10.0));
    REQUIRE(w.wrap(15.0) == Approx(15.0));
    REQUIRE(w.wrap(20.0) == Approx(10.0));

    REQUIRE(w.wrap(21.0) == Approx(11.0));
    REQUIRE(w.wrap(29.0) == Approx(19.0));
    REQUIRE(w.wrap(30.0) == Approx(10.0));

    REQUIRE(w.wrap(9.0) == Approx(19.0));
}

TEST_CASE("Unwrap continues increasing through the positive boundary", "[utils][unwrap]")
{
    Unwrap u(-M_PI, M_PI);

    double first = u.unwrap(M_PI - 0.1);
    REQUIRE(first == Approx(M_PI - 0.1));

    double second = u.unwrap(-M_PI + 0.1);
    REQUIRE(second == Approx(M_PI + 0.1));

    double third = u.unwrap(-M_PI + 0.2);
    REQUIRE(third == Approx(M_PI + 0.2));

    double fourth = u.unwrap(-M_PI + 0.3);
    REQUIRE(fourth == Approx(M_PI + 0.3));
}

TEST_CASE("Unwrap continues decreasing through the negative boundary and resets cleanly", "[utils][unwrap]")
{
    Unwrap u(-M_PI, M_PI);

    double first = u.unwrap(-M_PI + 0.1);
    REQUIRE(first == Approx(-M_PI + 0.1));

    double second = u.unwrap(M_PI - 0.1);
    REQUIRE(second == Approx(-M_PI - 0.1));

    double third = u.unwrap(M_PI - 0.2);
    REQUIRE(third == Approx(-M_PI - 0.2));

    u.clear();
    double restart = u.unwrap(1.0);
    REQUIRE(restart == Approx(1.0));
}
