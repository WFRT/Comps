#include "SchemesHeader.inc"
#include "Corrector.h"
#include "../Global.h"
#include "../Options.h"
#include "../Parameters.h"
#include "../Component.h"
#include "../Data.h"
#include "../Inputs/Input.h"
#include "../Obs.h"
#include "../Ensemble.h"
#include "../Averagers/Averager.h"
#include "../Scheme.h"
#include "../Variables/Variable.h"

Corrector::Corrector(const Options& iOptions, const Data& iData) :
      Component(iOptions, iData),
      mMemberSpecific(false),
      mPoolParameters(false),
      mEnforceLimits(false) { 

   //! Ensure that corrected values are within limits defined by the variable
   iOptions.getValue("enforceLimits", mEnforceLimits);
   //! Run this scheme on each ensemble member separately
   iOptions.getValue("memberSpecific", mMemberSpecific);
   //! Use the same parameters on each ensemble member
   iOptions.getValue("poolParameters", mPoolParameters);

   if(!mMemberSpecific && mPoolParameters) {
      std::stringstream ss;
      ss << "Corrector " << Component::getSchemeName()
         << ": It doesn't make sense to specify 'memberSpecific' and 'poolParameters'";
      Global::logger->write(ss.str(), Logger::warning);
   }
}
#include "Schemes.inc"

void Corrector::getDefaultParameters(Parameters& iParameters) const {
   getDefaultParametersCore(iParameters);
   if(isMemberSpecific()) {
      iParameters.setIsDefault(true);
   }
}
bool Corrector::isMemberSpecific() const {
   return mMemberSpecific;
}

bool Corrector::needsConstantEnsembleSize() const {
   return isMemberSpecific() && needsTraining();
}


void Corrector::enforceLimits(Ensemble& iEnsemble) const {
   const Variable* var = Variable::get(iEnsemble.getVariable());
   float min = var->getMin();
   float max = var->getMax();
   for(int i = 0; i < iEnsemble.size(); i++) {
      if(Global::isValid(min) && iEnsemble[i] < min)
         iEnsemble[i] = min;
      if(Global::isValid(max) && iEnsemble[i] > max)
         iEnsemble[i] = max;
   }
}

void Corrector::correct(const Parameters& iParameters, Ensemble& iEnsemble) const {
   if(!isMemberSpecific()) {
      correctCore(iParameters, iEnsemble);
   }
   else {
      // Correct each member separately
      Parameters currParameters = iParameters;
      int N = iEnsemble.size();

      // Reset parameters if they are new
      if(iParameters.getIsDefault() && !mPoolParameters) {
         std::vector<float> par;
         for(int n = 0; n < N; n++) {
            for(int j = 0; j < currParameters.size(); j++) {
               par.push_back(currParameters[j]);
            }
         }
         currParameters.setAllParameters(par);
         currParameters.setIsDefault(false);
         assert(currParameters.size() % N == 0);
      }

      int P = currParameters.size();
      // Loop over each ensemble member
      for(int n = 0; n < (int) iEnsemble.size(); n++) {
         Parameters parSubset;
         if(!mPoolParameters) {
            assert(currParameters.size() % N == 0); // Each member must have the same number of parameters
            int iStart = n*P/N;
            int iEnd   = (n+1)*P/N-1;
            if(currParameters.size() % N != 0) {
               std::stringstream ss;
               ss << "Corrector: Parameter size (" << currParameters.size() << ") is not a multiple of the number of ensemble members (" << N << "). Does the ensemble change size from one run to the next?";
            }
            // Each member must have the same number of parameters
            currParameters.getSubset(iStart, iEnd, parSubset);
         }
         else {
            parSubset = currParameters;
         }

         float value;
         if(!Global::isValid(iEnsemble[n])) {
            value = Global::MV;
         }
         else {
            Ensemble ens = iEnsemble;
            std::vector<float> ensValues;
            ensValues.push_back(iEnsemble[n]);
            ens.setValues(ensValues);

            correctCore(parSubset, ens);
            value = ens[0];
         }
         assert(!std::isnan(value) && ~std::isinf(value));
         iEnsemble[n] = value;
      }
   }

   if(mEnforceLimits) {
      enforceLimits(iEnsemble);
   }
}
void Corrector::updateParameters(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   assert(iObs.size() == iUnCorrected.size());
   int N = iUnCorrected[0].size();
   if(!isMemberSpecific()) {
      updateParametersCore(iUnCorrected, iObs, iParameters);
   }
   else {
      if(iParameters.getIsDefault() && !mPoolParameters) {
         std::vector<float> par;
         for(int i = 0; i < N; i++) {
            for(int j = 0; j < iParameters.size(); j++) {
               par.push_back(iParameters[j]);
            }
         }
         iParameters.setAllParameters(par);
      }
      int P = iParameters.size();
      std::vector<float> parameters;
      // Loop over members
      for(int i = 0; i < N; i++) {
         Parameters parSubset;
         if(!mPoolParameters) {
            assert(iParameters.size() % N == 0); // Each member must have the same number of parameters
            int iStart = i*P/N;
            int iEnd   = (i+1)*P/N-1;
            iParameters.getSubset(iStart, iEnd, parSubset);
         }
         else {
            parSubset = iParameters;
         }
         std::vector<Ensemble> ens = iUnCorrected;
         // Loop over all cases
         for(int n = 0; n < ens.size(); n++) {
            std::vector<float> ensValues;
            ensValues.push_back(ens[n][i]);
            ens[n].setValues(ensValues);
         }
         updateParametersCore(ens, iObs, parSubset);
         if(!mPoolParameters) {
            for(int j = 0; j < (int) parSubset.size(); j++) {
               parameters.push_back(parSubset[j]);
            }
         }
         else {
            parameters = parSubset.getAllParameters();
         }
      }
      iParameters.setAllParameters(parameters);
   }
   iParameters.setIsDefault(false);
}

