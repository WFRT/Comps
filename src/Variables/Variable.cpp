#include "SchemesHeader.inc"
#include "Variable.h"
#include "../Data.h"
#include "../Global.h"
#include "../Location.h"
#include "../Member.h"

Variable::Variable(std::string iName) :
      mName(iName),
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
   int dotPosition = -1;
   for(int i = 0; i < iName.size(); i++) {
      if(iName[i] == '.')
         dotPosition = i;
   }
   std::string folder;
   std::string tag;
   if(dotPosition == -1) {
      folder = "default";
      tag = iName;
   }
   else {
      assert(dotPosition != 0);
      folder = iName.substr(0,dotPosition);
      tag = iName.substr(dotPosition+1);//, iTag.size()-2);
   }
   mDescription = "";
   Namelist nl("variables", folder);

   if(nl.getOptions(iName, mOptions)) {
      mOptions.getValue("min", mMin);
      mOptions.getValue("max", mMax);
      mOptions.getValue("mean", mMean);
      mOptions.getValue("std", mStd);
      mOptions.getValue("lowerDiscrete", mLowerDiscrete);
      mOptions.getValue("upperDiscrete", mUpperDiscrete);
      mOptions.getValue("description", mDescription);
      mOptions.getValue("isCircular", mIsCircular);
      mOptions.getValue("units", mUnits);
      mOptions.getValue("standardName", mStandardName);
   }

   mCache.setName(mName);
}
/*
Variable::~Variable() {
   // Delete cached variables
   std::map<std::string, Variable*>::iterator itVar;
   for(itVar = mVariables.begin(); itVar != mVariables.end(); itVar++) {
      delete itVar->second;
   }
}
*/
#include "Schemes.inc"

std::map<std::string, Variable*> Variable::mVariables;
const Variable* Variable::get(std::string iVariable) {
   assert(iVariable != "");
   std::map<std::string, Variable*>::const_iterator it = mVariables.find(iVariable);
   if(it == mVariables.end()) {
      Variable* var = Variable::create(iVariable);
      mVariables[iVariable] = var;
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
float Variable::compute(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   // TODO: Opportunity to cache values
   Key::Input key(iDate, iInit, iOffset, iLocation.getId(), iMember.getId(), iType);
   float value;
   if(mCache.isCached(key)) {
      value = mCache.get(key);
   }
   else {
      value = computeCore(iData, iDate, iInit, iOffset, iLocation, iMember, iType);
      // Cache the value
      mCache.add(key, value);
   }
   return value;
}
std::string Variable::getUnits() const {
   return mUnits;
}

bool  Variable::isCircular() const {
   return mIsCircular;
}

void Variable::destroy() {
   // Delete cached variables
   std::map<std::string, Variable*>::iterator itVar;
   for(itVar = mVariables.begin(); itVar != mVariables.end(); itVar++) {
      delete itVar->second;
   }
}
std::string Variable::getBaseVariable() const {
   return mName;
}
