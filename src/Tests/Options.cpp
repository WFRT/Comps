#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Slice.h"
#include "../Location.h"
#include "../Parameters.h"
#include <gtest/gtest.h>

namespace {
   class OptionsTest : public ::testing::Test {
      protected:
         OptionsTest() {
            // You can do set-up work for each test here.
         }

         virtual ~OptionsTest() {
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

   };

   TEST_F(OptionsTest, noAttributes) {
      {
         Options opt("");
         float value = 1.1;
         bool status = opt.getValue("length", value);
         EXPECT_FLOAT_EQ(value, 1.1);
         EXPECT_EQ(status, false);
      }
      {
         // Options without input
         Options opt;
         float value = 1.1;
         bool status = opt.getValue("length", value);
         EXPECT_FLOAT_EQ(value, 1.1);
         EXPECT_EQ(status, false);
      }
   }
   TEST_F(OptionsTest, singleAtrribute) {
      Options opt("length=2");
      // Correct attribute
      {
         // Optional
         int valueI = 3;
         EXPECT_EQ(opt.getValue("length", valueI), true);
         EXPECT_FLOAT_EQ(valueI, 2);

         float valueF = 1.1;
         EXPECT_EQ(opt.getValue("length", valueF), true);
         EXPECT_FLOAT_EQ(valueF, 2);

         std::string valueS  = "none";
         EXPECT_EQ(opt.getValue("length", valueS), true);
         EXPECT_EQ(valueS, "2");

         std::vector<float> valuesF;
         EXPECT_EQ(opt.getValues("length", valuesF), true);
         EXPECT_EQ(valuesF.size(), 1);
         EXPECT_FLOAT_EQ(valuesF[0], 2);

         // Required
         valueI = 3;
         opt.getRequiredValue("length", valueI);
         EXPECT_FLOAT_EQ(valueI, 2);

         valueF = 1.1;
         opt.getRequiredValue("length", valueF);
         EXPECT_FLOAT_EQ(valueF, 2);

         valueS  = "none";
         opt.getValue("length", valueS);
         EXPECT_EQ(valueS, "2");

         valuesF.clear();
         opt.getRequiredValues("length", valuesF);
         EXPECT_EQ(valuesF.size(), 1);
         EXPECT_FLOAT_EQ(valuesF[0], 2);
      }
      // Missing attribute
      {
         float valueF = 2.1;
         EXPECT_EQ(opt.getValue("test", valueF), false);
         EXPECT_FLOAT_EQ(valueF, 2.1);

         std::vector<float> valuesF;
         EXPECT_EQ(opt.getValues("test", valuesF), false);
         EXPECT_EQ(valuesF.size(), 0);
      }
   }
   TEST_F(OptionsTest, singleVectorAttribute) {
      Options opt("lengths=1,2,3");
      // Correct attribute
      {
         // Should not find any single value attribute called lengths
         float valueF = 1.1;
         EXPECT_EQ(opt.getValue("lengths", valueF), false);
         EXPECT_FLOAT_EQ(valueF, 1.1);

         std::vector<float> valuesF;
         EXPECT_EQ(opt.getValues("lengths", valuesF), true);
         EXPECT_EQ(valuesF.size(), 3);
         EXPECT_FLOAT_EQ(valuesF[0], 1);
         EXPECT_FLOAT_EQ(valuesF[1], 2);
         EXPECT_FLOAT_EQ(valuesF[2], 3);
      }
      // Missing attribute
      {
         float value = 2.1;
         EXPECT_EQ(opt.getValue("test", value), false);
         EXPECT_FLOAT_EQ(value, 2.1);

         std::vector<float> valuesF;
         EXPECT_EQ(opt.getValues("test", valuesF), false);
         EXPECT_EQ(valuesF.size(), 0);
      }
   }
   // Check that attributes of the form start:inc:finish work
   TEST_F(OptionsTest, usingColons) {
      {
         Options opt("lengths=1:3");
         // Should not find any single value attribute called lengths
         int valueF = -287;
         EXPECT_EQ(opt.getValue("lengths", valueF), false);
         EXPECT_FLOAT_EQ(valueF, -287);

         std::vector<int> valuesF;
         EXPECT_EQ(opt.getValues("lengths", valuesF), true);
         EXPECT_EQ(valuesF.size(), 3);
         EXPECT_FLOAT_EQ(valuesF[0], 1);
         EXPECT_FLOAT_EQ(valuesF[1], 2);
         EXPECT_FLOAT_EQ(valuesF[2], 3);
      }
      {
         Options opt("lengths=4,1:3");
         // Should not find any single value attribute called lengths
         int valueF = -287;
         EXPECT_EQ(opt.getValue("lengths", valueF), false);
         EXPECT_FLOAT_EQ(valueF, -287);

         std::vector<int> valuesF;
         EXPECT_EQ(opt.getValues("lengths", valuesF), true);
         EXPECT_EQ(valuesF.size(), 4);
         EXPECT_FLOAT_EQ(valuesF[0], 4);
         EXPECT_FLOAT_EQ(valuesF[1], 1);
         EXPECT_FLOAT_EQ(valuesF[2], 2);
         EXPECT_FLOAT_EQ(valuesF[3], 3);
      }
      {
         Options opt("lengths=1:3,5,9:-2:6,11,4:6:5");
         // Should not find any single value attribute called lengths
         int valueF = -287;
         EXPECT_EQ(opt.getValue("lengths", valueF), false);
         EXPECT_FLOAT_EQ(valueF, -287);

         std::vector<int> valuesF;
         EXPECT_EQ(opt.getValues("lengths", valuesF), true);
         EXPECT_EQ(valuesF.size(), 8);
         EXPECT_FLOAT_EQ(valuesF[0], 1);
         EXPECT_FLOAT_EQ(valuesF[1], 2);
         EXPECT_FLOAT_EQ(valuesF[2], 3);
         EXPECT_FLOAT_EQ(valuesF[3], 5);
         EXPECT_FLOAT_EQ(valuesF[4], 9);
         EXPECT_FLOAT_EQ(valuesF[5], 7);
         EXPECT_FLOAT_EQ(valuesF[6], 11);
         EXPECT_FLOAT_EQ(valuesF[7], 4);
      }
      // Wrong sign on increment
      {
         Options opt("lengths=5:-1:6");
         std::vector<int> valuesF;
         EXPECT_EQ(opt.getValues("lengths", valuesF), false);
         EXPECT_EQ(valuesF.size(), 0);
      }
      // 0 increment
      {
         Options opt("lengths=5:0:6");
         std::vector<int> valuesF;
         EXPECT_EQ(opt.getValues("lengths", valuesF), false);
         EXPECT_EQ(valuesF.size(), 0);
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
