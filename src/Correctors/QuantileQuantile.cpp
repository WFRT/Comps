#include "QuantileQuantile.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorQuantileQuantile::CorrectorQuantileQuantile(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData) ,
      mMaxPoints(100),
      mCorrectExtremes(0) {
   //! Maximum number of quantiles to store in parameters
   iOptions.getValue("maxPoints", mMaxPoints);
   //! Don't allow forecasts to go outside previously seen quantiles. Will only occur if the number
   //! of quantiles recorded equals maxPoints
   iOptions.getValue("correctExtremes", mCorrectExtremes);
}
void CorrectorQuantileQuantile::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {

   // Get sorted quantiles
   std::vector<float> obsQuantiles;
   std::vector<float> fcstQuantiles;
   getSortedQuantiles(iParameters, obsQuantiles, fcstQuantiles);

   //for(int i = 0; i < obsQuantiles.size(); i++) {
   //   std::cout << obsQuantiles[i] << " " << fcstQuantiles[i] << std::endl;
   //}

   for(int n = 0; n < iUnCorrected.size(); n++) {
      float fcst = iUnCorrected[n];
      if(Global::isValid(fcst)) {
         int index = Global::MV;
         for(int i = 0; i < fcstQuantiles.size(); i++) {
            if(fcst < fcstQuantiles[i]) {
               break;
            }
         }
         if(Global::isValid(index) & index > 0) {
            float lowerObs  = obsQuantiles[index];
            float upperObs  = obsQuantiles[index+1];
            float lowerFcst = fcstQuantiles[index];
            float upperFcst = fcstQuantiles[index+1];
            assert(Global::isValid(lowerObs) & Global::isValid(upperObs));
            assert(Global::isValid(lowerFcst) & Global::isValid(upperFcst));
            if(lowerFcst == upperFcst)
               iUnCorrected[n] = (lowerObs + upperObs)/2;
            else
               // Linear interpolation
               iUnCorrected[n] = lowerObs + (upperObs-lowerObs)*(fcst - lowerFcst)/(upperFcst - lowerFcst);
         }
         else if(iParameters.size() == mMaxPoints && mCorrectExtremes) {
            // By default forecasts outside the range of forecasts found in the past are left the
            // way they are. Otherwise fix them to the lower or upper boundary:
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
   std::vector<float> param(2, 0);
   iParameters.setAllParameters(param);
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

   // Ensure size restrictions
   if(param.size()/2 <= mMaxPoints)
      iParameters.setAllParameters(param);
   else {
      // Only use the last mMaxPoints pairs
      std::vector<float> paramTrunc;
      paramTrunc.resize(mMaxPoints);
      int start = param.size() - mMaxPoints*2;
      for(int i = start; i < param.size(); i++) {
         paramTrunc.push_back(param[i]);
      }
      assert(paramTrunc.size() == mMaxPoints*2);
      iParameters.setAllParameters(paramTrunc);
   }
   assert(iParameters.size() > 0);
}

void CorrectorQuantileQuantile::getSortedQuantiles(const Parameters& iParameters, std::vector<float>& iObsQuantiles, std::vector<float>& iFcstQuantiles) const {
   assert(iParameters.size() % 2 == 0);
   for(int i = 0; i < iParameters.size()/2; i = i + 2) {
      iObsQuantiles.push_back(iParameters[i]);
      iFcstQuantiles.push_back(iParameters[i+1]);
   }
   std::sort(iObsQuantiles.begin(), iObsQuantiles.end());
   std::sort(iFcstQuantiles.begin(), iFcstQuantiles.end());
}
