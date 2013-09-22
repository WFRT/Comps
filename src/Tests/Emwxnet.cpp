#define BOOST_TEST_MODULE Emwxnet
#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Parameters.h"
#include "../ParameterIos/ParameterIo.h"
#include "../Configurations/Configuration.h"
#include <gtest/gtest.h>

namespace {
   class EmwxnetTest : public ::testing::Test {
      protected:
         EmwxnetTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.emwxnet");
            Options opt("tag=parEmwxnet class=ParameterIoEmwxnet finder=finder");
            mParIo = ParameterIo::getScheme(opt, *mData);
         }
         virtual ~EmwxnetTest() {
            delete mData;
            delete mParIo;
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
         ParameterIo* mParIo ;
   };

   TEST_F(EmwxnetTest, write) {
      int date = 20120101;
      int init = 0;
      float offset = 0;
      int index = 0;
      Location location("naefs", 415);
      std::string variable = "TMin24";
      Configuration conf("confRaw", *mData);

      std::vector<float> values;
      values.push_back(1);
      values.push_back(4.3);
      Parameters par(values);
      mParIo->add(Component::TypeCorrector, date, init, offset, location, variable, conf, index, par);
      mParIo->write();
   }
   TEST_F(EmwxnetTest, read) {
      int date = 20120101;
      int init = 0;
      float offset = 0;
      int index = 0;
      Location location("naefs", 415);
      std::string variable = "TMin24";
      Configuration conf("confRaw", *mData);

      Parameters par;
      mParIo->read(Component::TypeCorrector, date, init, offset, location, variable, conf, index, par);
      ASSERT_EQ(par.size(), 2);
      EXPECT_FLOAT_EQ(par[0], 1);
      EXPECT_FLOAT_EQ(par[1], 4.3);

      mParIo->read(Component::TypeSelector, date, init, offset, location, variable, conf, index, par);
      EXPECT_EQ(par.size(), 0);
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
