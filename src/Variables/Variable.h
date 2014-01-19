#ifndef VARIABLE_H
#define VARIABLE_H
#include "../Global.h"
#include "../Inputs/Input.h"
#include "../Options.h"

class Data;
class Location;
class Member;

class Variable {
   public:
      static const Variable* get(std::string iName);
      // TODO: How to free static map?
      //~Variable();
      float compute(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType = Input::typeUnspecified) const;
      std::string getName() const;
      std::string getDescription() const;
      float getMin() const;
      float getMax() const;
      float getStd() const;
      float getMean() const;
      bool  isLowerDiscrete() const;
      bool  isUpperDiscrete() const;
      bool  isCircular() const;
      std::string getUnits() const;
      static void destroy();
      virtual std::string getBaseVariable() const; // This should almost always be overridden
      virtual bool isDerived() const {return true;}; // Do not override, except for Bypass
   protected:
      Variable(std::string iName);
      virtual float computeCore(const Data& iData,
            int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const Member& iMember,
            Input::Type iType = Input::typeUnspecified) const = 0;
      std::string mName;
      std::string mDescription;
      Options mOptions;
   private:
      static Variable* create(std::string iName);
      static std::map<std::string, Variable*> mVariables;
      std::string mUnits;
      float mMin;
      float mMax;
      float mStd;
      float mMean;
      bool  mLowerDiscrete;
      bool  mUpperDiscrete;
      bool mIsCircular;
      mutable Cache<Key::Input, float> mCache;
};
#endif
