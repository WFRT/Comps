#include "Distribution.h"
#include "Calibrators/Calibrator.h"
#include "Updaters/Updater.h"
#include "Obs.h"
#include "Ensemble.h"
#include "Parameters.h"
#include "Averagers/Averager.h"
#include "Uncertainties/Uncertainty.h"
#include "Variables/Variable.h"
Distribution::Distribution(Ensemble iEnsemble, const Averager& iAverager, Parameters iAveragerParameters) :
      mEnsemble(iEnsemble),
      mAverager(iAverager),
      mAveragerParameters(iAveragerParameters) {
}
Distribution::~Distribution() {}

float Distribution::getP0() const {
   const Variable* var = Variable::get(getVariable());
   if(var->isLowerDiscrete()) {
      return getCdf(var->getMin());
   }
   else {
      return 0;
   }
}
float Distribution::getP1() const {
   const Variable* var = Variable::get(getVariable());
   if(var->isUpperDiscrete()) {
      float p1 = getCdf(var->getMin());
      if(Global::isValid(p1))
         return 1-p1;
      else
         return Global::MV;
   }
   else {
      return 0;
   }
}

Ensemble Distribution::getEnsemble(int iSize) const {
   std::vector<float> origValues = mEnsemble.getValues();
   int numEns = iSize;
   if(!Global::isValid(iSize))
      numEns = origValues.size();

   std::vector<float> values(numEns, Global::MV);
   if(origValues.size() != numEns) {
      // Create an ensemble by sampling values from the distribution
      // We have no rank information, so just produce a sorted list of members
      for(int i = 0; i < numEns; i++) {
         float cdf = (float) (i+1)/(numEns+1);
         float value = getInv(cdf);
         values[i] = value;
      }
   }
   else {
      // The requested ensemble is the same size as the base ensemble. We can therefore
      // use the rank information from the base ensemble to ensure they are preserved
      // after calibration. If the base ensemble has missing values, then we need to
      // preserve these missing values.
      std::vector<float> withoutMissing;
      withoutMissing.reserve(origValues.size());
      for(int i = 0; i < origValues.size(); i++) {
         float value = origValues[i];
         if(Global::isValid(value))
            withoutMissing.push_back(value);
      }

      int origSize = origValues.size();
      int numValid = withoutMissing.size();
      // Create an ensemble by sampling values from the distribution
      std::vector<std::pair<float, int> > pairs(numValid); // forecast, ensemble index
      std::vector<float> invs(numValid, Global::MV);
      int count = 0;
      for(int i = 0; i < origSize; i++) {
         if(Global::isValid(origValues[i])) {
            float cdf = (float) (count+1)/(numValid+1);
            float value = getInv(cdf);
            invs[count] = value;
            pairs[count]  = std::pair<float, int>(withoutMissing[count], i);
            count++;
         }
      }
      // Ensemble members should have the same rank as in the raw ensemble
      std::sort(pairs.begin(), pairs.end(), Global::sort_pair_first<float, int>());
      for(int i = 0; i < pairs.size(); i++) {
         int index = pairs[i].second;
         float value = invs[i];
         assert(index >= 0 && index < values.size());
         values[index] = value;
      }
   }
   // Set up the ensemble
   Ensemble ens(values, getVariable());
   ens.setInfo(getDate(), getInit(), getOffset(), getLocation(), getVariable());
   return ens;
}
float Distribution::getDeterministic() const {
   return mAverager.average(*this, mAveragerParameters);
}

DistributionUncertainty::DistributionUncertainty(const Uncertainty& iUncertainty, Parameters iParameters, Ensemble iEnsemble, const Averager& iAverager, Parameters iAveragerParameters) :
      Distribution(iEnsemble, iAverager, iAveragerParameters), 
      mUncertainty(iUncertainty),
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
int DistributionUncertainty::getInit() const {
   return mEnsemble.getInit();
}
float DistributionUncertainty::getOffset() const {
   return mEnsemble.getOffset();
}

DistributionCalibrator::DistributionCalibrator(const Distribution::ptr iUpstream,
      const Calibrator& iCalibrator, Parameters iParameters) : 
      Distribution(iUpstream->getBaseEnsemble(), iUpstream->getAverager(), iUpstream->getAveragerParameters()),
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
   float cdf = mUpstream->getCdf(iX);
   if(Global::isValid(pdf)) {
      float factor = mCalibrator.amplify(cdf, mUpstream, mParameters);
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
int DistributionCalibrator::getInit() const {
   return mUpstream->getInit();
}
float DistributionCalibrator::getOffset() const {
   return mUpstream->getOffset();
}


DistributionUpdater::DistributionUpdater(const Distribution::ptr iUpstream,
      const Updater& iUpdater, Obs iRecentObs, const Distribution::ptr iRecent, Parameters iParameters) :
      Distribution(iUpstream->getBaseEnsemble(), iUpstream->getAverager(), iUpstream->getAveragerParameters()),
      mUpstream(iUpstream),
      mUpdater(iUpdater),
      mRecentObs(iRecentObs),
      mRecent(iRecent),
      mParameters(iParameters) {
}

std::string DistributionUpdater::getVariable() const {
   return mUpstream->getVariable();
}
Location DistributionUpdater::getLocation() const {
   return mUpstream->getLocation();
}
int DistributionUpdater::getDate() const {
   return mUpstream->getDate();
}
int DistributionUpdater::getInit() const {
   return mUpstream->getInit();
}
float DistributionUpdater::getOffset() const {
   return mUpstream->getOffset();
}

float DistributionUpdater::getCdf(float iX)  const{
   float returnValue = Global::MV;
   if(Global::isValid(iX)) {
      float cdf = mUpstream->getCdf(iX);
      if(Global::isValid(cdf)) {
         returnValue = mUpdater.update(cdf, mRecentObs, mRecent, mUpstream, mParameters);
         assert(!Global::isValid(returnValue) || (returnValue >= 0 && returnValue <= 1));
      }
   }
   return returnValue;
}
float DistributionUpdater::getPdf(float iX) const {
   float pdf = mUpstream->getPdf(iX);
   float cdf = mUpstream->getCdf(iX);
   if(Global::isValid(pdf)) {
      float factor = mUpdater.amplify(cdf, mRecentObs, mRecent, mUpstream, mParameters);
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
float DistributionUpdater::getInv(float iCdf) const {
   if(Global::isValid(iCdf)) {
      float x = mUpdater.unUpdate(iCdf, mRecentObs, mRecent, mUpstream, mParameters);
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
float DistributionUpdater::getMoment(int iMoment) const {
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

