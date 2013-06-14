#ifndef SELECTOR_SUPER_H
#define SELECTOR_SUPER_H
#include "Selector.h"
class Input;
class Member;

//! Creates a super ensemble by using members from multiple EPSes
class SelectorSuper : public Selector {
   public:
      SelectorSuper(const Options& iOptions, const Data& iData);
      bool isLocationDependent() const {return false;};
      bool needsTraining() const {return false;};
   private:
      //! Does not check if the datasets have the offsets
      //! Relies on Input to do the time interpolation
      void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Slice>& iSlices) const;
      std::vector<Input*> mInputs;
      std::vector<Member> mMembers;
};
#endif
