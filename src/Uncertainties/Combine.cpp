#include "Combine.h"
#include "../Parameters.h"
#include "../Options.h"
#include "../Scheme.h"
#include "../Continuous/Continuous.h"
#include "../Discretes/Discrete.h"
#include "../Distribution.h"
#include "../Variables/Variable.h"
#include "../Obs.h"

UncertaintyCombine::UncertaintyCombine(const Options& iOptions, const Data& iData) : Uncertainty(iOptions, iData),
      mContinuous(NULL),
      mDiscreteLower(NULL),
      mDiscreteUpper(NULL),
      mDoContinuous(false),
      mDoLower(false),
      mDoUpper(false),
      mDoOnlyDiscrete(false) {

   std::string tag;
   if(iOptions.getValue("continuous", tag)) {
      Options opt;
      Scheme::getOptions(tag, opt);
      mContinuous = Continuous::getScheme(opt, iData);
      mDoContinuous = true;
   }
   if(iOptions.getValue("discreteLower", tag)) {
      Options opt;
      Scheme::getOptions(tag, opt);
      mDiscreteLower = Discrete::getScheme(opt, iData);
      mDoLower = true;
   }
   if(iOptions.getValue("discreteUpper", tag)) {
      Options opt;
      Scheme::getOptions(tag, opt);
      mDiscreteUpper = Discrete::getScheme(opt, iData);
      mDoLower = true;
   }
   if(iOptions.getValue("discrete", tag)) {
      Options opt;
      Scheme::getOptions(tag, opt);
      mDiscreteUpper = Discrete::getScheme(opt, iData);
      mDoLower = true;
      mDoOnlyDiscrete = true;
   }
   assert(mDoContinuous || mDoOnlyDiscrete); // Don't use discrete if there is a continuous
   assert(!mDoOnlyDiscrete || (!mDoLower && !mDoUpper)); // Don't mix discrete with lower/upper
   assert(mDoContinuous || (!mDoLower && !mDoUpper)); // Don't use lower/upper without continuous
}

UncertaintyCombine::~UncertaintyCombine() {
   if(mDoContinuous)
      delete mContinuous;
   if(mDoLower)
      delete mDiscreteLower;
   if(mDoUpper)
      delete mDiscreteUpper;
}

float UncertaintyCombine::getCdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::map<UncertaintyCombine::Type, Parameters> parMap;
   subsetParameters(iParameters, parMap);
   std::string variable = iEnsemble.getVariable();

   if(mDoOnlyDiscrete) {
      Parameters par = parMap[typeLower];
      float P = mDiscreteLower->getP(iEnsemble, par);
      float X = Variable::get(variable)->getMin();
      if(!Global::isValid(P) || !Global::isValid(X)) {
         return Global::MV;
      }
      if(iX < X)
         return 0;
      else if(iX == X)
         return P;
      else
         return 1;
   }

   assert(mDoContinuous);

   Parameters parCont = parMap[typeContinuous];
   float cdf = mContinuous->getCdf(iX, iEnsemble, parCont);
   if(!Global::isValid(cdf)) {
      return Global::MV;
   }

   float P0 = 0;
   float P1 = 0;
   float dP0 = 0;
   float dP1 = 0;
   
   if(mDoLower) {
      Parameters par = parMap[typeLower];
      P0 = mDiscreteLower->getP(iEnsemble, par);
      float X0 = Variable::get(variable)->getMin();
      if(iX < X0) {
         std::stringstream ss;
         ss << "Cannot compute CDF for values below " << X0 << ", which is the minimum for variable "
            << Variable::get(variable)->getName();
         Global::logger->write(ss.str(), Logger::error);
      }
      dP0 = mContinuous->getCdf(X0, iEnsemble, parCont);
   }
   if(mDoUpper) {
      Parameters par = parMap[typeUpper];
      P1 = mDiscreteLower->getP(iEnsemble, par);
      float X1 = Variable::get(variable)->getMax();
      if(iX > X1) {
         std::stringstream ss;
         ss << "Cannot compute CDF for values above " << X1 << ", which is the maximum for variable "
            << Variable::get(variable)->getName();
         Global::logger->write(ss.str(), Logger::error);
      }
      dP1 = mContinuous->getCdf(X1, iEnsemble, parCont);
   }
   if(!Global::isValid(P0) || !Global::isValid(P1) || !Global::isValid(dP0) || !Global::isValid(dP1)) {
      return Global::MV;
   }

   assert(1 - dP0 - dP1 >= 0);
   assert(1 - P0 - P1 >= 0);

   // Combine probabilities
   cdf = (cdf - dP0)/(1 - dP0 - dP1);
   cdf = P0 + cdf * (1 - P0 - P1);

   return cdf;
}
float UncertaintyCombine::getPdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::map<UncertaintyCombine::Type, Parameters> parMap;
   subsetParameters(iParameters, parMap);
   std::string variable = iEnsemble.getVariable();

   if(mDoOnlyDiscrete) {
      Parameters par = parMap[typeLower];
      float P = mDiscreteLower->getP(iEnsemble, par);
      float X = Variable::get(variable)->getMin();
      if(!Global::isValid(P) || !Global::isValid(X)) {
         return Global::MV;
      }
      if(iX == X)
         return P;
      else
         return 1-P;
   }

   assert(mDoContinuous);

   Parameters parCont = parMap[typeContinuous];
   float P0 = 0;
   float P1 = 0;
   float dP0 = 0;
   float dP1 = 0;
   if(mDoLower) {
      // Check if on lower boundary
      Parameters par = parMap[typeLower];
      P0 = mDiscreteLower->getP(iEnsemble, par);
      float X0 = Variable::get(variable)->getMin();
      if(iX == X0) {
         return P0;
      }
      dP0 = mContinuous->getCdf(X0, iEnsemble, parCont);
   }
   if(mDoUpper) {
      // Check if on upper boundary
      Parameters par = parMap[typeUpper];
      P1 = mDiscreteLower->getP(iEnsemble, par);
      float X1 = Variable::get(variable)->getMax();
      if(iX == X1) {
         return P1;
      }
      dP1 = mContinuous->getCdf(X1, iEnsemble, parCont);
   }
   if(!Global::isValid(P0) || !Global::isValid(P1) || !Global::isValid(dP0) || !Global::isValid(dP1)) {
      return Global::MV;
   }

   float pdf = mContinuous->getPdf(iX, iEnsemble, parCont);

   if(!Global::isValid(pdf)) {
      return Global::MV;
   }

   assert(1 - dP0 - dP1 > 0);
   assert(1 - P0 - P1 > 0);

   // Combine probabilities
   return pdf * (1 + dP0 + dP1 - P0 - P1);
}
float UncertaintyCombine::getInv(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::map<UncertaintyCombine::Type, Parameters> parMap;
   subsetParameters(iParameters, parMap);
   std::string variable = iEnsemble.getVariable();

   if(mDoOnlyDiscrete) {
      Parameters par = parMap[typeLower];
      float P = mDiscreteLower->getP(iEnsemble, par);
      if(!Global::isValid(P))
         return Global::MV;

      float X = Variable::get(variable)->getMin();
      if(!Global::isValid(P) || !Global::isValid(X)) {
         return Global::MV;
      }
      //! Returns 0 (false) if cdf is less than POP and 1 (true) otherwise
      // TODO:
      return iCdf > P;
   }

   Parameters parCont = parMap[typeContinuous];
   float P0 = 0;
   float P1 = 0;
   float dP0 = 0;
   float dP1 = 0;
   if(mDoLower) {
      // Check if on lower boundary
      Parameters par = parMap[typeLower];
      P0 = mDiscreteLower->getP(iEnsemble, par);
      if(!Global::isValid(P0))
         return Global::MV;
      float X0 = Variable::get(variable)->getMin();
      dP0 = mContinuous->getCdf(X0, iEnsemble, parCont);
      if(!Global::isValid(dP0))
         return Global::MV;
      if(iCdf <= P0)
         return X0;
   }
   if(mDoUpper) {
      // Check if on upper boundary
      Parameters par = parMap[typeUpper];
      P1 = mDiscreteLower->getP(iEnsemble, par);
      if(!Global::isValid(P1))
         return Global::MV;
      float X1 = Variable::get(variable)->getMax();
      dP1 = mContinuous->getCdf(X1, iEnsemble, parCont);
      if(!Global::isValid(dP1))
         return Global::MV;
      if(iCdf >= 1 - P1) {
         return X1;
      }
   }
   if(!Global::isValid(P0) || !Global::isValid(P1) || !Global::isValid(dP0) || !Global::isValid(dP1)) {
      return Global::MV;
   }

   // Combine probabilities
   // TODO: Check that these are correct
   assert(iCdf > 0 & iCdf < 1);
   iCdf = (iCdf - P0)/(1 - P0 - P1);
   assert(iCdf > 0 & iCdf < 1);
   iCdf = iCdf * (1 - dP0 - dP1) + dP0;
   assert(iCdf > 0 & iCdf < 1);

   float x = mContinuous->getInv(iCdf, iEnsemble, parCont);

   if(!Global::isValid(x)) {
      return Global::MV;
   }
   return x;
}

float UncertaintyCombine::getMoment(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::map<UncertaintyCombine::Type, Parameters> parMap;
   subsetParameters(iParameters, parMap);
   if(!mDoLower && !mDoUpper) {
      return mContinuous->getMoment(iMoment, iEnsemble, parMap[typeContinuous]);
   }
   else {
      Component::underDevelopment();
      return Global::MV;
   }
}

void UncertaintyCombine::subsetParameters(const Parameters& iAllParameters, std::map<UncertaintyCombine::Type, Parameters>& iParameterMap) const {
   // Continuous parameters
   if(iAllParameters.size() == 0) {
      // TODO: Do we need this?
      iParameterMap[typeContinuous] = Parameters();
      if(mDoLower) 
         iParameterMap[typeLower] = Parameters();
      if(mDoUpper) 
         iParameterMap[typeUpper] = Parameters();
      return;
   }
   assert(iAllParameters.size()>0);
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
   if(mDoLower) {
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
   }

   // Upper discrete parameters
   if(mDoUpper) {
      assert(iAllParameters.size() > ptr);
      std::vector<float> param;
      int N = iAllParameters[ptr];
      assert(iAllParameters.size() > ptr + N);
      param.resize(N);
      for(int i = 0; i < N; i++) {
         param[i] = iAllParameters[ptr + 1 + i];
      }
      iParameterMap[typeUpper] = Parameters(param);
   }
}

// Package parameters together
// N contParam1...contParamN M lowerParam1...lowerParamM P upperParam1...upperParamP
void UncertaintyCombine::getDefaultParameters(Parameters& iParameters) const {
   std::map<Type, Parameters> parMap;
   Parameters parCont;
   mContinuous->getDefaultParameters(parCont);
   parMap[typeContinuous] = parCont;

   if(mDoLower) {
      Parameters par;
      mDiscreteLower->getDefaultParameters(par);
      parMap[typeLower] = par;
   }
   if(mDoUpper) {
      Parameters par;
      mDiscreteUpper->getDefaultParameters(par);
      parMap[typeUpper] = par;
   }
   packageParameters(parMap, iParameters);
}

void UncertaintyCombine::updateParameters(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {
   std::map<Type,Parameters> parMap;
   subsetParameters(iParameters, parMap);

   mContinuous->updateParameters(iEnsemble, iObs, parMap[typeContinuous]);
   if(mDoLower) {
      mDiscreteLower->updateParameters(iEnsemble, iObs, parMap[typeLower]);
   }
   if(mDoUpper) {
      mDiscreteUpper->updateParameters(iEnsemble, iObs, parMap[typeUpper]);
   }
   packageParameters(parMap, iParameters);
}

void UncertaintyCombine::packageParameters(const std::map<UncertaintyCombine::Type, Parameters>& iParameterMap, Parameters& iAllParameters) const {
   std::map<UncertaintyCombine::Type, Parameters>::const_iterator it = iParameterMap.find(typeContinuous);
   assert(it != iParameterMap.end());
   Parameters parCont = it->second;
   std::vector<float> param;
   // Add size
   int N = parCont.size();
   param.push_back(N);
   for(int i = 0; i < N; i++) {
      param.push_back(parCont[i]);
   }

   if(mDoLower) {
      it = iParameterMap.find(typeLower);
      assert(it != iParameterMap.end());
      Parameters par = it->second;
      int N = par.size();
      // Add size
      param.push_back(N);
      for(int i = 0; i < N; i++) {
         param.push_back(par[i]);
      }
   }
   if(mDoUpper) {
      it = iParameterMap.find(typeUpper);
      assert(it != iParameterMap.end());
      Parameters par = it->second;
      int N = par.size();
      // Add size
      param.push_back(N);
      for(int i = 0; i < N; i++) {
         param.push_back(par[i]);
      }
   }
   iAllParameters.setAllParameters(param);
}
 
bool UncertaintyCombine::needsConstantEnsembleSize() const {
   bool flag=false;
   if(mDoContinuous && mContinuous->needsConstantEnsembleSize())
      flag = true;
   if(mDoLower && mDiscreteLower->needsConstantEnsembleSize())
      flag = true;
   if(mDoUpper && mDiscreteUpper->needsConstantEnsembleSize())
      flag = true;
   return flag;
}

bool UncertaintyCombine::needsTraining() const {
   bool flag=false;
   if(mDoContinuous && mContinuous->needsTraining())
      flag = true;
   if(mDoLower && mDiscreteLower->needsTraining())
      flag = true;
   if(mDoUpper && mDiscreteUpper->needsTraining())
      flag = true;
   return flag;
}
