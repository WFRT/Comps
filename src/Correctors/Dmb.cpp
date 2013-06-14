#include "Dmb.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorDmb::CorrectorDmb(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData) {
}
void CorrectorDmb::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   float value0 = iParameters[0];
   for(int n = 0; n < iUnCorrected.size(); n++) {
      if(Global::isValid(iUnCorrected[n])) {
         iUnCorrected[n] = iUnCorrected[n]/value0;
      }
   }
}

void CorrectorDmb::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param(1);
   param[0] = 1;
   iParameters.setAllParameters(param);
}

void CorrectorDmb::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {

   float oldBias = iParameters[0];
   float currBias = 0;
   float totalObs = 0;
   float totalFcst = 0;
   int counter = 0;
   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[i].getValue();
      float fcst = iUnCorrected[i].getMoment(1);

      if(Global::isValid(fcst) && Global::isValid(obs)) {
         totalObs += obs;
         totalFcst += fcst;
         counter++;
      }
   }
   if(counter > 0) {
      if(totalObs == 0) {
         std::stringstream ss;
         ss << "CorrectorDmb: obs (" << iObs[0].getDate() << " " << iObs[0].getOffset() << " "
            << " at location " << iObs[0].getLocation().getId() << ") is 0, cannot update parameters";
         Global::logger->write(ss.str(), Logger::message);
      }
      else {
         float currBias = totalFcst / totalObs / counter;
         float newBias = combine(oldBias, currBias, counter);
         iParameters[0] = newBias;
      }
   }
}
