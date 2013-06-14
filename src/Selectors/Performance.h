#ifndef SELECTOR_PERFORMANCE_H
#define SELECTOR_PERFORMANCE_H
#include "Selector.h"

class DetMetric;
/** Select the best ensemble member */
class SelectorPerformance : public Selector {
   public:
      SelectorPerformance(const Options& iOptions, const Data& iData);
      ~SelectorPerformance();
      int getMaxMembers() const;
      void getDefaultParameters(Parameters& iParameters) const;
      void updateParameters(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
   private:
      void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Slice>& iSlices) const;
      int mNum;
      DetMetric* mMetric;
};
#endif
