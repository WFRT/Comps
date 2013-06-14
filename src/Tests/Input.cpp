#include "../Inputs/Input.h"
#include "../Inputs/Flat.h"
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
   class InputTest : public ::testing::Test {
      protected:
         InputTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run");
         }

         virtual ~InputTest() {
            delete mData;
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
         Data* mData;

   };

   TEST_F(InputTest, locations) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      std::vector<Location> locations;
      input->getLocations(locations);
      EXPECT_EQ(locations.size(), 15);
      Location loc0 = locations[0];
      Location loc1 = locations[1];
      EXPECT_FLOAT_EQ(loc0.getLat(), 49.35);
      EXPECT_FLOAT_EQ(loc0.getLon(), -122.77);
      EXPECT_FLOAT_EQ(loc0.getElev(), Global::MV);
      EXPECT_FLOAT_EQ(loc1.getLat(), 49.23);
      EXPECT_FLOAT_EQ(loc1.getLon(), -121.61);
      EXPECT_FLOAT_EQ(loc1.getElev(), 12);

      double trueDist = 8.462350585148891e+04;
      EXPECT_NEAR(loc0.getDistance(loc1), trueDist, mDistanceAccuracy);
      EXPECT_NEAR(loc1.getDistance(loc0), trueDist, mDistanceAccuracy);

      // Surrounding locations
      {
         const int nums[] = {0, 1, 14, 15, 21};
         const int ans[]  = {0, 1, 14, 15, 15};
         for(int i = 0; i < 5; i++) {
            locations.clear();
            input->getSurroundingLocations(loc0, locations, nums[i]);
            EXPECT_EQ(locations.size(), ans[i]);
            if(ans[i] > 0) {
               // Nearest location is itself
               EXPECT_EQ(locations[0].getId(), 0);
               if(ans[i] > 1) {
                  // Second nearest location
                  EXPECT_EQ(locations[1].getId(), 1);
               }
            }
         }
      }

      // Surrounding locations by radius
      {
         const int radii[] = {0, 10000, 40000, 100000, 100000000};
         const int ans[]  = {1, 1,    2, 8, 15};
         for(int i = 0; i < 5; i++) {
            locations.clear();
            input->getSurroundingLocationsByRadius(loc0, locations, radii[i]);
            EXPECT_EQ(locations.size(), ans[i]);
            if(ans[i] > 0) {
               // Nearest location is itself
               EXPECT_EQ(locations[0].getId(), 0);
               // Not sorted!
               /*
               if(ans[i] > 1) {
                  // Second nearest location
                  EXPECT_EQ(locations[1].getId(), 597);
               }
               */
            }
         }

      }
      delete input;
   }

   TEST_F(InputTest, accessors) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      EXPECT_EQ(input->getNumMembers(), 3);
      EXPECT_EQ(input->getNumOffsets(), 8);
      EXPECT_EQ(input->getName(), "test");
      EXPECT_EQ(input->getType(), Input::typeForecast);
      delete input;
   }

   TEST_F(InputTest, offsets) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      // Has offsets
      const int offsetsTrue[]  = {0,1,12, 25, 48, 50, 52, 53};
      const int offsetsFalse[] = {-1, 2, 4, 24, 100};
      for(int i = 0; i < 8; i++) {
         EXPECT_TRUE(input->hasOffset(offsetsTrue[i]));
      }
      for(int i = 0; i < 5; i++) {
         EXPECT_FALSE(input->hasOffset(offsetsFalse[i]));
      }

      // getOffsetIndex
      const float offsets[] = {0, 1, 2, 12, 25, 52, 100};
      const int   indices[] = {0, 1, Global::MV, 2, 3, 6, Global::MV};
      for(int i = 0; i < 7; i++) {
         EXPECT_EQ(input->getOffsetIndex(offsets[i]), indices[i]);
      }

      delete input;
   }

   TEST_F(InputTest, variables) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      const std::string variablesTrue[]  = {"T", "U"};
      const std::string variablesFalse[] = {"W", "", "Q"};
      for(int i = 0; i < 2; i++) {
         EXPECT_TRUE(input->hasVariable(variablesTrue[i]));
      }
      for(int i = 0; i < 3; i++) {
         EXPECT_FALSE(input->hasVariable(variablesFalse[i]));
      }

      delete input;
   }

   TEST_F(InputTest, dates) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      std::vector<int> dates;
      input->getDates(dates);
      EXPECT_EQ(dates.size(), 2);
      const int datesTrue[] = {20110101, 20110102};
      for(int i = 0; i < (int) dates.size(); i++) {
         EXPECT_EQ(dates[i], datesTrue[i]);
      }

      delete input;
   }

   TEST_F(InputTest, members) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      std::vector<Member> members;
      input->getMembers(members);
      EXPECT_EQ(members.size(), 3);
      const int memberIds[] = {0,1,2};
      const std::string dataset = "test";
      const float resolutions[] = {4,12,Global::MV};
      const std::string models[] = {"MC2", "MM5", "WRF3"};

      for(int i = 0; i < 3; i++) {
         EXPECT_EQ(members[i].getId(), memberIds[i]);
         EXPECT_EQ(members[i].getDataset(), dataset);
         EXPECT_FLOAT_EQ(members[i].getResolution(), resolutions[i]);
         EXPECT_EQ(members[i].getModel(), models[i]);
      }

      delete input;
   }


   TEST_F(InputTest, values) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      // 20110101 location0
      {
         const int date = 20110101;
         const int init = 0;
         const float offsets[] = {0, 25, 48, 48, 26, 12, 50};
         const int locationId = 415;
         const int memberIds[] = {0, 0, 0, 1, 0, 2};
         const std::string variable = "T";
         const float ans[] = {-5.81512,-999, -1.137020, -999, -999, -5.838040};
         for(int i = 0; i < 6; i++) {
            EXPECT_EQ(input->getValue(date, init, offsets[i], locationId, memberIds[i], variable), ans[i]);
         }
      }
      delete input;
   }

   TEST_F(InputTest, valuesObs) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=observation"), *mData);
      // 20110101 location0
      {
         const int date = 20110101;
         const int init = 0;
         const float offsets[] = {0, 25, 12, 24, 36, 35};
         const int locationId = 415;
         const int memberIds[] = {0, 0, 2, 2, 1};
         const std::string variable = "T";
         // case[5]: Interpolation not allowed for observations
         const float ans[] = {-5.81512,4.85, -5.838040, 2.3, -3.09723, Global::MV};
         for(int i = 0; i < sizeof(ans)/sizeof(float); i++) {
            EXPECT_EQ(ans[i], input->getValue(date, init, offsets[i], locationId, memberIds[i], variable));
         }
      }
      delete input;
   }

   TEST_F(InputTest, valuesMissingStation) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      // 20110101 location1
      {
         const int date = 20110101;
         const int init = 0;
         const float offsets[] = {0, 25, 48, 48, 26, 12, 50};
         const int locationId = 421;
         const int memberIds[] = {0, 0, 0, 1, 0, 2};
         const std::string variable = "T";
         const float ans = Global::MV;
         for(int i = 0; i < 6; i++) {
            EXPECT_EQ(input->getValue(date, init, offsets[i], locationId, memberIds[i], variable), ans);
         }
      }
      delete input;
   }

   TEST_F(InputTest, timeInterpolation) {
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast allowTimeInterpolation"), *mData);
      // 20110101 location0
      {
         const int date = 20110101;
         const int init = 0;
         const float offsets[] = {0, 25, 48, 48, 11, 26, 51, 12, -1, -1, 100, 100};
         const int locationId = 415;
         const int memberIds[] = {0, 0, 0, 1, 0, 2, 0, 2, 0, 2, 0, 2};
         const std::string variable = "T";
         const float mv = Global::MV;
         // Case[4]: Normal interpolation
         // Case[5]: One side is missing
         // Case[6]: Both sides are missing
         // Case[8]: Below
         // Case[9]: Below with above missing
         // Case[10]: Above with below missing
         // Case[11]: Above
         const float ans[] = {-5.81512, mv, -1.137020, mv, -3.852686363636364, -0.1355, mv, -5.838040, mv, mv, mv, mv};
         for(int i = 0; i < 12; i++) {
            float value = input->getValue(date, init, offsets[i], locationId, memberIds[i], variable);
            EXPECT_EQ(value, ans[i]);
         }
      }
      delete input;
   }
   TEST_F(InputTest, extremeValuesWithQc) {
      // Extreme values should be set to missing
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast"), *mData);
      {
         const int date = 20110101;
         const int init = 0;
         const float offsets[] = {50, 52, 53};
         const int locationId = 415;
         const int memberIds[] = {2,2,2};
         const std::string variable = "T";
         const float ans[] = {Global::MV, Global::MV, -50};
         for(int i = 0; i < 3; i++) {
            EXPECT_EQ(input->getValue(date, init, offsets[i], locationId, memberIds[i], variable), ans[i]);
         }
      }
      delete input;
   }
   TEST_F(InputTest, extremeValuesWithoutQc) {
      // Extreme values should be set to missing
      Input* input = Input::getScheme(Options("tag=test class=InputFlat folder=test type=forecast skipQc"), *mData);
      {
         const int date = 20110101;
         const int init = 0;
         const float offsets[] = {50, 52, 53};
         const int locationId = 415;
         const int memberIds[] = {2,2,2};
         const std::string variable = "T";
         const float ans[] = {12312313, -1030211, -50};
         for(int i = 0; i < 3; i++) {
            EXPECT_EQ(input->getValue(date, init, offsets[i], locationId, memberIds[i], variable), ans[i]);
         }
      }
      delete input;
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
