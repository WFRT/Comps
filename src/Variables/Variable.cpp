#include "SchemesHeader.inc"
#include "Variable.h"
#include "../Data.h"
#include "../Global.h"
#include "../Location.h"
#include "../Member.h"

Variable::Variable(const Options& iOptions, const Data& iData) : Component(iOptions),
      mData(iData),
      mDescription(""),
      mMin(Global::MV),
      mMax(Global::MV),
      mMean(Global::MV),
      mStd(Global::MV),
      mLowerDiscrete(false),
      mUpperDiscrete(false),
      mIsCircular(false),
      mStandardName(""),
      mUnits("") {
   iOptions.getValue("name", mName);
   iOptions.getValue("min", mMin);
   iOptions.getValue("max", mMax);
   iOptions.getValue("mean", mMean);
   iOptions.getValue("std", mStd);
   iOptions.getValue("lowerDiscrete", mLowerDiscrete);
   iOptions.getValue("upperDiscrete", mUpperDiscrete);
   iOptions.getValue("isCircular", mIsCircular);
   iOptions.getValue("standardName", mStandardName);
   iOptions.getValue("units", mUnits);
}

Variable::~Variable() {}

#include "Schemes.inc"

std::map<std::string, Variable*> Variable::mDefaultVariables;
const Variable* Variable::get(std::string iVariable) {
   assert(iVariable != "");
   std::map<std::string, Variable*>::const_iterator it = mDefaultVariables.find(iVariable);
   if(it == mDefaultVariables.end()) {
      // Not pretty
      Data data;
      Options options;
      Scheme::getOptions(iVariable, options);

      Variable* var = getScheme(options, data);
      mDefaultVariables[iVariable] = var;
      std::stringstream ss;
      ss << "Variable: Loading: " << iVariable;
      Global::logger->write(ss.str(), Logger::debug);
      return var;
   }
   else {
      return it->second;
   }
}
std::string Variable::getName() const {
   return mName;
}
std::string Variable::getDescription() const {
   return mDescription;
}
float Variable::getMin() const {
   return mMin;
}
float Variable::getMax() const {
   return mMax;
}
float Variable::getStd() const {
   return mStd;
}
float Variable::getMean() const {
   return mMean;
}
bool Variable::isLowerDiscrete() const {
   return mLowerDiscrete;
}
bool Variable::isUpperDiscrete() const {
   return mUpperDiscrete;
}

std::string Variable::getStandardName() const {
   return mStandardName;
}
float Variable::compute(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   // TODO: Opportunity to cache values
   Key::Input key(iDate, iInit, iOffset, iLocation.getId(), iMember.getId(), iType);
   float value = computeCore(iDate, iInit, iOffset, iLocation, iMember, iType);
   return value;
}
std::string Variable::getUnits() const {
   return mUnits;
}

bool  Variable::isCircular() const {
   return mIsCircular;
}

std::string Variable::getBaseVariable() const {
   return mName;
}

std::string Variable::providesVariable() const {
   return mName;
}
