#include <catch2/catch_all.hpp>

#include <puara/utils/blobDetector.h>

using namespace Catch;

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
