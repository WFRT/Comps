#include "Continuous.h"
#include "SchemesHeader.inc"
#include "../Variables/Variable.h"
#include "../Obs.h"

Continuous::Continuous(const Options& iOptions, const Data& iData) :
      Probabilistic(iOptions, iData),
      mInvTol(1e-4) {
   iOptions.getValue("invTol", mInvTol);
}
#include "Schemes.inc"

float Continuous::getCdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   if(mEstimator) {
      // Remove estimator parameters
      Parameters coeffs;
      mEstimator->getCoefficients(iParameters, coeffs);
      return getCdfCore(iX, iEnsemble, coeffs);
   }
   else {
      // TODO:
      //Parameters coeffs;
      //mEstimator->getCoefficients(iParameters, coeffs);
      return getCdfCore(iX, iEnsemble, iParameters);
   }
}
float Continuous::getPdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   if(mEstimator) {
      // Remove estimator parameters
      Parameters coeffs;
      mEstimator->getCoefficients(iParameters, coeffs);
      return getPdfCore(iX, iEnsemble, coeffs);
   }
   else {
      // TODO:
      //Parameters coeffs;
      //mEstimator->getCoefficients(iParameters, coeffs);
      return getPdfCore(iX, iEnsemble, iParameters);
   }
}
float Continuous::getInv(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   if(mEstimator) {
      // Remove estimator parameters
      Parameters coeffs;
      mEstimator->getCoefficients(iParameters, coeffs);
      return getInvCore(iCdf, iEnsemble, coeffs);
   }
   else {
      // TODO:
      //Parameters coeffs;
      //mEstimator->getCoefficients(iParameters, coeffs);
      return getInvCore(iCdf, iEnsemble, iParameters);
   }
}
float Continuous::getMoment(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   if(mEstimator) {
      // Remove estimator parameters
      Parameters coeffs;
      mEstimator->getCoefficients(iParameters, coeffs);
      return getMomentCore(iMoment, iEnsemble, coeffs);
   }
   else {
      // TODO:
      //Parameters coeffs;
      //mEstimator->getCoefficients(iParameters, coeffs);
      return getMomentCore(iMoment, iEnsemble, iParameters);
   }
}

float Continuous::getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   // TODO
   float dx= 0.001;
   float cdfUpper = getCdf(iX+dx, iEnsemble, iParameters);
   float cdfLower = getCdf(iX, iEnsemble, iParameters);
   float pdf = (cdfUpper - cdfLower) / dx;
   return pdf;
}
float Continuous::getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float step    = 10;
   // TODO
   float X       = 5;
   float currCdf = getCdf(X, iEnsemble, iParameters);
   if(!Global::isValid(currCdf))
      return Global::MV;

   int dir       = 0;
   int counter   = 0;

   while(fabs(currCdf - iCdf) > mInvTol) {
      if(currCdf > iCdf) {
         if(dir == 1) {
            step /= 2;
         }
         X = X - step;
         dir = -1;
      }
      else {
         if(dir == -1) {
            step /= 2;
         }
         X = X + step;
         dir = 1;
      }
      // Check that we are not stepping outside the variable's domain
      if(X < Variable::get(iEnsemble.getVariable())->getMin()) {
         X = Variable::get(iEnsemble.getVariable())->getMin();
      }
      if(X > Variable::get(iEnsemble.getVariable())->getMax()) {
         X = Variable::get(iEnsemble.getVariable())->getMax();
      }
      if(!Global::isValid(X))
         return Global::MV;
      if(counter > 1000) {
         std::cout << "Continuous.cpp: Could not converge on CDF target: " << iCdf << " " << X << " " << currCdf << std::endl;
         return X;
      }
      currCdf = getCdf(X, iEnsemble, iParameters);
      if(!Global::isValid(currCdf)) {
         return Global::MV;
      }
      counter++;
   }

   return X;
}
float Continuous::getMomentCore(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   // TODO: Not tested
   const Variable* var = Variable::get(iEnsemble.getVariable());

   float total = 0;

   float minX  = var->getMin();
   float maxX  = var->getMax();
   int   nX    = 1000;
   float dX    = (maxX - minX)/((float) nX);

   float c = 0;
   if(iMoment > 1) {
      // Compute center of moment
      c = getMoment(1, iEnsemble, iParameters);
   }

   for(int i = 0; i < nX; i++) {
      float x = minX + i*dX;
      float pdf = getPdf(x, iEnsemble, iParameters);
      if(!Global::isValid(pdf))
         return Global::MV;

      total += pow(x - c, iMoment) * pdf;
   }
   return total * dX;
}

void Continuous::updateParameters(
      const Ensemble& iEnsemble,
      const Obs& iObs,
      Parameters& iParameters) const {
   const Variable* var = Variable::get(iObs.getVariable());
   // Only update if obs is not on the boundary
   if(Global::isValid(iObs.getValue()) &&
      (!var->isLowerDiscrete() || iObs.getValue() != var->getMin()) &&
      (!var->isUpperDiscrete() || iObs.getValue() != var->getMax())) {
      if(mEstimator) {
         std::vector<Obs> obs;
         obs.push_back(iObs);
         mEstimator->update(iEnsemble, obs, iParameters);
      }
      else {
         updateParametersCore(iEnsemble, iObs, iParameters);
      }
   }
}

void Continuous::updateParametersCore(
      const Ensemble& iEnsemble,
      const Obs& iObs,
      Parameters& iParameters) const {
   std::stringstream ss;
   ss << "This scheme: " << typeid(*this).name() << " requires an estimator" << std::endl;
   Global::logger->write(ss.str(), Logger::error);
}

float Continuous::getLikelihood(float iObs, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float L = getPdfCore(iObs, iEnsemble, iParameters);
   if(L == 0) {
      Global::logger->write("Warning: likelihood=0, reset to small non-zero value.", Logger::warning);
      L = Continuous::mMinPdf;
   }
   return L;
}

void Continuous::getDefaultParameters(Parameters& iParameters) const {
   if(mEstimator) {
      Parameters parScheme;
      getDefaultParametersCore(parScheme);
      Parameters parEstimator;
      mEstimator->getDefaultParameters(parScheme, parEstimator);

      // Combine parameters
      std::vector<float> param;
      int Ns = (int) parScheme.size();
      int Ne = (int) parEstimator.size();
      param.resize(Ns + Ne);
      for(int i = 0; i < Ns;  i++) {
         param[i] = parScheme[i];
      }
      for(int i = 0; i < Ne; i++) {
         param[Ns + i] = parEstimator[i];
      }
      iParameters.setAllParameters(param);
   }
   else {
      getDefaultParametersCore(iParameters);
   }

}
