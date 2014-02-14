#include "../Inputs/Input.h"
#include "../Inputs/BchNaefs.h"
#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

//! Tests specific to naefs input data. Does not need to test Input in general.
namespace {
   class NaefsTest : public ::testing::Test {
      protected:
         NaefsTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run");
            mInput = Input::getScheme(Options("tag=test class=InputBchNaefs folder=bchNaefs type=forecast"), *mData);
         }

         virtual ~NaefsTest() {
            delete mInput;
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
         Input* mInput;
         Data* mData;

   };

   TEST_F(NaefsTest, locations) {
      const std::vector<Location>& locations = mInput->getLocations();
      ASSERT_EQ(locations.size(), 83);
      Location loc0 = locations[0];
      Location loc1 = locations[20];
      EXPECT_FLOAT_EQ(loc0.getLat(), 49.2872);
      EXPECT_FLOAT_EQ(loc0.getLon(), -122.4844);
      EXPECT_FLOAT_EQ(loc0.getElev(), Global::MV);
      EXPECT_FLOAT_EQ(loc1.getLat(), 49.9769);
      EXPECT_FLOAT_EQ(loc1.getLon(), -125.5847);
      EXPECT_FLOAT_EQ(loc1.getElev(), Global::MV);
   }

   TEST_F(NaefsTest, accessors) {
      EXPECT_EQ(mInput->getNumMembers(), 45);
      EXPECT_EQ(mInput->getNumOffsets(), 16);
      EXPECT_EQ(mInput->getName(), "bchNaefs");
      EXPECT_EQ(mInput->getType(), Input::typeForecast);
   }

   TEST_F(NaefsTest, offsets) {
      // Has offsets
      for(int i = 0; i < 400; i++) {
         if(i % 24 == 0 && i <= 15*24) {
            EXPECT_TRUE(mInput->hasOffset(i));
         }
         else {
            EXPECT_TRUE(!mInput->hasOffset(i));
         }
      }

      for(int i = 0; i < 400; i++) {
         if(i % 24 == 0 && i <= 15*24) {
            EXPECT_EQ(mInput->getOffsetIndex(i), (int) i/24);
         }
         else {
            EXPECT_FLOAT_EQ(mInput->getOffsetIndex(i), Global::MV);
         }
      }
   }

   TEST_F(NaefsTest, variables) {
      const std::string variablesTrue[]  = {"TMin24", "TMax24", "Precip24"};
      const std::string variablesFalse[] = {"tMin24", "", "Q"};
      for(int i = 0; i < 2; i++) {
         EXPECT_TRUE(mInput->hasVariable(variablesTrue[i]));
      }
      for(int i = 0; i < 3; i++) {
         EXPECT_FALSE(mInput->hasVariable(variablesFalse[i]));
      }

   }

   TEST_F(NaefsTest, members) {
      /*
      std::vector<Member> members;
      mInput->getMembers(members);
      EXPECT_EQ(members.size(), 45);
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
      */
   }

   TEST_F(NaefsTest, values) {
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
            EXPECT_EQ(mInput->getValue(date, init, offsets[i], locationId, memberIds[i], variable), ans[i]);
         }
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
