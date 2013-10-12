#ifndef CONFIGURATION_DEFAULT_H
#define CONFIGURATION_DEFAULT_H
#include "Configuration.h"
#include "../Key.h"
#include "../Cache.h"
class Selector;
class Downscaler;
class Corrector;
class Calibrator;
class Updater;
class Averager;
class Smoother;
class Field;
class Uncertainty;
class EstimatorProbabilistic;

/** Contains all schemes for a particular configuration
 *  Does not know how components are linked */
class ConfigurationDefault : public Configuration {
   public:
      ConfigurationDefault(const Options& iOptions, const Data& iData);
      ~ConfigurationDefault();

      Distribution::ptr getDistribution(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable,
            ProcTypeDist iType = typeCalibrated) const;
      void getEnsemble(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable,
            Ensemble& iEnsemble,
            ProcTypeEns iType = typeCorrected) const;
      void  getDeterministic(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable,
            Deterministic& iDeterministic) const;
      void updateParameters(int iDate, int iInit, const std::string& iVariable);
      bool isValid(std::string& iMessage) const;
      std::string toString() const;
   private:
      void getSelectorIndicies(int iDate,
            int iInit,
            float iOffset, 
            const Location& iLocation,
            std::string iVariable,
            std::vector<Field>& slices) const;
      const Selector*   mSelector;
      const Downscaler* mDownscaler;
      const Corrector*  mCorrector;
      const Uncertainty* mUncertainty;
      const Averager*   mAverager;
      std::vector<const Corrector*> mCorrectors;
      std::vector<const Calibrator*> mCalibrators;
      std::vector<const Smoother*> mSmoothers;

      mutable Cache<Key::Ensemble, std::vector<Field> > mSelectorCache;
      mutable Cache<Key::Ensemble, Ensemble > mCorrectorCache;
};
#endif
