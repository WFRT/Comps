#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Ensemble.h"
#include "../Inputs/Input.h"
#include "../Location.h"
#include "../Neighbourhoods/Neighbourhood.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

namespace {
   class NeighbourhoodTest : public ::testing::Test {
      protected:
         NeighbourhoodTest() {
            // You can do set-up work for each test here.
            Options opt;
            opt.addOption("inputs", "test.grid16");
            mData = new Data(opt);
            mInput = Input::getScheme(Options("tag=grid16 class=InputGrid folder=testGrid type=forecast lats=0:3 lons=0:3 offsets=0"));
         }
         Neighbourhood* getNeighbourhood(int num) {
            std::stringstream ss;
            ss << "tag=test class=NeighbourhoodNearest num=" << num;
            Neighbourhood* hood = Neighbourhood::getScheme(Options(ss.str()));
            mHoods.push_back(hood);
            return hood;
         }
         virtual ~NeighbourhoodTest() {
            // You can do clean-up work that doesn't throw exceptions here.
            for(int i = 0; i < mHoods.size(); i++) {
               delete mHoods[i];
            }
            delete mData;
         }
         virtual void SetUp() {
            // Code here will be called immediately after the constructor (right
            // before each test).
         }

         virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
         }
         Data* mData;
         Input* mInput;
         std::vector<Neighbourhood*> mHoods;
   };
   TEST_F(NeighbourhoodTest, nearestNeighbour) {
      std::vector<Location> allLocations = mInput->getLocations();
      ASSERT_TRUE(allLocations.size()>0);
      Neighbourhood* hood = getNeighbourhood(1);

      for(int i = 0; i < allLocations.size(); i++) {
         Location location = allLocations[i];
         std::vector<Location> locations = hood->select(mInput, location);
         ASSERT_EQ(locations.size(), 1);
         EXPECT_EQ(locations[0].getId(), location.getId());
      }
   }
   TEST_F(NeighbourhoodTest, nearest4Neighbours) {
      std::vector<Location> allLocations = mInput->getLocations();
      ASSERT_TRUE(allLocations.size()>0);
      Location location = allLocations[0];

      Neighbourhood* hood = getNeighbourhood(4);
      std::vector<Location> locations = hood->select(mInput, location);
      int expected[] = {0,1,4,5};
      int size = sizeof(expected)/sizeof(int);
      ASSERT_EQ(locations.size(), size);

      for(int i = 0; i < size; i++) {
         EXPECT_EQ(expected[i], locations[i].getId());
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
