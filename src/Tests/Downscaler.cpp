#define BOOST_TEST_MODULE Downscaler
#include "../Downscalers/NearestNeighbour.h"
#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

namespace {
   class DownscalerTest : public ::testing::Test {
      protected:
         DownscalerTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run");
         }
         virtual ~DownscalerTest() {
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
         Data* mData;
         static const float mTol = 0.01;

   };

   TEST_F(DownscalerTest, locations) {
      DownscalerNearestNeighbour downscaler = DownscalerNearestNeighbour(Options("tag=test"), *mData);
      int date = 20110101;
      int init = 0;
      float offset = 0;
      std::string variable = "T";

      float lats[] = {49, 49.1, 49.6, 49.9};
      float lons[] = {-123,-123,-123,-123};
      float ans[]  = {5.05, 5.05, -7.65, -25.35};

      for(int i = 0; i < sizeof(lats)/sizeof(float); i++) {
         Location location("", 0, lats[i], lons[i], 0);
         std::vector<float> parVector;
         Parameters parameters(parVector);
         Member member("gfs", 0);
         Field slice(date, init, offset, member);
         float value = downscaler.downscale(slice, variable, location, parameters);
         EXPECT_NEAR(ans[i], value, mTol);
      }

      /*
         {
         Member member("ubc-mv-obs", 0);
         Field slice(date, init, offset, member);
         float value = downscaler.downscale(slice, variable, location, parameters);
         BOOST_CHECK_EQUAL(value,1);
         }
         */
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
