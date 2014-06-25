#ifndef CONFIGURATION_DEFAULT_H
#define CONFIGURATION_DEFAULT_H
#include "Configuration.h"
#include "../Key.h"
class Selector;
class Downscaler;
class Corrector;
class Calibrator;
class Updater;
class Averager;
class Smoother;
class Field;
class UncertaintyCombine;
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
      void updateParameters(int iDate, int iInit, const std::vector<float>& iOffsets, const std::vector<Location>& iLocations, const std::string& iVariable);
      bool isValid(std::string& iMessage) const;
      std::string toString() const;
   private:
      void getEnsemble(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable,
            Ensemble& iEnsemble,
            ProcTypeEns iType = typeCorrected) const;
      // Update using provided observations for forecasts issued on iDate, iInit, iOffset. Use
      // parameters from iOffsetGet and write to parameters on iOffsetSet
      void updateParameters(const std::vector<Obs>& iObs, int iDate, int iInit, float iOffset, int iPoolId, const std::string& iVariable, int iDateGet, int iDateSet, float iOffsetGet, float iOffsetSet);
      const Selector*   mSelector;
      const Corrector*  mCorrector;
      const UncertaintyCombine* mUncertainty;
      const Averager*   mAverager;
      std::vector<const Corrector*>  mCorrectors;
      std::vector<const Calibrator*> mCalibrators;
      std::vector<const Updater*>    mUpdaters;
      std::vector<const Smoother*>   mSmoothers;
      int mNumOffsetsSpreadObs;

      //! Do we need the ensemble when updating parameters?
      bool getNeedEnsemble() const;
};
#endif
