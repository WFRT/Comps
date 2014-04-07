#define BOOST_TEST_MODULE Run
#include "../Run.h"
#include "../Options.h"
#include <gtest/gtest.h>
// Google already has an object called Run
typedef Run CompsRun;

namespace Gtest {
   class RunTest : public ::testing::Test {
      protected:
         RunTest() {
         }
   };

   TEST_F(RunTest, test1) {
      Options options;
      options.addOption("inputs", "tutFcst,tutObs");
      options.addOption("varconfs", "T");
      options.addOption("pooler", "test.poolerDefault");
      std::stringstream ss;
      ss << "0,12,13.5,24";
      options.addOption("offsets", ss.str());
      CompsRun run(options);

      // Test that inputs are loaded
      Input* inputF = run.getData()->getInput();
      Input* inputO = run.getData()->getObsInput();
      ASSERT_EQ(true, inputF != NULL);
      ASSERT_EQ(true, inputO != NULL);
      EXPECT_EQ("tutFcst", inputF->getName());
      EXPECT_EQ("tutObs",  inputO->getName());

      // Test that offsets are loaded
      std::vector<float> offsets = run.getOffsets();
      EXPECT_EQ(4,  offsets.size());
      EXPECT_EQ(0,  offsets[0]);
      EXPECT_FLOAT_EQ(13.5,  offsets[2]);
   }

   TEST_F(RunTest, locations) {
      Options options;
      options.addOption("inputs", "tutFcst,tutObs");
      options.addOption("varconfs", "T");
      CompsRun run(options);

      // Test that locations are loaded
      Input* inputO = run.getData()->getObsInput();
      std::vector<Location> obsLocations = inputO->getLocations();
      std::vector<Location> runLocations = run.getLocations();
      EXPECT_EQ(obsLocations.size(), runLocations.size());

      std::vector<std::pair<int, float> > obsInfo;
      std::vector<std::pair<int, float> > runInfo;
      for(int i = 0; i < runLocations.size(); i++) {
         obsInfo.push_back(std::pair<int, float>(obsLocations[i].getId(), obsLocations[i].getLat()));
         runInfo.push_back(std::pair<int, float>(runLocations[i].getId(), runLocations[i].getLat()));
      }
      for(int i = 0; i < runInfo.size(); i++) {
         bool status = std::find(obsInfo.begin(), obsInfo.end(), runInfo[i]) != obsInfo.end();
         EXPECT_TRUE(status);
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
