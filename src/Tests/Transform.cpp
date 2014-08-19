#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Transforms/Transform.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

namespace {
   class TransformTest : public ::testing::Test {
      protected:
         TransformTest() {
            // You can do set-up work for each test here.
            mTransform    = Transform::getScheme(Options("tag=test class=TransformPower power=2"));
            mTransformLog = Transform::getScheme(Options("tag=test class=TransformLog base=10"));
            mTransformAbs = Transform::getScheme(Options("tag=test class=TransformAbsolute"));
         }
         virtual ~TransformTest() {
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
         template <class T>
         static bool sameSize(T array1[], T array2[]) {
            return sizeof(array1) == sizeof(array2);
         }
         Transform* mTransform;
         Transform* mTransformLog;
         Transform* mTransformAbs;
   };

   TEST_F(TransformTest, power2) {
      float inputs[]   = {-1,0,0.1,1,2,3, Global::MV};
      float expected[] = {1,0,0.01,1,4,9, Global::MV};

      assert(sameSize(inputs, expected));
      for(int i = 0; i < sizeof(inputs)/sizeof(float); i++) {
         float value = mTransform->transform(inputs[i]);
         EXPECT_FLOAT_EQ(expected[i], value);
      }
   }
   // Test that the inverse works
   TEST_F(TransformTest, power2inverse) {
      float inputs[]   = {1,0,0.01,1,4,9, Global::MV};
      float expected[] = {1,0,0.1,1,2,3, Global::MV};

      assert(sameSize(inputs, expected));
      for(int i = 0; i < sizeof(inputs)/sizeof(float); i++) {
         float value = mTransform->inverse(inputs[i]);
         EXPECT_FLOAT_EQ(expected[i], value);
      }
   }
   TEST_F(TransformTest, power2ensemble) {
      // Test that ensemble is properly transformed
      std::vector<float> ensValues;
      ensValues.push_back(2);
      ensValues.push_back(3);
      ensValues.push_back(0);
      ensValues.push_back(3);
      ensValues.push_back(11);
      float expected[] = {4,9,0,9,121};

      // Try each subset of the ensemble starting from the first value and up
      for(int e = 1; e < ensValues.size(); e++) {
         std::vector<float> ensTrunc;
         for(int i = 0; i < e; i++)
            ensTrunc.push_back(ensValues[i]);
         Ensemble ens(ensTrunc, "T");
         Ensemble orig = ens;

         mTransform->transform(ens);
         EXPECT_TRUE(ens.size() == e);
         EXPECT_TRUE(ens.size() <= sizeof(expected)/sizeof(float));
         for(int i = 0; i < ens.size(); i++) {
            EXPECT_FLOAT_EQ(expected[i], ens[i]);
         }
         mTransform->inverse(ens);
         EXPECT_EQ(ens.size(), orig.size());
         for(int i = 0; i < ens.size(); i++) {
            EXPECT_FLOAT_EQ(ens[i], orig[i]);
         }
      }
   }
   ///////////////////
   // Log transform //
   ///////////////////
   TEST_F(TransformTest, log) {
      float inputs[]   = {-1,0,0.1,1,10,100, Global::MV};
      float expected[] = {Global::MV,Global::MV,-1,0,1,2, Global::MV};

      assert(sameSize(inputs, expected));
      for(int i = 0; i < sizeof(inputs)/sizeof(float); i++) {
         float value = mTransformLog->transform(inputs[i]);
         EXPECT_FLOAT_EQ(expected[i], value);
      }
   }
   // Test that the inverse works
   TEST_F(TransformTest, logInverse) {
      float inputs[]   = {-1,0,1,2, Global::MV};
      float expected[] = {0.1,1,10,100, Global::MV};

      assert(sameSize(inputs, expected));
      for(int i = 0; i < sizeof(inputs)/sizeof(float); i++) {
         float value = mTransformLog->inverse(inputs[i]);
         EXPECT_FLOAT_EQ(expected[i], value);
      }
   }
   TEST_F(TransformTest, logEnsemble) {
      // Test that ensemble is properly transformed
      std::vector<float> ensValues;
      ensValues.push_back(0.1);
      ensValues.push_back(10);
      ensValues.push_back(1);
      ensValues.push_back(100);
      ensValues.push_back(1000);
      float expected[] = {-1,1,0,2,3};

      // Try each subset of the ensemble starting from the first value and up
      for(int e = 1; e < ensValues.size(); e++) {
         std::vector<float> ensTrunc;
         for(int i = 0; i < e; i++)
            ensTrunc.push_back(ensValues[i]);
         Ensemble ens(ensTrunc, "T");
         Ensemble orig = ens;

         mTransformLog->transform(ens);
         EXPECT_TRUE(ens.size() == e);
         EXPECT_TRUE(ens.size() <= sizeof(expected)/sizeof(float));
         for(int i = 0; i < ens.size(); i++) {
            EXPECT_FLOAT_EQ(expected[i], ens[i]);
         }
         mTransformLog->inverse(ens);
         EXPECT_EQ(ens.size(), orig.size());
         for(int i = 0; i < ens.size(); i++) {
            EXPECT_FLOAT_EQ(ens[i], orig[i]);
         }
      }
   }
   ///////////////////
   // Abs transform //
   ///////////////////
   TEST_F(TransformTest, abs) {
      float inputs[]   = {-5,-0.3,0,0.4,3, Global::MV};
      float expected[] = {5,0.3,0,0.4,3,Global::MV};

      assert(sameSize(inputs, expected));
      for(int i = 0; i < sizeof(inputs)/sizeof(float); i++) {
         float value = mTransformAbs->transform(inputs[i]);
         EXPECT_FLOAT_EQ(expected[i], value);
      }
   }
   // Test that the inverse works
   TEST_F(TransformTest, absInverse) {
      float inputs[]   = {-5,-0.3,0,0.4,3, Global::MV};
      float expected[] = {Global::MV,Global::MV, 0, 0.4,3,Global::MV};

      assert(sameSize(inputs, expected));
      for(int i = 0; i < sizeof(inputs)/sizeof(float); i++) {
         float value = mTransformAbs->inverse(inputs[i]);
         EXPECT_FLOAT_EQ(expected[i], value);
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
