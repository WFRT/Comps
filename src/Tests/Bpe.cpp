#include "../Global.h"
#include "../Continuous/Continuous.h"
#include <gtest/gtest.h>

namespace {
   class BpeTest : public ::testing::Test {
      protected:
         BpeTest() {
            // You can do set-up work for each test here.
            mData = new Data();

            mContStep = Continuous::getScheme(Options("tag=test class=ContinuousBpe"), *mData);
            std::vector<float> values;
            values.push_back(0);
            values.push_back(1);
            values.push_back(1);
            values.push_back(2);
            values.push_back(3);
            mEns = Ensemble(values, "T");
            int date = 20130101;
            int init = 0;
            float offset = 0;
            Location location("test", 0, 49, -122);
            location.setElev(0);
            mEns.setInfo(date, init, offset, location, "T");
            std::vector<float> values1(1,0);
            mEns1 = Ensemble(values1, "T");
         }

         virtual ~BpeTest() {
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
         Data* mData;
         InputContainer* mInputContainer;
         Continuous *mContStep;
         Ensemble mEns;
         Ensemble mEns1; // Ensemble with one member
         void testCdf(float xs[], float exp[], Ensemble& ens) {
            for(int i = 0; i < sizeof(xs)/sizeof(float); i++) {
               float x = xs[i];
               float cdf = mContStep->getCdf(x, ens, Parameters());
               EXPECT_FLOAT_EQ(exp[i], cdf);
            }
         };
         void testInv(float cdfs[], float exp[], Ensemble& ens) {
            for(int i = 0; i < sizeof(cdfs)/sizeof(float); i++) {
               float cdf = cdfs[i];
               float x = mContStep->getInv(cdf, ens, Parameters());
               EXPECT_FLOAT_EQ(exp[i], x);
            }
         }

   };

   TEST_F(BpeTest, getCdf) {
      float xs[]  = {-1, 0  , 0.01, 1  , 2  , 2.5, 3, 4};
      float exp[] = { 0, 0.2, 0.2 , 0.5, 0.8, 0.8, 1, 1};
      float exp1[]= { 0, 0.5, 1   , 1  , 1  , 1  , 1, 1};

      testCdf(xs, exp, mEns);
      testCdf(xs, exp1, mEns1);
   }
   TEST_F(BpeTest, getInv) {
      float cdfs[] = {0, 0.1, 0.2, 0.25, 0.5, 1};
      float exp[]  = {0, 0  , 0  , 0   , 1  , 3};
      float exp1[] = {0, 0  , 0  , 0   , 0  , 0};

      testInv(cdfs, exp, mEns);
      testInv(cdfs, exp1, mEns1);
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
