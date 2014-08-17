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

   float cdfCont = mContinuous->getCdf(iX, iEnsemble, parMap[typeContinuous]);
   if(!Global::isValid(cdfCont)) {
      return Global::MV;
   }

   // Continuous models (or discrete with flow-over)
   if(!mDoLower && !mDoUpper)
      return cdfCont;

   // Boundary probabilities
   float P0  = getP0(iEnsemble, parMap);
   float P1  = getP1(iEnsemble, parMap);

   if(!Global::isValid(P0) || !Global::isValid(P1)) {
      return Global::MV;
   }
   assert(P0 + P1 <= 1);

   // Check if we are on the boundaries
   float X0 = Variable::get(iEnsemble.getVariable())->getMin();
   float X1 = Variable::get(iEnsemble.getVariable())->getMax();
   if(iX == X0)
      return P0;
   if(iX == X1)
      return P1;

   if(P0 == 1)
      // All mass is on the lower discrete
      return 1;
   if(P1 == 1)
      // All mass is on the upper discrete
      return 0;

   // Combine probabilities
   float overflow0 = 0;
   float overflow1 = 0;
   if(mDoLower) {
      overflow0 = getOverflow0(iEnsemble, parMap); 
   }
   if(mDoUpper) {
      overflow1 = getOverflow1(iEnsemble, parMap); 
   }
   if(!Global::isValid(overflow0) || !Global::isValid(overflow1)) {
      return Global::MV;
   }
   assert(overflow0 <= 1 && overflow0 >= 0);
   assert(overflow1 <= 1 && overflow1 >= 0);
   assert(overflow0 + overflow1 <= 1);

   // Check that the whole distribution didn't overflow one of the boundaries
   if(overflow0 == 1)
      return 1;
   if(overflow1 == 1)
      return 0;

   // Stretch the continuous CDF such that it is 0 at X0 and 1 at X1
   float cdf = (cdfCont- overflow0)/(1 - overflow0 - overflow1);
   assert(cdf >= 0 && cdf <= 1);

   // Attach the continuous CDF between the two discrete masses
   cdf = P0 + cdf * (1 - P0 - P1);
   assert(cdf >= 0 && cdf <= 1);

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

   float pdfCont = mContinuous->getPdf(iX, iEnsemble, parMap[typeContinuous]);
   if(!Global::isValid(pdfCont)) {
      return Global::MV;
   }

   // Continuous models (or discrete with flow-over)
   if(!mDoLower && !mDoUpper)
      return pdfCont;

   // Boundary probabilities
   float P0  = getP0(iEnsemble, parMap);
   float P1  = getP1(iEnsemble, parMap);

   if(!Global::isValid(P0) || !Global::isValid(P1)) {
      return Global::MV;
   }
   assert(P0 + P1 <= 1);

   // Check if we are on the boundaries
   float X0 = Variable::get(iEnsemble.getVariable())->getMin();
   float X1 = Variable::get(iEnsemble.getVariable())->getMax();
   if(iX == X0)
      return P0;
   if(iX == X1)
      return P1;

   if(P0 == 1 || P1 == 1)
      // All mass is on one of the discretes
      return 0;

   // Combine probabilities
   float overflow0 = 0;
   float overflow1 = 0;
   if(mDoLower) {
      overflow0 = getOverflow0(iEnsemble, parMap); 
   }
   if(mDoUpper) {
      overflow1 = getOverflow1(iEnsemble, parMap); 
   }
   if(!Global::isValid(overflow0) || !Global::isValid(overflow1)) {
      return Global::MV;
   }
   assert(overflow0 <=1 && overflow0 >= 0);
   assert(overflow1 <=1 && overflow1 >= 0);
   assert(overflow1 + overflow1 <= 1);

   // Check that the whole distribution didn't overflow one of the boundaries
   if(overflow0 == 1 || overflow1 == 0)
      return 0;

   // The overflow is removed, therefore we get a higher PDF
   float pdf = pdfCont / (1 - overflow0 - overflow1);
   assert(pdf >= 0);

   // Account for boundary probability, there we get a lower PDF
   pdf = pdf * (1 - P0 - P1);
   assert(pdf >= 0);

   return pdf;
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

   assert(mDoContinuous);

   // Continuous models (or discrete with flow-over)
   if(!mDoLower && !mDoUpper) {
      return mContinuous->getInv(iCdf, iEnsemble, parMap[typeContinuous]);
   }

   // Boundary probabilities
   float P0  = getP0(iEnsemble, parMap);
   float P1  = getP1(iEnsemble, parMap);

   if(!Global::isValid(P0) || !Global::isValid(P1)) {
      return Global::MV;
   }
   assert(P0 + P1 <= 1);

   // Check if we are on the boundaries
   float X0 = Variable::get(iEnsemble.getVariable())->getMin();
   float X1 = Variable::get(iEnsemble.getVariable())->getMax();
   if(iCdf <= P0)
      return X0;
   if(iCdf >= 1-P1)
      return X1;

   // Combine probabilities
   float overflow0 = 0;
   float overflow1 = 0;
   if(mDoLower) {
      overflow0 = getOverflow0(iEnsemble, parMap); 
   }
   if(mDoUpper) {
      overflow1 = getOverflow1(iEnsemble, parMap); 
   }
   if(!Global::isValid(overflow0) || !Global::isValid(overflow1)) {
      return Global::MV;
   }
   assert(overflow0 <= 1 && overflow0 >= 0);
   assert(overflow1 <= 1 && overflow1 >= 0);
   assert(overflow0 + overflow1 <= 1);

   // Check that the whole distribution didn't overflow one of the boundaries
   if(overflow0 == 1)
      return X0;
   if(overflow1 == 1)
      return X1;

   assert(1 - P0 - P1 > 0);
   iCdf = (iCdf - P0)/(1 - P0 - P1);
   assert(iCdf >= 0 && iCdf <= 1);
   iCdf = iCdf * (1 - overflow0 - overflow1) + overflow0;
   assert(iCdf > 0 && iCdf < 1);

   float x = mContinuous->getInv(iCdf, iEnsemble, parMap[typeContinuous]);

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

float UncertaintyCombine::getP0(const Ensemble& iEnsemble, std::map<UncertaintyCombine::Type, Parameters>& iParMap) const {
   const Variable* var = Variable::get(iEnsemble.getVariable());
   if(!var->isLowerDiscrete()) {
      // Non-discrete variables
      return 0;
   }

   if(mDoLower) {
      // Discrete variables, with a boundary model
      if(!var->isLowerDiscrete()) {
         std::stringstream ss;
         ss << "Variable " << var->getName() << " does not have a lower discrete mass.";
         Global::logger->write(ss.str(), Logger::error);
      }

      float P0 = mDiscreteLower->getP(iEnsemble, iParMap[typeLower]);
      return P0;
   }
   else {
      // Discrete variables, without a boundary model
      float overflow = getOverflow0(iEnsemble, iParMap);
      return overflow;
   }
}
float UncertaintyCombine::getP1(const Ensemble& iEnsemble, std::map<UncertaintyCombine::Type, Parameters>& iParMap) const {
   const Variable* var = Variable::get(iEnsemble.getVariable());
   if(!var->isUpperDiscrete()) {
      // Non-discrete variables
      return 0;
   }

   if(mDoUpper) {
      // Discrete variables, with a boundary model
      if(!var->isUpperDiscrete()) {
         std::stringstream ss;
         ss << "Variable " << var->getName() << " does not have an upper discrete mass.";
         Global::logger->write(ss.str(), Logger::error);
      }

      float P1 = mDiscreteUpper->getP(iEnsemble, iParMap[typeLower]);
      return P1;
   }
   else {
      // Discrete variables, without a boundary model
      float overflow = getOverflow1(iEnsemble, iParMap);
      return overflow;
   }
}
float UncertaintyCombine::getOverflow0(const Ensemble& iEnsemble, std::map<UncertaintyCombine::Type, Parameters>& iParMap) const {
   float X0 = Variable::get(iEnsemble.getVariable())->getMin();
   float dP0 = mContinuous->getCdf(X0, iEnsemble, iParMap[typeContinuous]);
   return dP0;
}
float UncertaintyCombine::getOverflow1(const Ensemble& iEnsemble, std::map<UncertaintyCombine::Type, Parameters>& iParMap) const {
   float X1 = Variable::get(iEnsemble.getVariable())->getMax();
   float dP1 = mContinuous->getCdf(X1, iEnsemble, iParMap[typeContinuous]);
   if(!Global::isValid(dP1))
      return Global::MV;
   else
      return 1-dP1;
}

Continuous* UncertaintyCombine::getContinuous() const {
   return mContinuous;
}
Discrete* UncertaintyCombine::getDiscreteLower() const {
   return mDiscreteLower;
}
Discrete* UncertaintyCombine::getDiscreteUpper() const {
   return mDiscreteUpper;
}
