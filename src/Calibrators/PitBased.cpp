#include "PitBased.h"
#include "../Options.h"
#include "../Parameters.h"
#include "../Variables/Variable.h"
#include "../Obs.h"
#include "../Scheme.h"
#include "../Interpolators/Interpolator.h"

CalibratorPitBased::CalibratorPitBased(const Options& iOptions, const Data& iData) : Calibrator(iOptions, iData),
      mDoDiscrete(false),
      mInteliCal(false),
      mInteliCalFactor(1) {
   //Component::underDevelopment();
   iOptions.getRequiredValue("numSmooth", mNumSmooth);
   iOptions.getValue("inteliCalFactor", mInteliCalFactor);
   iOptions.getValue("inteliCal", mInteliCal);

   std::string interpolatorTag;
   iOptions.getRequiredValue("interp", interpolatorTag);
   Options optInterpolator;
   Scheme::getOptions(interpolatorTag, optInterpolator);
   mInterpolator = Interpolator::getScheme(optInterpolator, iData);


   /*
   iOptions.getValue("doDiscrete", mDoDiscrete);
   if(!iOptions.getValue("numSmooth0", mNumSmooth0)) {
      if(mDoDiscrete)
         mNumSmooth0 = mNumSmooth;
      else
         mNumSmooth0 = 0;
   }
   if(!iOptions.getValue("numSmooth1", mNumSmooth1)) {
      if(mDoDiscrete) 
         mNumSmooth1 = mNumSmooth;
      else
         mNumSmooth1 = 0;
   }
   */
   mPit.push_back(0);
   for(int i = 1; i <= mNumSmooth; i++) {
      mPit.push_back((float) i / (mNumSmooth + 1));
   }
   mPit.push_back(1);
   /*
   if(mDoDiscrete) {
      // pit0 denotes the upper boundary of a box
      for(int i = 1; i <= mNumSmooth0; i++) {
         mPit0.push_back((float) i / (mNumSmooth0));
      }
      assert(mPit0.back() == 1);
      for(int i = 1; i <= mNumSmooth1; i++) {
         mPit1.push_back((float) i / (mNumSmooth1));
      }
   }
   */
}
CalibratorPitBased::~CalibratorPitBased() {
   delete mInterpolator;
}

float CalibratorPitBased::calibrate(float iCdf, const Distribution::ptr iDist,  const Parameters& iParameters) const {
   float iP0 = 0;
   float iP1 = 0;
   const Variable* var = Variable::get(iDist->getVariable());
   if(var->isLowerDiscrete()) {
      iP0 = iDist->getCdf(var->getMin());
   }
   if(var->isLowerDiscrete()) {
      iP1 = iDist->getCdf(var->getMax());
   }
   if(!Global::isValid(iCdf) || !Global::isValid(iP0) || !Global::isValid(iP1)) {
      return Global::MV;
   }

   std::vector<float> phi = iParameters.getAllParameters();
   if(mInteliCal) {
      if(isFlatEnough(phi)) {
         return iCdf;
      }
   }
   /*// We can never be outside the boundaries
   assert(iCdf >= iP0);
   assert(iCdf <= iP1);
   */

   // Don't calibrate discrete
   if(iCdf <= iP0) {
      return iCdf;
   }
   if(iCdf >= 1-iP1) {
      return iCdf;
   }

   //std::map<Type, Parameters> parMap;
   //subsetParameters(iParameters, parMap);

   if(iCdf == 0 || iCdf == 1)
      return iCdf;

   //std::vector<float> phi = parMap[typeContinuous];
   /*
   int numSmooth = phi.size();
   std::vector<float> phi0 = parMap[typeLower];
   int numSmooth0 = phi0.size();
   std::vector<float> phi1 = parMap[typeUpper];
   int numSmooth1 = phi1.size();
   */

   float cdfCont = (iCdf - iP0) / (1 - iP0 - iP1);
   float cdfContCal = mInterpolator->interpolate(cdfCont, mPit, phi);
   float cdfCal = iP0 + (cdfContCal)*(1 - iP0 - iP1);
   if(cdfCal < 0 || cdfCal > 1) {
      std::stringstream ss;
      ss << "Calibrator mName produced a calibrated cdf outside the range [0, 1]. ";
      if(mInterpolator->isMonotonic())
         ss << "This may be because the interpolator " << mInterpolator->getSchemeName() << " is not monotonic";
      else
         ss << "The interpolator " << mInterpolator->getSchemeName() << " claims to be monotonic, but isn't?";
      Global::logger->write(ss.str(), Logger::error);
   }
   return cdfCal;
   //return interp(iCdf, phi);
}
float CalibratorPitBased::unCalibrate(float iCdf, const Distribution::ptr iDist,  const Parameters& iParameters) const {
   double s = Global::clock();
   float iP0 = 0;
   float iP1 = 0;
   const Variable* var = Variable::get(iDist->getVariable());
   if(var->isLowerDiscrete()) {
      iP0 = iDist->getCdf(var->getMin());
   }
   if(var->isUpperDiscrete()) {
      iP1 = iDist->getCdf(var->getMax());
   }
   if(!Global::isValid(iCdf) || !Global::isValid(iP0) || !Global::isValid(iP1)) {
      return Global::MV;
   }

   std::vector<float> phi = iParameters.getAllParameters();
   if(mInteliCal) {
      if(isFlatEnough(phi)) {
         return iCdf;
      }
   }

   // Don't calibrate discrete
   if(iCdf <= iP0) {
      return iCdf;
   }
   if(iCdf >= 1-iP1) {
      return iCdf;
   }


   if(iCdf == 0 || iCdf == 1)
      return iCdf;

   // Reverse of equations in calibrate
   float cdfContCal = (iCdf - iP0)/(1 - iP0 - iP1);
   float cdfCont = mInterpolator->interpolate(cdfContCal, phi, mPit);
   float cdf = cdfCont * (1 - iP0 - iP1) + iP0;
   double e = Global::clock();
   //std::cout << "CalibratorPitBased::uncalibrate: " << e - s << std::endl;
   if(cdf < 0 || cdf > 1) {
      std::stringstream ss;
      ss << "Calibrator mName produced an inverseCdf outside the range [0, 1]. ";
      if(!mInterpolator->isMonotonic())
         ss << "This may be because the interpolator " << mInterpolator->getSchemeName() << " is not monotonic";
      else
         ss << "The interpolator " << mInterpolator->getSchemeName() << " claims to be monotonic, but isn't?";
      Global::logger->write(ss.str(), Logger::error);
   }
   return cdf;
}

float CalibratorPitBased::amplify(float iCdf, const Distribution::ptr iDist, const Parameters& iParameters) const {
   float iP0 = 0;
   float iP1 = 0;
   const Variable* var = Variable::get(iDist->getVariable());
   if(var->isLowerDiscrete()) {
      iP0 = iDist->getCdf(var->getMin());
   }
   if(var->isLowerDiscrete()) {
      iP1 = iDist->getCdf(var->getMax());
   }
   // Don't calibrate discrete
   if(iCdf <= iP0) {
      return 1;
   }
   if(iCdf >= 1-iP1) {
      return 1;
   }

   // InteliCal check
   std::vector<float> phi = iParameters.getAllParameters();
   if(mInteliCal) {
      if(isFlatEnough(phi)) {
         return 1;
      }
   }

   float cdfCont = (iCdf - iP0) / (1 - iP0 - iP1);
   float factor = mInterpolator->slope(cdfCont, mPit, phi);
   return factor;

}

void CalibratorPitBased::getDefaultParameters(Parameters& iParameters) const {
   std::vector<float> param;
   //param.push_back(mNumSmooth);
   for(int i = 0; i < mPit.size(); i++) {
      param.push_back(mPit[i]);
   }
   /*
   param.push_back(mNumSmooth0);
   for(int i = 0; i < mNumSmooth0; i++) {
      phi0.push_back(mPit0[i]);
   }
   param.push_back(mNumSmooth1);
   for(int i = 0; i < mNumSmooth1; i++) {
      phi1.push_back(mPit1[i]);
   }
   */
   iParameters.setAllParameters(param);
}

void  CalibratorPitBased::updateParameters(const std::vector<Distribution::ptr> iDist, const std::vector<Obs>& iObs, Parameters& iParameters) const {
   if(iObs.size() > 1) {
      std::stringstream ss;
      ss << "CalibratorPitBased cannot update parameters for multiple obs/ens values";;
      Global::logger->write(ss.str(), Logger::error);
   }

   float obs = iObs[0].getValue();
   Distribution::ptr dist = iDist[0];
   if(!Global::isValid(obs)) {
      std::vector<float> phi = iParameters.getAllParameters();
      if(phi[0] != 0 || phi[phi.size()-1] != 1) {
         // Older parameters did not include the end points.
         // Lets add them if this is the case
         // Should remove this code at some point when no one is using the old format
         std::vector<float> par;
         if(phi[0] != 0)
            par.push_back(0);
         for(int i = 0; i < phi.size(); i++) {
            par.push_back(phi[i]);
         }
         if(phi[phi.size()-1] != 1)
            par.push_back(1);
         iParameters.setAllParameters(par);
      }
      return;
   }

   float iP0 = 0;
   float iP1 = 0;
   const Variable* var = Variable::get(dist->getVariable());
   if(var->isLowerDiscrete()) {
      iP0 = dist->getCdf(var->getMin());
   }
   if(var->isLowerDiscrete()) {
      iP1 = dist->getCdf(var->getMax());
   }
   float iCdf = dist->getCdf(obs);
   //std::map<Type, Parameters> parMap;
   //subsetParameters(iParameters, parMap);

   std::vector<float> phi = iParameters.getAllParameters();
   int numPoints = phi.size();
   if(iCdf > iP0 && iCdf < 1-iP1) {
      // Only adjust interior points
      for(int i = 1; i < numPoints-1; i++) {
         float tempPit;
         float currPhi = mPit[i];
         // Determine interior pit
         tempPit = (iCdf - iP0)/(1 - iP0 - iP1);
         // Update
         if(tempPit < currPhi)
            phi[i] = combine(phi[i], 1);
         else
            phi[i] = combine(phi[i], 0);
         if(i > 0)
            assert(phi[i] >= phi[i-1]);
         //cout << "updateing: " << newpit << endl;
      }
   }
   if(phi[0] != 0 || phi[phi.size()-1] != 1) {
      // Older parameters did not include the end points.
      // Lets add them if this is the case
      // Should remove this code at some point when no one is using the old format
      std::vector<float> par;
      if(phi[0] != 0)
         par.push_back(0);
      for(int i = 0; i < phi.size(); i++) {
         par.push_back(phi[i]);
      }
      if(phi[phi.size()-1] != 1)
         par.push_back(1);
      iParameters.setAllParameters(par);
   }
   else {
      iParameters.setAllParameters(phi);
   }
}


void CalibratorPitBased::subsetParameters(const Parameters& iAllParameters, std::map<Type, Parameters>& iParameterMap) const {
   /*
   // Continuous parameters
   int Ncont = iAllParameters[0];
   assert(iAllParameters.size() > Ncont);
   std::vector<float> paramCont;
   paramCont.resize(Ncont);
   for(int i = 0; i < Ncont; i++) {
      paramCont[i] = iAllParameters[i+1];
   }
   iParameterMap[typeContinuous] = Parameters(paramCont);

   int ptr = Ncont + 1; // points to beginning of next record
   
   // Lower discrete parameters
   assert(iAllParameters.size() > ptr);
   std::vector<float> param;
   int N = iAllParameters[ptr];
   assert(iAllParameters.size() > ptr + N);
   param.resize(N);
   for(int i = 0; i < N; i++) {
      param[i] = iAllParameters[ptr + 1 + i];
   }
   iParameterMap[typeLower] = Parameters(param);
   ptr = ptr + N + 1;

   // Upper discrete parameters
   assert(iAllParameters.size() > ptr);
   std::vector<float> param;
   int N = iAllParameters[ptr];
   assert(iAllParameters.size() > ptr + N);
   param.resize(N);
   for(int i = 0; i < N; i++) {
      param[i] = iAllParameters[ptr + 1 + i];
   }
   iParameterMap[typeUpper] = Parameters(param);
   */
}

void CalibratorPitBased::packageParameters(const std::map<Type, Parameters>& iParameterMap, Parameters& iAllParameters) const {
   /*
   std::map<Type, Parameters>::const_iterator it = iParameterMap.find(typeContinuous);
   assert(it != iParameterMap.end());
   Parameters parCont = it->second;
   std::vector<float> param;
   // Add size
   int N = parCont.size();
   param.push_back(N);
   for(int i = 0; i < N; i++) {
      param.push_back(parCont[i]);
   }

   it = iParameterMap.find(typeLower);
   assert(it != iParameterMap.end());
   Parameters par = it->second;
   int N = par.size();
   // Add size
   param.push_back(N);
   for(int i = 0; i < N; i++) {
      param.push_back(par[i]);
   }
   it = iParameterMap.find(typeUpper);
   assert(it != iParameterMap.end());
   Parameters par = it->second;
   int N = par.size();
   // Add size
   param.push_back(N);
   for(int i = 0; i < N; i++) {
      param.push_back(par[i]);
   }
   iAllParameters.setAllParameters(param);
   */
}

bool CalibratorPitBased::isFlatEnough(const Parameters& iCurve) const {
   int numBins = iCurve.size()+1;
   // the iCurve values passed in are the phi values of the calibration curve 
   // there is one value per smoothing point
   // the number of bins is one more than the size of iCurve, because iCurve has phi values
   // only at internal points (the 0 and 1 points are not specified)
   
   std::vector<float> binFreq (numBins);
   // since iCurve defined only at interior smoothing points, first binFreq is equal to first phi
   binFreq[0] = iCurve[0]; 
   // and the last binFreq is 1 - last phi
   binFreq[numBins-1] = 1.0 - iCurve[numBins-2]; 
   // the rest of the bins are filled by calculating the phi accumulated between interior smoothing points
   for (int i = 1; i < numBins-1; i++) {
      binFreq[i] = iCurve[i] - iCurve[i-1];
   }
   
   float expected = sqrt((float) (1 - (float) 1/numBins)/((float) mEfold * numBins));
   float binVar = 0;
   // Only check interior points
   for (int i = 1; i<numBins-1; i++) {
     binVar = binVar + pow(binFreq[i] - (float) 1/numBins, 2);
   }
   binVar /= numBins;
   // the adjustment factor of 1.5 allows us to use the e-folding time rather than a moving window size
   // this value was determined empirically and seems to fit well for eFolding times on the order of 100-350
   // the adjustment factor allows users to fine-tune the performance of the calibrator
   float actual_dev = mInteliCalFactor * 1.5 * sqrt(binVar);
   //std::cout << "InteliCal: actual=" << actual_dev << " expected=" << expected << std::endl;

   if (actual_dev <= expected) 
      return true; //the pit histogram is flat enough to be considered calibrated
   else 
      return false;

   // return true or false
}
