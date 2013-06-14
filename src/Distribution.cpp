#include "Distribution.h"
#include "Calibrators/Calibrator.h"
#include "Ensemble.h"
#include "Parameters.h"
#include "Uncertainties/Uncertainty.h"
#include "Variables/Variable.h"
Distribution::Distribution() {

}
DistributionUncertainty::DistributionUncertainty(const Uncertainty& iUncertainty,
      Ensemble iEnsemble,
      Parameters iParameters) :
      mUncertainty(iUncertainty),
      mEnsemble(iEnsemble),
      mParameters(iParameters) {
}
float DistributionUncertainty::getCdf(float iX)  const{
   if(Global::isValid(iX)) {
      float value = mUncertainty.getCdf(iX, mEnsemble, mParameters);
      assert(!Global::isValid(value) || (value >= 0 && value <= 1));
      return value;
   }
   else {
      return Global::MV;
   }
}
float DistributionUncertainty::getPdf(float iX) const {
   return mUncertainty.getPdf(iX, mEnsemble, mParameters);
}
float DistributionUncertainty::getInv(float iCdf) const {
   return mUncertainty.getInv(iCdf, mEnsemble, mParameters);
}
float DistributionUncertainty::getMoment(int iMoment) const {
   return mUncertainty.getMoment(iMoment, mEnsemble, mParameters);
}
std::string DistributionUncertainty::getVariable() const {
   return mEnsemble.getVariable();
}
Location DistributionUncertainty::getLocation() const {
   return mEnsemble.getLocation();
}

int DistributionUncertainty::getDate() const {
   return mEnsemble.getDate();
}
float DistributionUncertainty::getOffset() const {
   return mEnsemble.getOffset();
}

DistributionCalibrator::DistributionCalibrator(const Distribution::ptr iUpstream,
      const Calibrator& iCalibrator, Parameters iParameters) : 
      mParameters(iParameters),
      mUpstream(iUpstream),
      mCalibrator(iCalibrator) {

}

float DistributionCalibrator::getCdf(float iX)  const{
   float returnValue = Global::MV;
   if(Global::isValid(iX)) {
      float cdf = mUpstream->getCdf(iX);
      if(Global::isValid(cdf)) {
         returnValue = mCalibrator.calibrate(cdf, mUpstream, mParameters);
         assert(!Global::isValid(returnValue) || (returnValue >= 0 && returnValue <= 1));
      }
   }
   return returnValue;
}
float DistributionCalibrator::getPdf(float iX) const {
   float pdf = mUpstream->getPdf(iX);
   if(Global::isValid(pdf)) {
      float factor = mCalibrator.amplify(pdf, mUpstream, mParameters);
      if(!Global::isValid(factor)) {
         return Global::MV;
      }
      else {
         return pdf * factor;
      }
   }
   else {
      return Global::MV;
   }
}
float DistributionCalibrator::getInv(float iCdf) const {
   if(Global::isValid(iCdf)) {
      float x = mCalibrator.unCalibrate(iCdf, mUpstream, mParameters);
      if(Global::isValid(x)) {
         assert(x >= 0 && x <= 1);
         return mUpstream->getInv(x);
      }
      else {
         return Global::MV;
      }
   }
   else {
      return Global::MV;
   }
}
float DistributionCalibrator::getMoment(int iMoment) const {
   // Compute numerically
   // TODO:
   float integral = 0;
   float dx = 1;
   int N = 10;
   //integrate 
   for(int i = 0 ; i < N; i++) {
      float x = 10*i;
      float cdf = getCdf(x);
      if(!Global::isValid(cdf))
         return Global::MV;
      integral += pow(x,iMoment)*cdf;
   }
   return integral * dx;
}

std::string DistributionCalibrator::getVariable() const {
   return mUpstream->getVariable();
}
Location DistributionCalibrator::getLocation() const {
   return mUpstream->getLocation();
}
int DistributionCalibrator::getDate() const {
   return mUpstream->getDate();
}
float DistributionCalibrator::getOffset() const {
   return mUpstream->getOffset();
}

