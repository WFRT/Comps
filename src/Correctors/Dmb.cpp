#include "Dmb.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorDmb::CorrectorDmb(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData),
      mFixedDivisor(Global::MV) {
   //! Divide forecasts by this fixed divisor
   iOptions.getValue("fixedDivisor", mFixedDivisor);
}
void CorrectorDmb::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   float meanObs  = iParameters[0];
   float meanFcst = iParameters[1];
   for(int n = 0; n < iUnCorrected.size(); n++) {
      if(Global::isValid(iUnCorrected[n]) && meanFcst > 0) {
         iUnCorrected[n] = iUnCorrected[n]*meanObs/meanFcst;
      }
   }
}

void CorrectorDmb::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param(2);
   param[0] = 1;
   if(Global::isValid(mFixedDivisor))
      param[1] = mFixedDivisor;
   else
      param[1] = 1;
   iParameters.setAllParameters(param);
}

void CorrectorDmb::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   if(!Global::isValid(mFixedDivisor)) {
      float oldMeanObs  = iParameters[0];
      float oldMeanFcst = iParameters[1];
      float totalObs  = 0;
      float totalFcst = 0;
      int counter = 0;
      for(int i = 0; i < iObs.size(); i++) {
         float obs  = iObs[i].getValue();
         float fcst = iUnCorrected[i].getMoment(1);

         if(Global::isValid(fcst) && Global::isValid(obs)) {
            totalObs  += obs;
            totalFcst += fcst;
            counter++;
         }
      }
      if(counter > 0) {
         if(totalFcst < 0 || totalObs < 0) {
            std::stringstream ss;
            ss << "CorrectorDmb: fcst or obs at location " << iObs[0].getLocation().getId() << " is less than, cannot update parameters";
            Global::logger->write(ss.str(), Logger::message);
         }
         else {
            iParameters[0] = combine(oldMeanObs,  totalObs/counter, counter);
            iParameters[1] = combine(oldMeanFcst, totalFcst/counter, counter);
         }
      }
   }
}
