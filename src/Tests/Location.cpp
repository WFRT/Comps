#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Slice.h"
#include "../Location.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

namespace {
   class LocationTest : public ::testing::Test {
      protected:
         LocationTest() {
            // You can do set-up work for each test here.
         }

         virtual ~LocationTest() {
            // You can do clean-up work that doesn't throw exceptions here.
         }
         virtual void SetUp() {
            // Code here will be called immediately after the constructor (right
            // before each test).
         }

         virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
         }
         static const float mDistanceAccuracy = 1;

   };

   TEST_F(LocationTest, distance) {
      // getDistance
      const float lat1[] = {0,  49, 49,12.4, 80, 70};
      const float lon1[] = {0,-123,  0,-172.1, -180, -180};
      const float lat2[] = {0,  49, 30,-72.1, 80, 70};
      const float lon2[] = {0,-123,  0,104, 179, 180};
      const double ans[] = {0,   0,2115070.3,11124137.27, 1.9330188e+04,0};
      for(int i = 0; i < 6; i++) {
         EXPECT_NEAR(Location::getDistance(lat1[i], lon1[i], lat2[i], lon2[i]), ans[i], mDistanceAccuracy);
         Location loc11("none", 11, lat1[i], lon1[i], 0);
         Location loc12("none", 12, lat2[i], lon2[i], 0);
         EXPECT_NEAR(loc11.getDistance(loc12), ans[i], mDistanceAccuracy);
         EXPECT_NEAR(loc12.getDistance(loc11), ans[i], mDistanceAccuracy);
      }

      // Real data
      /*
      std::string dataset = "test";
      Location loc0(dataset, 0);
      Location loc1(dataset, 1);
      EXPECT_FLOAT_EQ(loc0.getLat(), 49.35);
      EXPECT_FLOAT_EQ(loc0.getLon(), -122.77);
      EXPECT_FLOAT_EQ(loc0.getElev(), Global::MV);
      EXPECT_FLOAT_EQ(loc1.getLat(), 49.2319);
      EXPECT_FLOAT_EQ(loc1.getLon(), -121.6186);
      EXPECT_FLOAT_EQ(loc1.getElev(), 12);

      double trueDist = 8.462350585148891e+04;
      EXPECT_NEAR(loc0.getDistance(loc1), trueDist, mDistanceAccuracy);
      EXPECT_NEAR(loc1.getDistance(loc0), trueDist, mDistanceAccuracy);
      */
   }
   TEST_F(LocationTest, degRad) {
      // deg2rad and rad2deg
      const float pi = Global::pi;
      const float deg[] = {-720, -360, -270, -180, -90, -30, 0, 30, 45, 180, 360, 720};
      const float rad[] = {-4*pi, -2*pi, -pi*1.5, -pi, -pi/2, -pi/6, 0, pi/6, pi/4, pi, 2*pi, 4*pi};
      for(int i = 0; i < 12; i++) {
         EXPECT_FLOAT_EQ(rad[i], Location::deg2rad(deg[i]));
         EXPECT_FLOAT_EQ(deg[i], Location::rad2deg(rad[i]));
      }
   }
   TEST_F(LocationTest, accessors) {
      // Empty
      {
         Location loc;
         EXPECT_FLOAT_EQ(loc.getLat(), Global::MV);
         EXPECT_FLOAT_EQ(loc.getLon(), Global::MV);
         EXPECT_FLOAT_EQ(loc.getElev(), Global::MV);
         EXPECT_EQ(loc.getId(), 0);
         EXPECT_EQ(loc.getDataset(), "");

         Location loc2("none", 2, 49,-123,12);
         EXPECT_FLOAT_EQ(loc.getDistance(loc2), Global::MV);

         EXPECT_FLOAT_EQ(loc.getDistance(loc2), Global::MV);
      }
      // Normal
      {
         std::string dataset = "test";
         Location loc(dataset, 2);
         EXPECT_EQ(loc.getDataset(), dataset);
         EXPECT_EQ(loc.getId(), 2);
         EXPECT_FLOAT_EQ(loc.getLat(), Global::MV);
         EXPECT_FLOAT_EQ(loc.getLon(), Global::MV);
         EXPECT_FLOAT_EQ(loc.getElev(), Global::MV);
      }
   }
   TEST_F(LocationTest, mutators) {
      const float lat[] = {0,  49, 49,12.4, 80, 70};
      const float lon[] = {0,-123,  0,-172.1, -180, -180};
      const float elev[]= {0,1,2,3,4,5};
      const double ans[] = {};
      for(int i = 0; i < 6; i++) {
         Location loc("none", 0, lat[i], lon[i], elev[i]);
         EXPECT_FLOAT_EQ(loc.getLat(), lat[i]);
         EXPECT_FLOAT_EQ(loc.getLon(), lon[i]);
         EXPECT_FLOAT_EQ(loc.getElev(),elev[i]);
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
