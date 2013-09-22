#define BOOST_TEST_MODULE Base
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
   class BaseTest : public ::testing::Test {
      protected:
         BaseTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run");
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

   TEST_F(BaseTest, test1) {
      int date = 20110101;
      EXPECT_EQ(date, 20110101);
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
