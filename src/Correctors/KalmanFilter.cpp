#include "KalmanFilter.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorKalmanFilter::CorrectorKalmanFilter(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData) {
   iOptions.getRequiredValue("ratio", mRatio);
}
void CorrectorKalmanFilter::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {

   //float currBiasEstimate = computeBiasEstimate(iParameters);
   float kalmanGain              = iParameters[4];
   float yesterdaysError         = iParameters[5];
   float yesterdaysBiasEstimate  = iParameters[7];
   float todaysBiasEstimate;
   assert(Global::isValid(yesterdaysBiasEstimate));
   assert(Global::isValid(kalmanGain));
   if(Global::isValid(yesterdaysError)) {
      todaysBiasEstimate = yesterdaysBiasEstimate + kalmanGain*(yesterdaysError - yesterdaysBiasEstimate);
   }
   else {
      todaysBiasEstimate = yesterdaysBiasEstimate;
   }
   for(int i = 0; i < iUnCorrected.size(); i++) {
      if(Global::isValid(iUnCorrected[i]))
         iUnCorrected[i] = iUnCorrected[i] - todaysBiasEstimate;
   }
}

void CorrectorKalmanFilter::getDefaultParametersCore(Parameters& iParameters) const {
   float pVarV             = 1000;
   float kalmanGainVar     = 0;
   float varV              = 1;
   float p                 = 1;
   float kalmanGain        = 0;
   float lastError         = 0;
   float previousLastError = 0;
   float biasEstimate      = 0;

   std::vector<float> param;
   param.push_back(pVarV);
   param.push_back(kalmanGainVar);
   param.push_back(varV);
   param.push_back(p);
   param.push_back(kalmanGain);
   param.push_back(lastError);
   param.push_back(previousLastError);
   param.push_back(biasEstimate);
   
   iParameters.setAllParameters(param);
}

void CorrectorKalmanFilter::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   if(iObs.size() > 1) {
      std::stringstream ss;
      ss << "CorrectorKalmanFilter not implemented to update parameters for multiple obs/ens pairs";
      Global::logger->write(ss.str(), Logger::error);
   }
   assert(iParameters.size() == 8);
   for(int i = 0; i < (int) iObs.size(); i++) {
      float obs = iObs[i].getValue();
      float forecast = iUnCorrected[i].getMoment(1);
      float pVarV             = iParameters[0];
      float kalmanGainVar     = iParameters[1];
      float varV              = iParameters[2];
      float p                 = iParameters[3];
      float kalmanGain        = iParameters[4];
      float lastError         = iParameters[5];
      float previousLastError = iParameters[6];
      float biasEstimate      = iParameters[7];

      assert(pVarV > 0);
      assert(varV > 0);
      assert(p > 0);
      assert(kalmanGainVar >= 0 && kalmanGainVar <= 1);
      assert(kalmanGain    >= 0 && kalmanGain    <= 1);

      // Update
      float error = Global::MV;
      if(Global::isValid(obs) && Global::isValid(forecast)) {
         error = forecast - obs;
         // Set currPvarV currKalmanGainVar, currVarV
         if(Global::isValid(lastError)) {
            assert(pVarV + mVarVarW + mVarVarV > 0);

            kalmanGainVar = (pVarV + mVarVarW) / (pVarV + mVarVarW + mVarVarV);
            pVarV         = (pVarV + mVarVarW) * (1 - kalmanGainVar);

            varV = varV + kalmanGainVar*(pow((error - lastError),2)/(2 + mRatio) - varV);
         }
         float varW = mRatio * varV;

         kalmanGain = (p + varW) / (p + varW + varV);
         p = (p + varW)*(1 - kalmanGain);
         assert(p + varW + varV > 0);

         biasEstimate = biasEstimate + kalmanGain*(error - biasEstimate);
      }
      else {
         error = Global::MV;
         p = p + mRatio * varV;

      }

      if(p > CorrectorKalmanFilter::mMaxP) {
         p = CorrectorKalmanFilter::mMaxP;
      }

      assert(pVarV > 0);
      assert(varV > 0);
      assert(p > 0);

      // Set new parameters
      std::vector<float> param;
      param.push_back(pVarV);
      param.push_back(kalmanGainVar);
      param.push_back(varV);
      param.push_back(p);
      param.push_back(kalmanGain);
      param.push_back(error);
      param.push_back(lastError);
      param.push_back(biasEstimate);

      //std::cout << "Setting: " << kalmanGain << std::endl;

      iParameters.setAllParameters(param);
   }
}
