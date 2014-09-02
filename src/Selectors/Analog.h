#ifndef SELECTOR_ANALOG_H
#define SELECTOR_ANALOG_H
#include "Selector.h"
#include "../Location.h"
#include "../Cache.h"
#include "../Key.h"

class DetMetric;
class Averager;
class Input;

//! Select dates/offsets in the past where the forecast issued is similar to today's forecast
class SelectorAnalog : public Selector {
   public:
      SelectorAnalog(const Options& iOptions, const Data& iData);
      ~SelectorAnalog();
      int getMaxMembers() const;
      void getDefaultParameters(Parameters& iParameters) const;
      bool isLocationDependent() const;
      bool isOffsetDependent() const;
      void updateParameters(const std::vector<int>& iDates,
            int iInit,
            const std::vector<float>& iOffsets,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      bool needsTraining() const;
      // Get the forecast data (data used to find analogs)
      const std::vector<float>& getData(int iDate, int iInit, float iOffset, const Location& iLocation, const std::vector<std::string>& iVariables) const;    
   private:
      void selectCore(int iDate,
                  int iInit,
                  float iOffset,
                  const Location& iLocation,
                  const std::string& iVariable,
                  const Parameters& iParameters,
                  std::vector<Field>& iFields) const;
      int  numParametersCore() const;
      bool mLocationIndependent;
      bool mOffsetIndependent;
      bool mDoObsForward;
      mutable std::vector<float> mWeights;
      std::vector<float> mAllOffsets;
      std::vector<std::string> mVariables;
      DetMetric* mMetric;
      int mNumAnalogs;
      int mDayWidth;
      Input* mObsInput;

      // Caching averaged ensemble
      mutable Cache<Key::Three<int,float,int>, std::vector<float> >mCache; // Date, offset, locationId
      bool mComputeVariableVariances;
      //! How many hours earlier should we look for analogs?
      //! Positive is earlier, negative is later
      float mAdjustOffset;
      std::string mDataset;
      bool mPrintDates;
      bool mAllowFutureValues;
};
#endif

