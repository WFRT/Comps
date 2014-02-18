#define BOOST_TEST_MODULE Corrector
#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Parameters.h"
#include "../Ensemble.h"
#include "../Correctors/Corrector.h"
#include <gtest/gtest.h>

namespace {
   class CorrectorTest : public ::testing::Test {
      protected:
         CorrectorTest() {
            // You can do set-up work for each test here.
            mData = new Data;

            std::vector<float> values;
            values.push_back(1);
            values.push_back(2);
            mEns = Ensemble(values, "T");
            int date = 20120101;
            float offset = 0;
            Location location("test.input", 0, 49, -123);
            mEns.setInfo(date, 0, offset, location, "T");

            mCorrector = Corrector::getScheme(Options("tag=test class=CorrectorFixed value=2"), *mData);
         }
         virtual ~CorrectorTest() {
            delete mData;
            delete mCorrector;
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
         Ensemble mEnsSingle;
         Ensemble mEns;
         Corrector* mCorrector;

   };

   TEST_F(CorrectorTest, test1) {

      Parameters par;
      mCorrector->getDefaultParameters(par);
      Ensemble ens = mEns;
      mCorrector->correct(par, ens);

      EXPECT_EQ(ens.getMoment(1), 2);
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
