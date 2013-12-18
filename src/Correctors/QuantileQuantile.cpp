#include "QuantileQuantile.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorQuantileQuantile::CorrectorQuantileQuantile(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData) ,
      mMaxPoints(100) {
   //! Maximum number of quantile-pairs to store in parameters
   iOptions.getValue("maxPoints", mMaxPoints);
}
void CorrectorQuantileQuantile::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   // Parameters haven't been computed yet
   if(iParameters.size() == 0) {
      return;
   }

   // Get sorted quantiles
   std::vector<float> obsQuantiles;
   std::vector<float> fcstQuantiles;
   getSortedQuantiles(iParameters, obsQuantiles, fcstQuantiles);

   // Correct each ensemble member
   for(int n = 0; n < iUnCorrected.size(); n++) {
      float fcst = iUnCorrected[n];
      if(Global::isValid(fcst)) {
         int index = Global::MV;
         // Find which quantile the forecast is in
         for(int i = 0; i < fcstQuantiles.size(); i++) {
            if(fcst < fcstQuantiles[i]) {
               index = i;
               break;
            }
         }
         if(Global::isValid(index) & index > 0) {
            float lowerObs  = obsQuantiles[index-1];
            float upperObs  = obsQuantiles[index];
            float lowerFcst = fcstQuantiles[index-1];
            float upperFcst = fcstQuantiles[index];
            assert(Global::isValid(lowerObs) & Global::isValid(upperObs));
            assert(Global::isValid(lowerFcst) & Global::isValid(upperFcst));
            if(lowerFcst == upperFcst)
               iUnCorrected[n] = (lowerObs + upperObs)/2;
            else
               // Linear interpolation
               iUnCorrected[n] = lowerObs + (upperObs-lowerObs)*(fcst - lowerFcst)/(upperFcst - lowerFcst);
         }
         else {
            // Forecast is outside the range of previously seen forecasts.
            // Fix them to the lower or upper boundary:
            if(index == 0) {
               // Fcst below all previous fcsts
               iUnCorrected[n] = obsQuantiles[0];
            }
            else {
               // Fcst above all previous fcsts
               iUnCorrected[n] = obsQuantiles[obsQuantiles.size()-1];
            }
         }
      }
   }
}

void CorrectorQuantileQuantile::getDefaultParametersCore(Parameters& iParameters) const {
   // Start with no quantile-pairs
}

void CorrectorQuantileQuantile::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   // Store fcst and obs quantiles as pairs: (obs1, fcst1, obs2, fcst2, ...)
   std::vector<float> param = iParameters.getAllParameters();

   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[i].getValue();
      float fcst = iUnCorrected[i].getMoment(1);

      // Add quantiles
      if(Global::isValid(fcst) && Global::isValid(obs)) {
         param.push_back(obs);
         param.push_back(fcst);
      }
   }

   // Ensure that the set of parameters isn't overfilled
   if(param.size()/2 <= mMaxPoints)
      iParameters.setAllParameters(param);
   else {
      // Remove the earliest quantile-pairs: Only use the last mMaxPoints pairs
      std::vector<float> paramTrunc;
      paramTrunc.resize(mMaxPoints*2);
      int start = param.size() - mMaxPoints*2;
      for(int i = start; i < param.size(); i++) {
         paramTrunc[i - start] = (param[i]);
      }
      assert(paramTrunc.size() == mMaxPoints*2);
      iParameters.setAllParameters(paramTrunc);
      assert(iParameters.size() % 2 == 0);
   }
   assert(iParameters.size() % 2 == 0);
}

void CorrectorQuantileQuantile::getSortedQuantiles(const Parameters& iParameters, std::vector<float>& iObsQuantiles, std::vector<float>& iFcstQuantiles) {
   assert(iParameters.size() % 2 == 0);
   for(int i = 0; i < iParameters.size(); i = i + 2) {
      iObsQuantiles.push_back(iParameters[i]);
      iFcstQuantiles.push_back(iParameters[i+1]);
   }
   std::sort(iObsQuantiles.begin(), iObsQuantiles.end());
   std::sort(iFcstQuantiles.begin(), iFcstQuantiles.end());
}
