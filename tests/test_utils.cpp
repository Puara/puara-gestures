#include <catch2/catch_all.hpp>
#include <cmath>
#include <puara/utils.h>
#include <thread>

using namespace Catch;
using namespace puara_gestures::utils;

// utils.h

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
TEST_CASE("unit conversion helpers", "[utils]")
{
    using namespace puara_gestures::utils::convert;

    REQUIRE(g_to_ms2(1.0) == Approx(9.80665));
    REQUIRE(ms2_to_g(9.80665) == Approx(1.0));

    REQUIRE(dps_to_rads(180.0) == Approx(M_PI));
    REQUIRE(rads_to_dps(M_PI) == Approx(180.0));

    REQUIRE(gauss_to_tesla(10000.0) == Approx(1.0));
    REQUIRE(tesla_to_gauss(1.0) == Approx(10000.0));
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

TEST_CASE("phased spherical/cartesian round trip", "[utils]")
{
    using namespace puara_gestures::utils::convert;

    puara_gestures::Spherical p0{};
    p0.azimuth = M_PI / 2.0;
    p0.elevation = M_PI / 6.0;
    p0.distance = 2.0;

    auto c0 = phased_spheric_to_cartesian(p0);
    REQUIRE(c0.x == Approx(2.0 * cos(M_PI / 6.0) * cos(M_PI / 2.0)));
    REQUIRE(c0.y == Approx(2.0 * cos(M_PI / 6.0) * sin(M_PI / 2.0)));
    REQUIRE(c0.z == Approx(2.0 * sin(M_PI / 6.0)));

    auto p0rt = phased_cartesian_to_spheric(c0);
    REQUIRE(p0rt.r == Approx(p0.distance));
    REQUIRE(p0rt.elevation == Approx(p0.elevation));
    REQUIRE(p0rt.azimuth == Approx(p0.azimuth));

    // check primary x-axis convention: elevation=0 and azimuth=0 should map to (r,0,0)
    puara_gestures::Spherical p1{};
    p1.azimuth = 0.0;
    p1.elevation = 0.0;
    p1.distance = 1.0;
    auto c1 = phased_spheric_to_cartesian(p1);
    REQUIRE(c1.x == Approx(1.0));
    REQUIRE(c1.y == Approx(0.0).margin(1e-15));
    REQUIRE(c1.z == Approx(0.0).margin(1e-15));

    auto p1rt = phased_cartesian_to_spheric(c1);
    REQUIRE(p1rt.azimuth == Approx(0.0).margin(1e-15));
    REQUIRE(p1rt.elevation == Approx(0.0).margin(1e-15));
}

//blobDetector.h
TEST_CASE("BlobDetector handles empty array", "[blobDetector]")
{
    puara_gestures::BlobDetector<4> detector;
    int data[] = {0, 0, 0, 0};

    detector.detect1D(data, 4);

    REQUIRE(detector.blobCount == 0);
}

TEST_CASE("BlobDetector finds a single blob", "[blobDetector]")
{
    puara_gestures::BlobDetector<4> detector;
    int data[] = {0, 1, 1, 1, 0};

    detector.detect1D(data, 5);

    REQUIRE(detector.blobCount == 1);
    REQUIRE(detector.blobStartPos[0] == 1);
    REQUIRE(detector.blobSize[0] == 3);
    REQUIRE(detector.blobCenter[0] == Approx(2.0));
}

TEST_CASE("BlobDetector finds multiple blobs and stops at maxNumBlobs", "[blobDetector]")
{
    puara_gestures::BlobDetector<2> detector;
    int data[] = {1, 1, 0, 1, 0, 1, 1};

    detector.detect1D(data, 7);

    REQUIRE(detector.blobCount == 2);
    REQUIRE(detector.blobStartPos[0] == 0);
    REQUIRE(detector.blobSize[0] == 2);
    REQUIRE(detector.blobCenter[0] == Approx(0.5));

    REQUIRE(detector.blobStartPos[1] == 3);
    REQUIRE(detector.blobSize[1] == 1);
    REQUIRE(detector.blobCenter[1] == Approx(3.0));
}

TEST_CASE("BlobDetector tracks prevBlobStartPos", "[blobDetector]")
{
    puara_gestures::BlobDetector<3> detector;
    int data1[] = {0, 1, 1, 0, 1};
    int data2[] = {1, 1, 0, 0, 1};

    detector.detect1D(data1, 5);
    REQUIRE(detector.blobCount == 2);

    detector.detect1D(data2, 5);
    REQUIRE(detector.blobCount == 2);
    REQUIRE(detector.prevBlobStartPos[0] == 1);
    REQUIRE(detector.prevBlobStartPos[1] == 4);
}

TEST_CASE("BlobDetector handles full array blob", "[blobDetector]")
{
    puara_gestures::BlobDetector<4> detector;
    int data[] = {1, 1, 1, 1};

    detector.detect1D(data, 4);

    REQUIRE(detector.blobCount == 1);
    REQUIRE(detector.blobStartPos[0] == 0);
    REQUIRE(detector.blobSize[0] == 4);
    REQUIRE(detector.blobCenter[0] == Approx(1.5));
}
// calibration.h not is this file

// chrono.h
TEST_CASE("getCurrentTimeMicroseconds is monotonic", "[utils]")
{
    auto t0 = getCurrentTimeMicroseconds();
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    auto t1 = getCurrentTimeMicroseconds();

    REQUIRE(t1 >= t0);
    REQUIRE(t1 - t0 >= 0);
}

// circularbuffer.h
TEST_CASE("CircularBuffer retains most recent values", "[utils]")
{
    puara_gestures::utils::CircularBuffer buf;
    buf.size = 3;

    buf.add(1.0);
    buf.add(2.0);
    buf.add(3.0);
    buf.add(4.0);

    REQUIRE(buf.buffer.size() == 3);
    REQUIRE(buf.buffer[0] == Approx(4.0));
    REQUIRE(buf.buffer[1] == Approx(3.0));
    REQUIRE(buf.buffer[2] == Approx(2.0));
}

// discretizer.h
TEST_CASE("Discretizer detects changes in data flow", "[utils]")
{
    puara_gestures::utils::Discretizer detector;
    detector.isNew(0.0);

    REQUIRE(detector.isNew(0.0) == false);
    REQUIRE(detector.isNew(0.0) == false);
    REQUIRE(detector.isNew(1.0) == true);
    REQUIRE(detector.isNew(1.0) == false);
    REQUIRE(detector.isNew(0.0) == true);
    REQUIRE(detector.isNew(1.0) == true);

}

// leakyintegrator.h
TEST_CASE("LeakyIntegrator basic leak behavior with freq = 0", "[utils]")
{
    puara_gestures::utils::LeakyIntegrator integrator(0.0, 0.0, 0.5, 0, 0);

    REQUIRE(integrator.current_value == Approx(0.0));
    REQUIRE(integrator.old_value == Approx(0.0));

    double v1 = integrator.integrate(1.0);
    REQUIRE(v1 == Approx(1.0));
    REQUIRE(integrator.current_value == Approx(1.0));
    REQUIRE(integrator.old_value == Approx(1.0));

    double v2 = integrator.integrate(1.0);
    REQUIRE(v2 == Approx(1.5));
    REQUIRE(integrator.current_value == Approx(1.5));
    REQUIRE(integrator.old_value == Approx(1.5));

    double v3 = integrator.integrate(2.0);
    REQUIRE(v3 == Approx(2.75)); // 2.0 + 1.5*0.5
}

TEST_CASE("LeakyIntegrator cutoff branch when timer pushes update", "[utils]")
{
    puara_gestures::utils::LeakyIntegrator integrator;
    integrator.leak = 0.5;
    integrator.old_value = 0.0;

    // set a far future timer to force the 'reading + old_value' branch for freq > 0
    unsigned long long fakeTimer = ULLONG_MAX - 1000;
    double v1 = integrator.integrate(1.0, 0.0, 0.5, 100, fakeTimer);
    REQUIRE(v1 == Approx(1.0));
    REQUIRE(integrator.old_value == Approx(1.0));

    double v2 = integrator.integrate(1.0, 0.0, 0.5, 100, fakeTimer);
    REQUIRE(v2 == Approx(2.0));
}

// maprange.h
TEST_CASE("MapRange maps and preserves values when outMin == outMax", "[utils]")
{
    puara_gestures::utils::MapRange mapper;
    mapper.inMin = 0;
    mapper.inMax = 10;
    mapper.outMin = 0;
    mapper.outMax = 100;

    REQUIRE(mapper.range(5) == Approx(50.0));
    REQUIRE(mapper.range(0) == Approx(0.0));
    REQUIRE(mapper.range(10) == Approx(100.0));

    mapper.outMin = 2;
    mapper.outMax = 2;
    REQUIRE(mapper.range(5) == Approx(2.0));
}

// rollingminmax.h
TEST_CASE("RollingMinMax tracks the min and max of a sliding window", "[utils]")
{
    puara_gestures::utils::RollingMinMax<int> window(3);

    auto range1 = window.update(10);
    REQUIRE(range1.min == Approx(10));
    REQUIRE(range1.max == Approx(10));

    auto range2 = window.update(4);
    REQUIRE(range2.min == Approx(4));
    REQUIRE(range2.max == Approx(10));

    auto range3 = window.update(7);
    REQUIRE(range3.min == Approx(4));
    REQUIRE(range3.max == Approx(10));

    auto range4 = window.update(2);
    REQUIRE(range4.min == Approx(2));
    REQUIRE(range4.max == Approx(7));

    auto range5 = window.update(12);
    REQUIRE(range5.min == Approx(2));
    REQUIRE(range5.max == Approx(12));
}



//threshold.h
TEST_CASE("Threshold clamps values to the specified range", "[utils]")
{
    puara_gestures::utils::Threshold thresh{-1.0, 1.0};
    double safe = thresh.update(1.5);
    double raw  = thresh.current;

    REQUIRE(safe == Approx(1.0));
    REQUIRE(raw == Approx(1.5));

    REQUIRE(thresh.update(-2.0) == Approx(-1.0));
    REQUIRE(thresh.update(0.5) == Approx(0.5));
    REQUIRE(thresh.update(-0.5) == Approx(-0.5));
}



// wrap.h
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