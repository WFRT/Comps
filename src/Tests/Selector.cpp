#include "../Data.h"
#include "../Global.h"
#include "../Selectors/Selector.h"
#include "../Member.h"
#include "../Slice.h"
#include "../Location.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

namespace {
   class SelectorTest : public ::testing::Test {
      protected:
         SelectorTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run");
         }

         virtual ~SelectorTest() {
            // You can do clean-up work that doesn't throw exceptions here.
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
   };

   TEST_F(SelectorTest, default) {
      Selector* selector = Selector::getScheme(Options("tag=test class=SelectorDefault"), *mData);
      Input* input = Input::getScheme(Options("tag=test class=InputFlat name=test type=forecast"), *mData);
      std::vector<Location> locations;
      input->getLocations(locations);
      ASSERT_EQ(locations.size(), 15);

      // Add fake station
      Location fakeLocation("test", 123111);
      locations.push_back(fakeLocation);

      const int allDates[] = {20110101, 20120101, 10, 12};
      const float allOffsets[] = {0, 1, 12, 25, 48, 50, 52, 53, 54};
      const int allLocationIds[] = {0,1,3,12,14,15};
      for(int d = 0; d < sizeof(allDates)/sizeof(int); d++) {
         for(int o = 0; o < sizeof(allOffsets)/sizeof(float); o++) {
            for(int l = 0; l < sizeof(allLocationIds)/sizeof(int); l++) {
               int date = allDates[d];
               int init = 0;
               float offset = allOffsets[o];
               Location location = locations[l];
               std::string variable = "T";
               Parameters par;
               selector->getDefaultParameters(par);
               std::vector<Slice> slices;

               selector->select(date, init, offset, location, variable, par, slices);
               ASSERT_EQ(slices.size(), 3);
               const int dates[] = {date, date, date};
               const int inits[] = {init, init, init};
               const float offsets[] = {offset, offset, offset};
               const int memberIds[] = {0, 1, 2};
               const int skills[]     = {Global::MV, Global::MV, Global::MV};
               for(int i = 0; i < (int) slices.size(); i++) {
                  EXPECT_EQ(slices[i].getDate(), dates[i]);
                  EXPECT_EQ(slices[i].getInit(), inits[i]);
                  EXPECT_EQ(slices[i].getOffset(), offsets[i]);
                  EXPECT_EQ(slices[i].getMember().getId(), memberIds[i]);
                  EXPECT_EQ(slices[i].getSkill(), skills[i]);
               }
            }
         }
      }
      delete selector;
      delete input;
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
