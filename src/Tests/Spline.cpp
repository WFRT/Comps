#define BOOST_TEST_MODULE Base
#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Interpolators/Spline.h"
#include "../Member.h"
#include "../Slice.h"
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
         }

         virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
         }
         Data* mData;

   };

   TEST_F(BaseTest, test0) {
      std::vector<float> phi;
      phi.push_back(0);
      phi.push_back(0.1);
      phi.push_back(0.2);
      phi.push_back(0.305);
      phi.push_back(0.39);
      phi.push_back(0.5);
      phi.push_back(0.55);
      phi.push_back(0.6);
      phi.push_back(0.7);
      phi.push_back(0.9999);
      phi.push_back(1);

      Options opt("tag=spline class=InterpolatorSpline numSmooth=9 monotonic");
      //Options opt("tag=spline class=InterpolatorLinear numSmooth=9");

      Interpolator* interpolator = Interpolator::getScheme(opt, *mData);
      std::vector<float> pit;
      pit.push_back(0);
      pit.push_back(0.1);
      pit.push_back(0.2);
      pit.push_back(0.3);
      pit.push_back(0.4);
      pit.push_back(0.5);
      pit.push_back(0.6);
      pit.push_back(0.7);
      pit.push_back(0.8);
      pit.push_back(0.9);
      pit.push_back(1);

      // const float x[] = {0.1,0.2,0.3,0.31, 0.35};
      std::vector<float> x(100);
      x[0] = 0.01;
      for (int i=0; i< 100; i++){
         x[i] =  x[i-1] + 0.01;
      }
      std::cout << "x    y" << std::endl;
      // Loop over all x values
      for(int i = 0; i < 100; i++) {
         float y = interpolator->interpolate(x[i], pit, phi);
         std::cout << x[i] << " " << y << std::endl;
      }

   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
