#include <catch2/catch_all.hpp>

#include <puara/utils/leakyintegrator.h>

using namespace Catch;

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
