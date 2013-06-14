#include "Find.h"
#include "../Options.h"
#include "../Data.h"
#include "../Location.h"
#include "../Member.h"
#include "../Scheme.h"
#include "../DetMetrics/DetMetric.h"
#include "../Slice.h"
#include "../Parameters.h"
#include "../Selectors/Analog.h"

VarSelectorFind::VarSelectorFind(const Options& iOptions, const Data& iData) : VarSelector(iOptions, iData) {
   iOptions.getRequiredValue("maxVars", mMaxVars);
   iOptions.getRequiredValue("trainingDays", mTrainingDays);
}

void VarSelectorFind::getVariables(const Data& iData,
      int iInit,
      float iOffset,
      const Location& iLocation,
      std::vector<std::string>& iVariables) const {
   Input* inputF = iData.getInput();
   Input* inputO = iData.getObsInput();

   std::vector<std::string> variables;
   inputF->getVariables(variables);

   int locationId = iLocation.getId();

   // Members
   std::vector<Member> members;
   inputF->getMembers(members);

   std::vector<std::string> varList;
   std::vector<std::string> varRemaining = variables;

   int numDays = (int) Global::getTimeDiff(mEndDate, 0, 0, mStartDate, 0, 0)/24;
   std::vector<float> obsTarget;
   obsTarget.resize(numDays);

   // Load the target observations
   for(int d = 0; d < numDays; d++) {
      int currDate = Global::getDate(mStartDate, 24*d);
      obsTarget[d] = inputO->getValue(currDate, iInit, iOffset, iLocation.getId(), 0, mVariable);
   }

   std::map<std::string, std::vector<float> > forecasts;

   std::vector<std::pair<std::string, float> > corrs;
   SelectorAnalog selector(makeOptionsObs(mVariable), iData);
   Parameters par;
   selector.getDefaultParameters(par);
   for(int v = 0; v < (int) variables.size(); v++) {
      std::string currVar = variables[v];
      // Load the observations
      /*
      std::vector<float> obs;
      obs.resize(numDays);
      for(int d = 0; d < numDays; d++) {
         int currDate = Global::getDate(mStartDate, 24*d);
         obs[d] = inputO->getValue(currDate, iInit, iOffset, iLocation.getId(), 0, variables[v]);
      }
      */
      double startTime = Global::clock();

      // Load forecasts
      forecasts[currVar].resize(numDays-mTrainingDays);
      for(int d = mTrainingDays; d < numDays; d++) {
         int currDate = Global::getDate(mStartDate, 24*d);
         std::vector<float> values;
         inputF->getValues(currDate, iInit, iOffset, iLocation.getId(), currVar, values);
         forecasts[currVar][d-mTrainingDays] = Global::mean(values);
      }

      // For each day, find the analog that would have been the best
      std::vector<float> forecastsAtBest;
      forecastsAtBest.resize(numDays-mTrainingDays);
      for(int d = mTrainingDays; d < numDays; d++) {
         int currDate = Global::getDate(mStartDate, 24*d);

         //Global::logger->setDateInfo(currDate, d-mTrainingDays+1, numDays-mTrainingDays);

         std::vector<Slice> slices;
         selector.select(currDate, iInit, iOffset, iLocation, mVariable, par, slices);

         // Get the forecasts at these times
         std::vector<float> temp;
         for(int i = 0; i < (int) slices.size(); i++) {
            // Since the observation dataset is used, past dates may be outside the range
            // TODO: This would be a problem since the selector only picks the n best analogs
            // TODO: Hard coded 10
            if(temp.size() < 10) {
               if(slices[i].getDate() >= mStartDate && slices[i].getDate() <= mEndDate) {
                  float value = inputF->getValue(slices[i].getDate(), slices[i].getInit(),
                           slices[i].getOffset(), iLocation.getId(),
                           slices[i].getMember().getId(), variables[v]);
                  temp.push_back(value);
                  //if(variables[v] == "CloudCover")
                  //   std::cout << "DATES: " << slices[i].getDate() << " " << value << std::endl;
               }
            }
         }
         float fcst = Global::mean(temp); // Forecasts at the best obs
         forecastsAtBest[d-mTrainingDays] = fcst;
      }

      // Compute the score
      float corr = Global::corr(forecasts[currVar], forecastsAtBest);
      if(Global::isValid(corr)) {
         float score = -corr;
         std::pair<std::string, float> p(variables[v], score);
         corrs.push_back(p);
         writeScore(variables[v], corr);
      }
      double endTime = Global::clock();
      //std::cout << "Time = " << endTime - startTime << std::endl;
   }

   // Sort
   std::sort(corrs.begin(), corrs.end(), Global::sort_pair_second<std::string, float>());
   assert(iVariables.size() == 0);

   // Check how many to add
   std::vector<std::string> currVars;
   std::vector<int> currVarsI;
   std::vector<std::string> bestVars;
   float bestScore = Global::INF;
   for(int v = 0; v < mMaxVars; v++) {
      if(v < corrs.size()) {
         std::string currVar = corrs[v].first;
         // Check that current variable isn't correlated to any added variables
         float varCorr = 0;
         for(int vv = 0; vv < currVars.size(); vv++) {
            varCorr = Global::corr(forecasts[currVar], forecasts[currVars[vv]]);
            std::cout << "   varCorr[" << currVar << "," << currVars[vv] << "] = " << varCorr << std::endl;
         }
         if(!Global::isValid(varCorr) || fabs(varCorr) < mMaxVarCrossCorrelation) {
            currVars.push_back(currVar);
            currVarsI.push_back(v);
            float score = VarSelector::run(iData, iInit, iOffset, iLocation, currVars, *mDetMetric);
            std::cout << "Scores:";
            for(int vv = 0; vv < currVars.size(); vv++) {
               std::cout << " " << currVars[vv];
            }
            std::cout << " " << score << std::endl;
            if(score < bestScore) {
               bestScore = score;
               bestVars = currVars;
            }
         }
      }
   }
   iVariables = bestVars;
}

Options VarSelectorFind::makeOptionsObs(std::string iVariable) const {
   std::stringstream ss;
   ss << "tag=bogus ";
   ss << "class=SelectorAnalog obsSet=wfrt.mv-obs analogMetric=normMetric ";
   ss << "numAnalogs=200 averager=mean normalize=1 dayWidth=365 ";
   ss << "variables=";
   ss << iVariable;
   return Options(ss.str());
}
