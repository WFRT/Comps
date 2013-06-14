#ifndef AVERAGER_MEMBER_H
#define AVERAGER_MEMBER_H
#include "../Global.h"
#include "../Options.h"

//! Specify which members to include in the average
class AveragerMember : public Averager {
   public:
      AveragerMember(const Options& iOptions, const Data& iData);
      float average(const Ensemble& iValues, const Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const {};
      bool needsTraining() const {return false;};
   private:
      std::vector<int> mMembers;
};
#endif
