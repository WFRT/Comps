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
   TEST_F(OptionsTest, noValue) {
      Options opt("length=2 q=");
      int valueI = 3;
      EXPECT_EQ(opt.getValue("length", valueI), true);
      EXPECT_EQ(valueI, 2);
      std::string valueS = "test";
      EXPECT_EQ(opt.getValue("q", valueS), true);
      EXPECT_EQ(valueS, "");
   }
   TEST_F(OptionsTest, singleAtrribute) {
      Options opt("length=2");
      // Correct attribute
      {
         // Optional
         int valueI = 3;
         EXPECT_EQ(opt.getValue("length", valueI), true);
         EXPECT_EQ(valueI, 2);

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
         EXPECT_EQ(valueI, 2);

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
      // Fractional increment
      {
         Options opt("lengths=5:0.4:6");
         std::vector<float> valuesF;
         EXPECT_EQ(opt.getValues("lengths", valuesF), true);
         EXPECT_EQ(valuesF.size(), 3);
         EXPECT_FLOAT_EQ(valuesF[0], 5);
         EXPECT_FLOAT_EQ(valuesF[1], 5.4);
         EXPECT_FLOAT_EQ(valuesF[2], 5.8);
      }
      // Discard comments
      {
         Options opt("length=5 # test=123");
         int value;
         EXPECT_EQ(opt.getValue("length", value), true);
         EXPECT_EQ(opt.getValue("test", value), false);
      }
   }
   // Check that embedded attributes work
   TEST_F(OptionsTest, embedded) {
      {
         Options opt("scheme=[test=1 test2=2]");
         std::string value;
         EXPECT_EQ(opt.getValue("scheme", value), true);
         EXPECT_EQ(value, "[test=1 test2=2]");
      }
   }
   // Boolean options
   TEST_F(OptionsTest, boolOptions) {
      {
         Options opt("test=123 doWork");
         bool value;
         EXPECT_EQ(opt.getValue("doWork", value), true);
         EXPECT_EQ(value, true);
      }
      {
         Options opt("doWork");
         bool value;
         EXPECT_EQ(opt.getValue("doWork", value), true);
         EXPECT_EQ(value, true);
      }
   }
   // To string
   TEST_F(OptionsTest, toString) {
      // The toString function should recreated the contents in the options.
      // However the order of the attribtues is unspecified, so we cannot just check
      // that the string generating the options equals the result of toString()
      // Instead check that the final string has all the attributes
      std::string genString = "test=123 doWork";
      Options opt(genString);
      std::string resString = opt.toString();
      Options opt2(resString);
      int value;
      opt2.getValue("test", value);
      EXPECT_EQ(value, 123);
      opt2.getValue("doWork", value);
      EXPECT_EQ(value, 1);
   }
   TEST_F(OptionsTest, getKeys) {
      {
         Options opt("length=123");
         std::vector<std::string> keys = opt.getKeys();
         EXPECT_EQ(keys.size(), 1);
         EXPECT_EQ(keys[0], "length");
      }{
         Options opt("length=123 doWork");
         std::vector<std::string> keys = opt.getKeys();
         EXPECT_EQ(keys.size(), 2);
         EXPECT_EQ(std::find(keys.begin(), keys.end(), "length") != keys.end(), true);
         EXPECT_EQ(std::find(keys.begin(), keys.end(), "doWork") != keys.end(), true);
      }

   }
   // Append options to existing options
   TEST_F(OptionsTest, addOption) {
      {
         Options opt("length=123");
         opt.addOption("length", 4);
         int value;
         EXPECT_EQ(opt.getValue("length", value), true);
         EXPECT_EQ(value, 4);

         opt.addOption("test", 5);
         EXPECT_EQ(opt.getValue("test", value), true);
         EXPECT_EQ(value, 5);

         opt.addOption("testString", "someString");
         std::string valueString;
         EXPECT_EQ(opt.getValue("testString", valueString), true);
         EXPECT_EQ(valueString, "someString");

         opt.addBoolOption("testBool");
         bool valueBool;
         EXPECT_EQ(opt.getValue("testBool", valueBool), true);
         EXPECT_EQ(valueBool, true);

         Options optNew("test2=5");
         opt.addOptions(optNew);
         EXPECT_EQ(opt.getValue("test2", value), true);
         EXPECT_EQ(value, 5);

         std::vector<int> values0;
         std::vector<int> values1;
         values0.push_back(1);
         values0.push_back(4);
         opt.addOptions("test", values0);
         EXPECT_EQ(opt.getValues("test", values1), true);
         EXPECT_EQ(values1.size(), values0.size());
         for(int i = 0; i < values0.size(); i++) {
            EXPECT_EQ(values1[i], values0[i]);
         }
      }
   }
   TEST_F(OptionsTest, copyOption) {
      {
         Options opt1("length=3");
         Options opt2("length=7");
         Options::copyOption("length", opt1, opt2);
         int value;
         EXPECT_TRUE(opt2.getValue("length", value));
         EXPECT_EQ(value, 3);
         Options::copyOption("doesntExist", opt1, opt2);
         EXPECT_FALSE(opt1.hasValue("doesntExist"));
         EXPECT_FALSE(opt2.hasValue("doesntExist"));
      }
      {
         Options opt1("doWork length=3 noWork lengths=1,3,4 noLengths=4,5,6 noLength=1");
         Options opt2;
         Options::copyOption("length", opt1, opt2);
         Options::copyOption("doWork", opt1, opt2);
         Options::copyOption("lengths", opt1, opt2);
         int value;
         EXPECT_TRUE(opt2.getValue("doWork", value));
         EXPECT_EQ(value, 1);
         EXPECT_TRUE(opt2.getValue("length", value));
         EXPECT_EQ(value, 3);
         std::vector<float> values;
         EXPECT_TRUE(opt2.getValues("lengths", values));
         EXPECT_EQ(values.size(), 3);
         EXPECT_EQ(values[0], 1);
         EXPECT_EQ(values[1], 3);
         EXPECT_EQ(values[2], 4);
         EXPECT_FALSE(opt2.hasValue("noWork"));
         EXPECT_FALSE(opt2.hasValue("noLength"));
         EXPECT_FALSE(opt2.hasValue("noLengths"));
      }
   }
   TEST_F(OptionsTest, hasValue) {
      {
         Options opt("");
         EXPECT_FALSE(opt.hasValue("length"));
         EXPECT_FALSE(opt.hasValues("length"));
      }
      {
         Options opt;
         EXPECT_FALSE(opt.hasValue("length"));
         EXPECT_FALSE(opt.hasValues("length"));
      }
      {
         Options opt("length=1 lengths=1,2,3 doWork");
         EXPECT_TRUE(opt.hasValue("length"));
         EXPECT_TRUE(opt.hasValues("length"));
         EXPECT_FALSE(opt.hasValue("lengths"));
         EXPECT_TRUE(opt.hasValues("lengths"));
         EXPECT_TRUE(opt.hasValue("doWork"));
         EXPECT_TRUE(opt.hasValues("doWork"));
         opt.addOption("length", "1,2");
         EXPECT_FALSE(opt.hasValue("length"));
         EXPECT_TRUE(opt.hasValues("length"));
      }
   }
   TEST_F(OptionsTest, scheme) {
      {
         Options opt1("neighbourhood=[class=DownscalerNearest num=5]");

         std::string neighbourhood;
         EXPECT_TRUE(opt1.getValue("neighbourhood", neighbourhood));
         EXPECT_EQ(neighbourhood,"[class=DownscalerNearest num=5]");
      }
   }
   TEST_F(OptionsTest, mulitpleSchemes) {
      {
         Options opt1("neighbourhoods=[class=DownscalerNearest num=5],[class=DownscalerNearest num=10]");

         std::vector<std::string> neighbourhoods;
         EXPECT_TRUE(opt1.getValues("neighbourhoods", neighbourhoods));
         EXPECT_EQ(neighbourhoods[0],"[class=DownscalerNearest num=5]");
         EXPECT_EQ(neighbourhoods[1],"[class=DownscalerNearest num=10]");
      }
   }
   TEST_F(OptionsTest, mulitpleSchemesMix) {
      {
         Options opt1("neighbourhoods=[class=DownscalerNearest num=5],3,test,[class=DownscalerNearest num=10]");

         std::vector<std::string> neighbourhoods;
         EXPECT_TRUE(opt1.getValues("neighbourhoods", neighbourhoods));
         ASSERT_EQ(neighbourhoods.size(),4);
         EXPECT_EQ(neighbourhoods[0],"[class=DownscalerNearest num=5]");
         EXPECT_EQ(neighbourhoods[1],"3");
         EXPECT_EQ(neighbourhoods[2],"test");
         EXPECT_EQ(neighbourhoods[3],"[class=DownscalerNearest num=10]");
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
