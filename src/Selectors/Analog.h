#ifndef SELECTOR_ANALOG_H
#define SELECTOR_ANALOG_H
#include "Selector.h"
#include "../Location.h"
#include "../Cache.h"
#include "../Key.h"

class DetMetric;
class Averager;

/** Select dates in the past based on analogs */
class SelectorAnalog : public Selector {
   public:
      SelectorAnalog(const Options& iOptions, const Data& iData);
      ~SelectorAnalog();
      int getMaxMembers() const;
      void getDefaultParameters(Parameters& iParameters) const;
      bool isLocationDependent() const;
      bool isOffsetDependent() const;
      void updateParameters(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      bool needsTraining() const;
   private:
      void selectCore(int iDate,
                  int iInit,
                  float iOffset,
                  const Location& iLocation,
                  const std::string& iVariable,
                  const Parameters& iParameters,
                  std::vector<Slice>& iSlices) const;
      bool mLocationIndependent;
      bool mOffsetIndependent;
      bool mDoNormalize;
      bool mDoObsForward;
      mutable std::vector<float> mWeights;
      std::vector<float> mAllOffsets;
      std::vector<std::string> mVariables;
      DetMetric* mMetric;
      int mNumAnalogs;
      Averager* mAverager;
      int mDayWidth;

      // Caching averaged ensemble
      mutable Cache<Key::Three<int,int,int>, std::vector<float> >mCache; // Date, offset, locationId
      // Get the forecast data (data used to find analogs)
      const std::vector<float>& getData(int iDate, int iOffsetId, const Location& iLocation) const;    
      bool mCheckIfObsExists;
      bool mComputeVariableVariances;
      //! How many hours earlier should we look for analogs?
      //! Positive is earlier, negative is later
      float mAdjustOffset;
};
#endif

