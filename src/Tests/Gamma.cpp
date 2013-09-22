#include "../Continuous/Gamma.h"
#include "../Data.h"
#include "../Loggers/Default.h"
#include "../Loggers/Logger.h"
#include "../Global.h"
#include "../Member.h"
#include "../Field.h"
#include "../Location.h"
#include "../Parameters.h"
#include "../Obs.h"
#include <gtest/gtest.h>

namespace {
   class GammaTest : public ::testing::Test {
      protected:
         GammaTest() {
            // You can do set-up work for each test here.
            mData = new Data("test.run");

            std::vector<float> ensValues;
            ensValues.push_back(0);
            ensValues.push_back(1);
            ensValues.push_back(2);

            mVariable = "PCP";

            mEnsemble = Ensemble(ensValues, mVariable);
            mDate = 20100101;
            mInit = 0;
            mOffset = 0;
            mLocation = Location("bogus", 0, 49,-123,0);
            mEnsemble.setInfo(mDate, mInit, mOffset, mLocation, mVariable);
         }

         virtual ~GammaTest() {
            delete mData;
            // You can do clean-up work that doesn't throw exceptions here.
         }
         virtual void SetUp() {
            // Code here will be called immediately after the constructor (right
            // before each test).
         }

         void makeEnsemble(const float iValues[], int iN, Ensemble& iEnsemble) {
            std::vector<float> values;
            for(int i = 0; i < iN; i++) {
               values.push_back(iValues[i]);
            }
            iEnsemble = Ensemble(values, mVariable);
            iEnsemble.setInfo(mDate, mInit, mOffset, mLocation, mVariable);
         }

         virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
         }
         static const float mDistanceAccuracy = 1;
         Data* mData;
         Ensemble mEnsemble;
         Location mLocation;
         int mDate;
         int mInit;
         float mOffset;
         std::string mVariable;

   };

   // With default parameters
   TEST_F(GammaTest, default) {
      Continuous* gamma = Continuous::getScheme(Options( "tag=test class=ContinuousGamma estimator=test.mle"), *mData);

      Parameters par;
      gamma->getDefaultParameters(par);
      float x[] = {0,1,2,3};
      float exp[] = { 0,0.2642,0.5940,0.8009};
      for(int i = 0; i < sizeof(x)/sizeof(float); i++) {
         // mean = 1;
         // mu = c0 + c1 cubeRoot(mean) = 2
         // var  = c3 mu                = 2

         // mean = ab
         // var = ab2
         // a = mean2/var = 2 shape
         // b = var/mean = 1 scale
         EXPECT_NEAR(exp[i], gamma->getCdf(x[i], mEnsemble, par), 0.001);
      }

      delete gamma;
   };

   TEST_F(GammaTest, useConst) {
      Continuous* gamma = Continuous::getScheme(Options( "tag=test class=ContinuousGamma estimator=test.mle"), *mData);

      Parameters par;
      gamma->getDefaultParameters(par);

      Obs obs(10, mDate, 0, mVariable, mLocation);
      Ensemble ens;
      float values[] = {0, 10, 20};
      makeEnsemble(values, sizeof(values)/sizeof(float), ens);
      for(int i = 0; i < 100; i++) {
         std::cout << "Par = " << par[0] << " " << par[1] << " " << par[2] << std::endl;
         gamma->updateParameters(ens, obs, par);
      }
      //Obs obs2(100, mDate, 0, mVariable, mLocation);
      EXPECT_FLOAT_EQ(1, par[0]);
      EXPECT_FLOAT_EQ(1, par[1]);
      EXPECT_FLOAT_EQ(1, par[2]);
      EXPECT_FLOAT_EQ(1, par[3]);

      float x[] = {0,1,2,3};
      float exp[] = { 0,0.2642,0.5940,0.8009};
      EXPECT_FLOAT_EQ(0, gamma->getCdf(1, mEnsemble, par));
      for(int i = 0; i < sizeof(x)/sizeof(float); i++) {
         // mean = 1;
         // mu = c0 + c1 cubeRoot(mean) = 2
         // var  = c3 mu                = 2

         // mean = ab
         // var = ab2
         // a = mean2/var = 2 shape
         // b = var/mean = 1 scale

         //EXPECT_NEAR(exp[i], gamma->getCdf(x[i], mEnsemble, par), 0.001);
      }

      delete gamma;
   };
   /*
   TEST_F(GammaTest, useMean) {
      Discrete* logit = Discrete::getScheme(Options("tag=test class=DiscreteGamma useConst useMean efold=2"), *mData);

      float meanOnWet = 2.6; // Ensemble mean on rainy days
      float meanOnDry = 0.2; // Ensemble mean on dry days

      float fracCorrect = 0.2;
      int N = 10;

      // Obs
      std::vector<Obs> dryObs;
      std::vector<Obs> wetObs;
      dryObs.push_back(Obs(0, mDate, 0, mVariable, mLocation)); // Rainy day
      wetObs.push_back(Obs(1, mDate, 0, mVariable, mLocation)); // Dry day

      // Ensemble
      std::vector<float> dryValues;
      std::vector<float> wetValues;
      dryValues.push_back(meanOnDry);
      wetValues.push_back(meanOnWet);
      Ensemble dryEnsemble(dryValues, mVariable);
      Ensemble wetEnsemble(wetValues, mVariable);
      dryEnsemble.setInfo(mDate, mInit, mOffset, mLocation, mVariable);
      wetEnsemble.setInfo(mDate, mInit, mOffset, mLocation, mVariable);

      // Parameters
      Parameters par;
      logit->getDefaultParameters(par);
      EXPECT_EQ(6, par.size());
      EXPECT_FLOAT_EQ(0, par[0]);
      EXPECT_FLOAT_EQ(0, par[1]);
      EXPECT_FLOAT_EQ(1, par[2]);
      EXPECT_FLOAT_EQ(0, par[3]);
      EXPECT_FLOAT_EQ(0, par[4]);
      EXPECT_FLOAT_EQ(1, par[5]);

      // Over the long run, the parameter should be adjusted to give the correct probability of 0
      for(int t = 0; t < 100; t++) {
         for(int i = 0; i < N; i++) {
            if(i < N*fracCorrect) {
               logit->updateParameters(wetEnsemble, wetObs, par);
               logit->updateParameters(dryEnsemble, dryObs, par);
            }
            else {
               logit->updateParameters(wetEnsemble, dryObs, par);
               logit->updateParameters(dryEnsemble, wetObs, par);
            }
            EXPECT_EQ(6, par.size());
         }
      }
      EXPECT_FLOAT_EQ(1, par[0]);
      EXPECT_FLOAT_EQ(1, par[1]);
      float pWet = logit->getP(wetEnsemble, par);
      EXPECT_FLOAT_EQ(1-fracCorrect, pWet);
      float pDry = logit->getP(dryEnsemble, par);
      EXPECT_FLOAT_EQ(fracCorrect, pDry);
   };

   TEST_F(GammaTest, all) {
      Discrete* logit = Discrete::getScheme(Options("tag=test class=DiscreteGamma useConst useMean useFrac efold=2 estimator=test.mle"), *mData);

      float meanOnWet = 2.6; // Ensemble mean on rainy days
      float meanOnDry = 0.2; // Ensemble mean on dry days

      float fracCorrect = 0.2;
      int N = 10;

      // Obs
      std::vector<Obs> obs;
      obs.push_back(Obs(0, mDate, 0, mVariable, mLocation)); // Dry day
      obs.push_back(Obs(1, mDate, 0, mVariable, mLocation)); // Dry day
      obs.push_back(Obs(0, mDate, 0, mVariable, mLocation)); // Rainy day
      obs.push_back(Obs(1, mDate, 0, mVariable, mLocation)); // Rainy day

      // Ensemble

      float values0[] = {0,0,0,3,1};
      float values1[] = {0,0,0,0,1};
      float values2[] = {0,0,1,9,12};
      float values3[] = {0,1,3,4,3};

      Ensemble ens0;
      Ensemble ens1;
      Ensemble ens2;
      Ensemble ens3;
      makeEnsemble(values0, sizeof(values0)/sizeof(float), ens0);
      makeEnsemble(values1, sizeof(values1)/sizeof(float), ens1);
      makeEnsemble(values2, sizeof(values2)/sizeof(float), ens2);
      makeEnsemble(values3, sizeof(values3)/sizeof(float), ens3);

      std::vector<Ensemble> ens;
      ens.push_back(ens0);
      ens.push_back(ens1);
      ens.push_back(ens2);
      ens.push_back(ens3);

      float exp[] = {0,0,0,0};
      
      // Parameters
      Parameters par;
      logit->getDefaultParameters(par);
      EXPECT_EQ(12, par.size());
      EXPECT_FLOAT_EQ(0, par[0]);
      EXPECT_FLOAT_EQ(0, par[1]);
      EXPECT_FLOAT_EQ(0, par[2]);
      EXPECT_FLOAT_EQ(1, par[3]);
      EXPECT_FLOAT_EQ(0, par[4]);
      EXPECT_FLOAT_EQ(0, par[5]);
      EXPECT_FLOAT_EQ(0, par[6]);
      EXPECT_FLOAT_EQ(1, par[7]);
      EXPECT_FLOAT_EQ(0, par[8]);
      EXPECT_FLOAT_EQ(0, par[9]);
      EXPECT_FLOAT_EQ(0, par[10]);
      EXPECT_FLOAT_EQ(1, par[11]);

      // Over the long run, the parameter should be adjusted to give the correct probability of 0
      for(int t = 0; t < 500; t++) {
         for(int i = 0; i < (int) ens.size(); i++) {
            std::vector<Obs> currObs;
            currObs.push_back(obs[i]);
            logit->updateParameters(ens[i], currObs, par);
            EXPECT_EQ(12, par.size());
         }
      }
      EXPECT_FLOAT_EQ(1, par[0]);
      EXPECT_FLOAT_EQ(1, par[1]);
      EXPECT_FLOAT_EQ(1, par[2]);
      for(int i = 0; i < (int) ens.size(); i++) {
         float p = logit->getP(ens[i], par);
         EXPECT_FLOAT_EQ(exp[i], p);
      }
   };
   */
   
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
