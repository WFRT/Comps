#include "Correlation.h"
#include "../Options.h"
#include "../Data.h"
#include "../Inputs/Input.h"
#include "../Location.h"
#include "../Ensemble.h"
#include "../Parameters.h"
#include "../Slice.h"
#include "../Selectors/Clim.h"

VarSelectorCorrelation::VarSelectorCorrelation(const Options& iOptions, const Data& iData) :
      VarSelector(iOptions, iData),
      mUseAnomaly(false),
      mUseBias(false) {
   iOptions.getRequiredValue("maxVars", mMaxVars);
   iOptions.getValue("useAnomaly", mUseAnomaly);
   iOptions.getValue("useBias", mUseBias);
   if(mUseAnomaly && mUseBias) {
      Global::logger->write("VarSelectorCorrelation: Can't have both 'useAnomaly' and 'useBias'", Logger::error);
   }
}

void VarSelectorCorrelation::getVariables(const Data& iData,
      int iInit,
      float iOffset,
      const Location& iLocation,
      std::vector<std::string>& iVariables) const {
   Input* inputF = iData.getInput();
   Input* inputO = iData.getObsInput();

   std::vector<std::string> variables;
   inputF->getVariables(variables);

   int locationId = iLocation.getId();

   int numDays = (int) Global::getTimeDiff(mEndDate, 0, 0, mStartDate, 0, 0)/24;

   // Get observation
   std::vector<float> obs;
   obs.resize(numDays, Global::MV);
   int idDate = 0;
   int currDate = mStartDate;
   while(currDate < mEndDate) {
      float currObs = inputO->getValue(currDate, iInit, iOffset, locationId, 0, mVariable);
      if(mUseBias) {
         // Compute the bias
         Ensemble ens;
         inputF->getValues(currDate, iInit, iOffset, locationId, mVariable, ens);
         float ensMean = ens.getMoment(1);

         obs[idDate] = Global::MV;
         if(Global::isValid(currObs) && Global::isValid(ensMean))
            obs[idDate] = currObs - ensMean;
      }
      else {
         obs[idDate] = currObs;
         if(mUseAnomaly && Global::isValid(currObs)) {
            // Compute anomaly
            float clim = iData.getClim(currDate, iInit, iOffset, iLocation, mVariable);
            if(Global::isValid(clim)) {
               obs[idDate] = currObs - clim;
            }
         }
      }

      currDate = Global::getDate(currDate, 24);
      idDate++;
   }

   // Get forecasts and correlation
   std::vector<std::pair<std::string, float> > corrs;

   Options opt("tag=t class=SelectorClim dayLength=15 hourLength=0 allowWrappedOffsets allowFutureValues futureBlackout=10");
   SelectorClim* mClimSelector = new SelectorClim(opt, iData);

   for(int v = 0; v < (int) variables.size(); v++) {
      std::vector<float> forecasts;
      forecasts.resize(numDays, Global::MV);
      idDate = 0;
      currDate = mStartDate;
      while(currDate < mEndDate) {
         // Compute ensemble mean
         Ensemble ens;
         inputF->getValues(currDate, iInit, iOffset, locationId, variables[v], ens);
         forecasts[idDate] = Global::MV;

         float ensMean = ens.getMoment(1);
         forecasts[idDate] = ensMean;

         currDate = Global::getDate(currDate, 24);
         idDate++;
      }
      assert(idDate == numDays);

      float corr = Global::corr(forecasts, obs);
      if(Global::isValid(corr)) {
         float score = 1 - fabs(corr);
         std::pair<std::string, float> p(variables[v], score);
         corrs.push_back(p);
         writeScore(variables[v], corr);
      }
   }
   delete mClimSelector;

   // Sort
   std::sort(corrs.begin(), corrs.end(), Global::sort_pair_second<std::string, float>());
   assert(iVariables.size() == 0);

   // Check how many to add
   std::vector<std::string> vars;
   int bestNum = 0;
   float bestScore = Global::INF;
   for(int v = 0; v < mMaxVars; v++) {
      if(v < corrs.size()) {
         vars.push_back(corrs[v].first);
         float score = VarSelector::run(iData, iInit, iOffset, iLocation, vars, *mDetMetric);
         if(score < bestScore) {
            bestNum = v;
            bestScore = score;
         }
      }
   }
   

   for(int v = 0; v < bestNum; v++) {
      if(v < corrs.size()) {
         iVariables.push_back(corrs[v].first);
      }
   }

}
