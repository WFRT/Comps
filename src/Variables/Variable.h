#ifndef VARIABLE_H
#define VARIABLE_H
#include "../Global.h"
#include "../Inputs/Input.h"
#include "../Options.h"
#include "../Component.h"

class Data;
class Location;
class Member;

class Variable : public Component {
   public:
      Variable(const Options& iOptions, const Data& iData);
      static Variable* getScheme(const Options& iOptions, const Data& iData);
      static Variable* getScheme(const std::string& iTag, const Data& iData);
      // Access default variables
      static const Variable* get(std::string iName);
      // TODO: How to free static map?
      virtual ~Variable();
      float compute(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType = Input::typeUnspecified) const;
      std::string getDescription() const;
      float getMin() const;
      float getMax() const;
      float getStd() const;
      float getMean() const;
      bool  isLowerDiscrete() const;
      bool  isUpperDiscrete() const;
      bool  isCircular() const;
      std::string getUnits() const;
      //! What variable does this output?
      virtual std::string getBaseVariable() const = 0;
      std::string getName() const {return getBaseVariable();};
      virtual bool isDerived() const {return true;}; // Do not override, except for Default
      std::string getStandardName() const;
      static std::string getUndecoratedVariable(std::string iDecoratedVariable);
      static std::string getDecoratedVariable(std::string iVariable, std::string iDecorator);
   protected:
      void loadOptionsFromBaseVariable();
      Variable(std::string iName);
      virtual float computeCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const Member& iMember,
            Input::Type iType = Input::typeUnspecified) const = 0;
      std::string mDescription;
      const Data& mData;
   private:
      std::string mUnits;
      float mMin;
      float mMax;
      float mStd;
      float mMean;
      bool  mLowerDiscrete;
      bool  mUpperDiscrete;
      bool mIsCircular;
      std::string mStandardName;
      static std::map<std::string, Variable*> mDefaultVariables;
      void init(const Options& iOptions);
      Options getOptions() const;
      Options mOptions;
};
#endif
