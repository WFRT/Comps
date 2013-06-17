#include "PolynomialRegression.h"
#include "../Data.h"
#include "../Obs.h"
#include "../Parameters.h"
CorrectorPolynomialRegression::CorrectorPolynomialRegression(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData) {
   iOptions.getRequiredValues("useOrders", mUseOrders);
   mOrder = mUseOrders.size() - 1;

   /*
   if(!iOptions.getValue("noOffset", mIsNoOffset)) {
      mIsNoOffset = false;
   }
   if(mIsNoOffset && mOrder == 0) {
      Global::logger->write("CorrectorPolynomialRegression: 'noOffset' cannot be specified when 'order' is 0", Logger::error);
   }
   */
   assert(mOrder <= 1);
   assert(((mOrder == 0) && (mUseOrders[0])) || ((mOrder == 1) && (mUseOrders[0] || mUseOrders[1])));
}
void CorrectorPolynomialRegression::correctCore(const Parameters& iParameters,
      Ensemble& iUnCorrected) const {
   std::vector<float> coeffs;
   computeCoefficients(iParameters, coeffs);
   assert((int) coeffs.size() == mOrder+1);

   for(int n = 0; n < iUnCorrected.size(); n++) {
      float fcst = iUnCorrected[n];
      if(Global::isValid(fcst)) {
         float correctedValue = 0;
         //std::cout << "Coeffs: " << coeffs[0] << " " << coeffs[1] << std::endl;
         for(int i = 0; i < (int) mOrder+1; i++) {
            correctedValue += coeffs[i] * pow((double) fcst, (double) i);
         }
         iUnCorrected[n] = correctedValue;
      }
   }
}

void CorrectorPolynomialRegression::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> coeffs;
   coeffs.push_back(0);
   coeffs.push_back(0);
   coeffs.push_back(1);
   coeffs.push_back(1);
   iParameters.setAllParameters(coeffs);
}

void CorrectorPolynomialRegression::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   float oldMeanObs = iParameters[0];
   float oldMeanForecast = iParameters[1];
   float oldMeanForecast2 = iParameters[2];
   float oldMeanObsForecast = iParameters[3];
   float currMeanObs = 0;
   float currMeanForecast = 0;
   float currMeanForecast2 = 0;
   float currMeanObsForecast = 0;
   int   counter = 0;
   for(int i = 0; i < (int) iObs.size(); i++) {
      float obs = iObs[i].getValue();
      float fcst = iUnCorrected[i].getMoment(1);
      if(obs != Global::MV && fcst != Global::MV) {
         currMeanObs += obs;
         currMeanForecast += fcst;
         currMeanForecast2 += fcst*fcst;
         currMeanObsForecast += obs*fcst;
         counter++;
      }
   }
   if(counter > 0) {
      // Only update parameters if we have valid observations and forecasts
      iParameters[0] = combine(oldMeanObs, currMeanObs/counter, counter);
      iParameters[1] = combine(oldMeanForecast, currMeanForecast/counter, counter);
      iParameters[2] = combine(oldMeanForecast2, currMeanForecast2/counter, counter);
      iParameters[3] = combine(oldMeanObsForecast, currMeanObsForecast/counter, counter);
      //std::cout << "Obs[" << iOffset << "]= " << oldMeanObs << " " << currMeanObs << " " << iParameters[0] << std::endl;
      //std::cout << "Fcs[" << iOffset << "]= " << oldMeanForecast << " " << currMeanForecast << " " << iParameters[1] << std::endl;
   }
}
void CorrectorPolynomialRegression::computeCoefficients(const Parameters& iParameters,
      std::vector<float>& iCoefficients) const {
   float meanObs = iParameters[0];
   float meanForecast = iParameters[1];
   float meanForecast2 = iParameters[2];
   float meanObsForecast = iParameters[3];

   if(mOrder == 0) {
      float slope = meanObs - meanForecast;
      iCoefficients.push_back(slope);
   }
   else if(mOrder == 1) {
      float slope;
      float offset;
      if(!mUseOrders[0] && mUseOrders[1]) {
         // Only use slope
         slope = meanObsForecast / meanForecast2;
         offset = 0;
      }
      else if(mUseOrders[0] && !mUseOrders[1]) {
         // Only use offset
         slope = 1;
         offset = meanObs - meanForecast;
         //std::cout << "Offset[]= " << meanObs << " " << meanForecast << " " << offset << std::endl;
      }
      else if(meanForecast2 - meanForecast*meanForecast == 0) {
         slope = 1;
         offset = 0;
         std::cout << "CorrectorPolynomialRegression: Warning: Denominator 0: " << meanForecast2 << " " << meanForecast << std::endl;
      }
      else {
         slope = (meanObsForecast - meanForecast*meanObs)/(meanForecast2 - meanForecast*meanForecast);
         offset = meanObs - slope*meanForecast;
      }
      iCoefficients.push_back(offset);
      iCoefficients.push_back(slope);
   }
}
