#include "../Data.h"
#include "../Global.h"
#include "../Component.h"
#include "../Configurations/Configuration.h"
#include "../ParameterIos/ParameterIo.h"
#include "../ParameterIos/Memory.h"
#include "../Parameters.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Parameters.h"
#include "../Continuous/Continuous.h"
#include <gtest/gtest.h>

namespace {
   class ParameterIoTest : public ::testing::Test {
      protected:
         ParameterIoTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run");
         }

         virtual ~ParameterIoTest() {
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

   TEST_F(ParameterIoTest, default) {
      ParameterIo* parIo = new ParameterIoMemory(Options("tag=test finder=test.finder"), *mData);
      Continuous* continuous = Continuous::getScheme(Options("tag=test class=ContinuousMoments distribution=test.dist type=full efold=60"), *mData);
      Input* input = Input::getScheme(Options("tag=test class=InputFlat name=test type=forecast"), *mData);
      const std::vector<Location>& locations = input->getLocations();
      ASSERT_EQ(locations.size(), 15);

      Component::Type type = Component::TypeSelector;
      int date = 20110101;
      int init = 0;
      float offset = 0;
      int index = 0;
      std::string variable = "T";
      Configuration configuration("test.raw", *mData);
      Parameters parIn;
      continuous->getDefaultParameters(parIn);
      // Adjust parameters
      for(int i = 0; i < parIn.size(); i++) {
         parIn[i] = i;
      }
      
      const int locationIds[] = {0,1};
      for(int k = 0; k < sizeof(locationIds)/sizeof(int); k++) {
         parIo->add(type, date, init, offset, locations[k], variable, configuration, index, parIn);
         std::vector<float> parInVector = parIn.getAllParameters();
         Location currLocation = locations[locationIds[k]];

         Parameters parOut;
         parIo->read(type, date, init, offset, currLocation, variable, configuration, index, parOut);
         std::vector<float> parOutVector = parOut.getAllParameters();
         ASSERT_EQ(parInVector.size(), parOutVector.size());
         for(int i = 0; i < (int) parInVector.size(); i++) {
            EXPECT_FLOAT_EQ(parInVector[i], parOutVector[i]);
         }
      }

      delete parIo;
      delete continuous;
      delete input;
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
