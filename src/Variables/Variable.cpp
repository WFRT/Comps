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
   std::string line = nl.findLine(iName);

   if(line != "") {
      mOptions = Options(line);
      mOptions.getValue("min", mMin);
      mOptions.getValue("max", mMax);
      mOptions.getValue("mean", mMean);
      mOptions.getValue("std", mStd);
      mOptions.getValue("lowerDiscrete", mLowerDiscrete);
      mOptions.getValue("upperDiscrete", mUpperDiscrete);
      mOptions.getValue("description", mDescription);
      mOptions.getValue("isCircular", mIsCircular);
      mOptions.getValue("units", mUnits);
      mOptions.getValues("cdfX", mCdfX);
      mOptions.getValues("pdfX", mPdfX);
      if(!mOptions.getValues("cdfInv", mCdfInv)) {
         float cdfInv0 = Global::MV;
         float cdfInv1 = Global::MV;
         float cdfInvDx = Global::MV;
         mOptions.getValue("cdfInv0", cdfInv0);
         mOptions.getValue("cdfInv1", cdfInv1);
         mOptions.getValue("cdfInvDx", cdfInvDx);
         if(Global::isValid(cdfInv0) && Global::isValid(cdfInv1) && Global::isValid(cdfInvDx)) {
            if(cdfInv1 > cdfInv0 && cdfInvDx > 0 && (cdfInv1-cdfInv0)/cdfInvDx < 1000) {
               float cdfInv = cdfInv0;
               while(cdfInv <= cdfInv1) {
                  mCdfInv.push_back(cdfInv);
                  cdfInv += cdfInvDx;
               }
            }
            else {
               std::stringstream ss;
               ss << "cdfInv0, cdfInv1 and/or cdfInvDx invalid for variable " << mName << std::endl;
               ss << "Requirements:" << std::endl;
               ss << "   cdfInv0 < cdfInv1" << std::endl;
               ss << "   cdfInvDx > 0" << std::endl;
               ss << "   (cdfInv1 - cdfInv1)/cdfInvDx < 1000";
               Global::logger->write(ss.str(), Logger::error);
            }
         }
      }
   }

   if(mCdfX.size() == 0)
      mCdfX.push_back(mMean);
   if(mPdfX.size() == 0)
      mPdfX.push_back(mMean);
   if(mCdfInv.size() == 0) {
      mCdfInv.push_back(0.01);
      for(int i = 1; i<= 9; i++) {
         mCdfInv.push_back((float) i/10);
      }
      mCdfInv.push_back(0.99);
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
void Variable::getCdfX(std::vector<float>& iCdfX) const {
   iCdfX = mCdfX;
}
void Variable::getPdfX(std::vector<float>& iPdfX) const {
   iPdfX = mPdfX;
}
void Variable::getCdfInv(std::vector<float>& iCdfInv) const {
   iCdfInv = mCdfInv;
}

float Variable::compute(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   // TODO: Opportunity to cache values
   Key::Input key(iDate, iInit, iOffset, iLocation.getId(), iMember.getId(), 0);
   float value;
   if(mCache.isCached(key)) {
      const std::vector<float> values = mCache.get(key);
      value = values[0];
   }
   else {
      value = computeCore(iData, iDate, iInit, iOffset, iLocation, iMember, iType);
      // Cache the value
      std::vector<float> values;
      values.push_back(value);
      mCache.add(key, values);
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
