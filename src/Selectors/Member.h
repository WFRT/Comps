#ifndef SELECTOR_MEMBER_H
#define SELECTOR_MEMBER_H
#include "Selector.h"

/** Select specific ensemble members */
class SelectorMember : public Selector {
   public:
      SelectorMember(const Options& rOptions, const Data& iData);
      int  getMaxMembers() const;
      void getDefaultParameters(Parameters& iParameters) const {};
      bool isLocationDependent() const {return false;};
      bool isVariableDependent() const {return false;};
      //bool isOffsetDependent() const {return false;};
      bool needsTraining() const {return false;};
   private:
      void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Field>& iFields) const;
      std::vector<int> mMembers;
      float mMaxResolution;
      float mMinResolution;
      bool mSelectByMember;
      std::vector<std::string> mModels;
};
#endif
