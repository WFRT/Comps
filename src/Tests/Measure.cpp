#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Measures/Measure.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

namespace {
   class MeasureTest : public ::testing::Test {
      protected:
         MeasureTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run");
            mMeasure    = Measure::getScheme(Options("tag=test class=MeasureEnsembleMoment moment=2 preTransforms=test.abs,test.log10 postTransforms=test.square,test.square"), *mData);
         }
         virtual ~MeasureTest() {
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
         Measure* mMeasure;
   };
   TEST_F(MeasureTest, moment) {
      std::vector<float> ensValues;
      ensValues.push_back(-100);
      ensValues.push_back(10);
      ensValues.push_back(-0.1);
      ensValues.push_back(Global::MV);
      ensValues.push_back(-10);
      float expected = 1.9885406212109378;

      Ensemble ens(ensValues, "T");

      float value = mMeasure->measure(ens);
      EXPECT_FLOAT_EQ(expected, value);
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
