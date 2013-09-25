#include "RecentObs.h"
#include "../Obs.h"
#include "../Data.h"
#include "../Parameters.h"

CorrectorRecentObs::CorrectorRecentObs(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData) {
}
void CorrectorRecentObs::correctCore(const Parameters& iParameters, Ensemble& iEnsemble) const {
   int N = iEnsemble.size();

   // Retrieve recent observation
   Obs obs;
   Location loc = iEnsemble.getLocation();
   std::string var = iEnsemble.getVariable();
   mData.getRecentObs(loc, var, obs);
   float obsValue = obs.getValue();

   if(!Global::isValid(obsValue))
      return;

   int   obsDate  = obs.getDate();
   float obsOffset  = obs.getOffset();
   int   fcstDate = iEnsemble.getDate();
   int   fcstInit = iEnsemble.getInit();
   float fcstOffset = iEnsemble.getOffset();
   float hoursAgo = Global::getTimeDiff(fcstDate, fcstInit, fcstOffset, obsDate, 0, obsOffset);

   // Get ensemble issued at time of observation
   Ensemble recentEns;
   mData.getEnsemble(obsDate, fcstInit, obsOffset, loc, var, Input::typeForecast, recentEns);

   // Adjust each member
   float timeScale = iParameters[0];
   for(int i = 0; i < N; i++) {
      assert(iEnsemble.size() == N);
      float fcst = iEnsemble[i];
      assert(fcst < 200);
      float recentFcst = recentEns[i];
      if(hoursAgo <= 0) {
         iEnsemble[i] = obsValue;
      }
      else {
         if(Global::isValid(fcst) && Global::isValid(recentFcst)) {
            float bias = obsValue - recentFcst;
            assert(hoursAgo < 30);
            float biasWeight = exp(-hoursAgo / timeScale);
            float fcstWeight = 1;
            iEnsemble[i] = fcstWeight * fcst + biasWeight * bias;
            std::cout << fcstWeight << "*" << fcst << " + " << biasWeight << "* " << bias << " (nhours ago = " << hoursAgo << ") = " << iEnsemble[i] << std::endl;
         }
      }
   }
}

void CorrectorRecentObs::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param;
   param.push_back(1); // Timescale
   iParameters.setAllParameters(param);
}

void CorrectorRecentObs::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   float oldTimeScale = iParameters[0];
   int   counter = 0;
   float total = 0;
   for(int i = 0; i < (int) iObs.size(); i++) {
      float obs = iObs[i].getValue();
      float fcst = iUnCorrected[i].getMoment(1);
      if(Global::isValid(obs) && Global::isValid(fcst)) {
         float bias = obs - fcst;
         Location loc = iUnCorrected[i].getLocation();
         std::string var = iUnCorrected[i].getVariable();
         int   fcstInit = iUnCorrected[i].getInit();

         // Check how correlated this bias is to the next ensemble
         int   obsDate    = iObs[i].getDate();
         float obsOffset  = iObs[i].getOffset();
         float nextOffset = 0;

         // Find a nearby offset to check how much the bias changes over time
         std::vector<float> offsets;
         mData.getInput(iUnCorrected[i].getLocation().getDataset())->getOffsets(offsets);

         // Can't use this method when only one offset is available
         if(offsets.size() < 2)
            return;

         // Current offset is the highest offset, use the second last one
         if(obsOffset > offsets[offsets.size()-1]) {
            for(int i = offsets.size()-1; i >= 0; i++) {
               if(offsets[i] < obsOffset) {
                  nextOffset = offsets[i];
                  break;
               }
            }
         }
         // Find offset just before current offset
         else {
            for(int i = 0; i < offsets.size(); i++) {
               if(offsets[i] > obsOffset) {
                  nextOffset = offsets[i];
                  break;
               }
            }
         }
         int nextDate = Global::getDate(obsDate, 0, nextOffset);

         // Get obs/ens for this nearby time
         Ensemble nextEns;
         mData.getEnsemble(nextDate, fcstInit, nextOffset, loc, var, Input::typeForecast, nextEns);
         Obs nextObs;
         mData.getObs(nextDate, fcstInit, nextOffset, loc, var, nextObs);
         float ensMean = nextEns.getMoment(1);

         // TODO:
         // We have no good way to compute the time scale, this is only a rough way
         if(Global::isValid(nextObs.getValue()) && Global::isValid(ensMean)) {
            float nextBias = nextObs.getValue() - ensMean;
            total += fabs(nextBias / bias);
            counter++;
         }
      }
   }
   if(counter > 0) {
      float currTimeScale = total / counter;
      float newTimeScale = combine(oldTimeScale, currTimeScale, counter);
      iParameters[0] = newTimeScale;
      assert(newTimeScale > 0);
   }
}
