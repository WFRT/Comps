#define BOOST_TEST_MODULE Base
#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Interpolators/Spline.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

namespace {
   class BaseTest : public ::testing::Test {
      protected:
         BaseTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run2");
         }
         virtual ~BaseTest() {
            delete mData;
            // You can do clean-up work that doesn't throw exceptions here.
         }
         virtual void SetUp() {
            // Code here will be called immediately after the constructor (right
            // before each test).

            // Number of interior points
            std::vector<int> numSmooths;
            numSmooths.push_back(0);
            numSmooths.push_back(1);
            numSmooths.push_back(4);
            numSmooths.push_back(9);

            for(int i = 0; i < numSmooths.size(); i++) {
               int smooth = numSmooths[i];
               std::vector<float> pit;
               std::vector<float> phi;
               for(int k = 0; k <= smooth+1; k++) {
                  pit.push_back((float) k/(smooth+1));
                  phi.push_back((float) k/(smooth+1));
               }
               mPit.push_back(pit);
               mPhi.push_back(phi);
            }

            std::vector<std::string> optStrings;
            optStrings.push_back("tag=spline class=InterpolatorSpline monotonic");
            optStrings.push_back("tag=spline class=InterpolatorSpline");
            optStrings.push_back("tag=spline class=InterpolatorLinear");


            for(int k = 0; k < optStrings.size(); k++) {
               std::stringstream ss;
               ss << optStrings[k];
               mOpts.push_back(Options(ss.str()));
            }
         }

         virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
         }
         Data* mData;
         std::vector<std::vector<float> > mPhi;
         std::vector<std::vector<float> > mPit;
         std::vector<Options> mOpts;

   };

   TEST_F(BaseTest, throughPoints) {

      std::vector<float> x;
      for(int i = 0; i < 101; i++) {
         x.push_back((float) i / 100);
      }

      int N = mOpts.size();
      for(int n = 0; n < N; n++) {
         Interpolator* interpolator = Interpolator::getScheme(mOpts[n], *mData);
         for(int i = 0; i < mPit.size(); i++) {
            for(int k = 0; k < mPit[i].size(); k++) {
               float y = interpolator->interpolate(mPit[i][k], mPit[i], mPhi[i]);
               EXPECT_FLOAT_EQ(mPhi[i][k],y);
            }
         }
         delete interpolator;
      }
   }
   TEST_F(BaseTest, monotonic) {
      int N = mOpts.size();
      for(int n = 0; n < N; n++) {
         Interpolator* interpolator = Interpolator::getScheme(mOpts[n], *mData);
         for(int i = 0; i < mPit.size(); i++) {
            float last = -1;
            for(int k = 0; k < 101; k++) {
               float x = (float) k / 100;
               float y = interpolator->interpolate(x, mPit[i], mPhi[i]);
               if(interpolator->isMonotonic()) {
                  EXPECT_EQ(y > last, true);
                  float slope = interpolator->slope(x, mPit[i], mPhi[i]);
                  EXPECT_EQ(slope >= 0, true);
               }
               last = y;
            }
         }
         delete interpolator;
      }
   }
   TEST_F(BaseTest, empty) {
      int N = mOpts.size();
      std::vector<float> pit;
      std::vector<float> phi;

      for(int i = 0; i < N; i++) {
         Interpolator* interpolator = Interpolator::getScheme(mOpts[i], *mData);
         for(int i = 0; i < 101; i++) {
            float x = (float) i / 100;
            float y = interpolator->interpolate(x, pit, phi);
            EXPECT_EQ(y, Global::MV);
         }
         delete interpolator;
      }
   }
   TEST_F(BaseTest, Equal) {
      // Two points are equal
      int N = mOpts.size();
      std::vector<float> pit;
      std::vector<float> phi;
      pit.push_back(0);
      pit.push_back(0.1);
      pit.push_back(0.1);
      pit.push_back(1);
      phi = pit;

      for(int i = 0; i < N; i++) {
         Interpolator* interpolator = Interpolator::getScheme(mOpts[i], *mData);
         float x = 0.1;
         float y = interpolator->interpolate(x, pit, phi);
         EXPECT_FLOAT_EQ(y, 0.1);
         delete interpolator;
      }
   }
   TEST_F(BaseTest, SameY) {
      // Two Y points are equal
      int N = mOpts.size();
      std::vector<float> pit;
      std::vector<float> phi;
      pit.push_back(0);
      pit.push_back(0.1);
      pit.push_back(0.2);
      pit.push_back(1);
      phi = pit;
      phi[2] = 0.1;

      for(int i = 0; i < N; i++) {
         Interpolator* interpolator = Interpolator::getScheme(mOpts[i], *mData);
         if(interpolator->isMonotonic()) {
            float x = 0.15;
            float y = interpolator->interpolate(x, pit, phi);
            EXPECT_FLOAT_EQ(y, 0.1);
            delete interpolator;
         }
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
