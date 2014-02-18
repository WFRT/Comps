#include "Default.h"
#include "../Uncertainties/Uncertainty.h"
#include "../Selectors/Selector.h"
#include "../Downscalers/Downscaler.h"
#include "../Correctors/Corrector.h"
#include "../Continuous/Continuous.h"
#include "../Discretes/Discrete.h"
#include "../Averagers/Averager.h"
#include "../Calibrators/Calibrator.h"
#include "../Updaters/Updater.h"
#include "../Estimators/Estimator.h"
#include "../Estimators/MaximumLikelihood.h"
#include "../Smoothers/Smoother.h"
#include "../Field.h"
#include "../Deterministic.h"
#include "../ParameterIos/ParameterIo.h"
#include "../Regions/Region.h"

ConfigurationDefault::ConfigurationDefault(const Options& iOptions, const Data& iData) : Configuration(iOptions, iData),
      mNumOffsetsSpreadObs(0) {

   mSelectorCache.setName("selectorCache");
   mCorrectorCache.setName("correctorCache");
   // Selector
   {
      std::string tag;
      iOptions.getRequiredValue("selector", tag);
      mSelector = Selector::getScheme(tag, mData);
      addProcessor(mSelector, Component::TypeSelector);
   }
   // Correctors
   {
      std::vector<std::string> tags;
      iOptions.getValues("correctors", tags);
      for(int i = 0; i < (int) tags.size(); i++) {
         const Corrector* corrector = Corrector::getScheme(tags[i], mData);
         mCorrectors.push_back(corrector);
         addProcessor(corrector, Component::TypeCorrector);
      }
   }
   // Averager
   {
      std::string tag;
      iOptions.getRequiredValue("averager", tag);
      mAverager = Averager::getScheme(tag, mData);
      addProcessor(mAverager, Component::TypeAverager);
   }
   // Updaters
   {
      std::vector<std::string> tags;
      iOptions.getValues("updaters", tags);
      for(int i = 0; i < (int) tags.size(); i++) {
         Updater* updater = Updater::getScheme(tags[i], mData);
         mUpdaters.push_back(updater);
         addProcessor(updater, Component::TypeUpdater);
      }
   }
   // Smoother
   {
      std::vector<std::string> tags;
      iOptions.getValues("smoothers", tags);
      for(int i = 0; i < (int) tags.size(); i++) {
         Smoother* smoother = Smoother::getScheme(tags[i], mData);
         mSmoothers.push_back(smoother);
         addProcessor(smoother, Component::TypeSmoother);
      }
   }
   // Set up uncertainty object. In the future different ways of combining continuous and discretes
   // can be supported, but for now use UncertaintyCombine.
   {
      std::stringstream ss;
      ss << "tag=unc class=UncertaintyCombine ";
      std::string tag;
      bool continuous = false;
      bool lower      = false;
      bool upper      = false;
      bool discrete   = false;

      if(iOptions.getValue("continuous", tag)) {
         ss << "continuous=" << tag << " ";
         continuous = true;
      }
      if(iOptions.getValue("discreteLower", tag)) {
         ss << "discreteLower=" << tag << " ";
         lower = true;
      }
      if(iOptions.getValue("discreteUpper", tag)) {
         ss << "discreteUpper=" << tag << " ";
         upper = true;
      }
      if(iOptions.getValue("discrete", tag)) {
         ss << "discrete=" << tag << " ";
         discrete = true;
      }

      if(!continuous && !lower && !upper && !discrete) {
         std::stringstream ss;
         ss << "Configuration " << mName << " has no continuous or discrete models specified";
         Global::logger->write(ss.str(), Logger::error);
      }
      if(!continuous && (lower || upper)) {
         std::stringstream ss;
         ss << "Configuration " << mName << " has lower/upperDiscrete defined but no continuous ";
         ss << "model defined. Either provide a) 'continuous' and ('lowerDiscrete' and/or ";
         ss << "'upperDiscrete') or b) 'discrete'.";
      }
      if(continuous && discrete) {
         std::stringstream ss;
         ss << "Configuration " << mName << " has 'continuous' and 'discrete' specified'. ";
         ss << "Use 'lowerDiscrete' and/or 'upperDiscrete' instead of 'discrete'";
         Global::logger->write(ss.str(), Logger::error);
      }

      mUncertainty = Uncertainty::getScheme(Options(ss.str()), mData);
      addProcessor(mUncertainty, Component::TypeUncertainty);
   }

   // Calibrator
   {
      std::vector<std::string> tags;
      iOptions.getValues("calibrators", tags);
      for(int i = 0; i < (int) tags.size(); i++) {
         const Calibrator* calibrator = Calibrator::getScheme(tags[i], mData);
         mCalibrators.push_back(calibrator);
         addProcessor(calibrator, Component::TypeCalibrator);
      }
   }
   //! Across how many offsets should observations be allowed to be spread?
   //! For some stations, the obs occur less frequent than the output offsets. In these cases
   //! Parameters are usually never updated. Allow obs to be taken from neighbouring offsets.
   iOptions.getValue("numOffsetsSpreadObs", mNumOffsetsSpreadObs);

   init();
}
ConfigurationDefault::~ConfigurationDefault() {
   /*
   std::map<Component::Type, const EstimatorProbabilistic*>::iterator it;
   for(it = mEstimators.begin(); it != mEstimators.end(); it++) {
      delete it->second;
   }
  */
}

void ConfigurationDefault::getEnsemble(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable,
            Ensemble& iEnsemble,
            ProcTypeEns iType) const {

   int   locationCode = mRegion->find(iLocation);
   float offsetCode   = mRegion->find(iOffset);

   ////////////
   // Select //
   ////////////
   Parameters parSelector;
   int selectorIndex = 0; // Only one selector
   getParameters(Component::TypeSelector, iDate, iInit, offsetCode, locationCode, iVariable, selectorIndex, parSelector);
   Ensemble ensSelected = mSelector->select(iDate, iInit, iOffset, iLocation, iVariable, parSelector);

   if(iType == typeUnCorrected) {
      iEnsemble = ensSelected;
      return;
   }

   /////////////
   // Correct //
   /////////////
   Key::Ensemble key(iDate, iInit, iOffset, iLocation.getId(), iVariable);
   if(mCorrectorCache.isCached(key)) {
      const Ensemble& ens = mCorrectorCache.get(key);
      iEnsemble = ens;
   }
   else {
      Ensemble ensCorrected = ensSelected;
      // Do all correctors in sequence
      for(int i = 0; i < (int) mCorrectors.size(); i++) {
         Parameters parCorrector;
         getParameters(Component::TypeCorrector, iDate, iInit, offsetCode, locationCode, iVariable, i, parCorrector);
         mCorrectors[i]->correct(parCorrector, ensCorrected);

         // TODO: Remove old dates from cache that won't be read again (to save space)
      }
      iEnsemble = ensCorrected;
      mCorrectorCache.add(key, iEnsemble);
   }
}

Distribution::ptr ConfigurationDefault::getDistribution(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      std::string iVariable,
      ProcTypeDist iType) const {

   int   locationCode = mRegion->find(iLocation);
   float offsetCode   = mRegion->find(iOffset);

   Ensemble ens;
   getEnsemble(iDate, iInit, iOffset, iLocation, iVariable, ens);

   /////////////////
   // Uncertainty //
   /////////////////
   Parameters parUnc;
   getParameters(Component::TypeUncertainty, iDate, iInit, offsetCode, locationCode, iVariable, 0, parUnc);
   Parameters parAverager;
   getParameters(Component::TypeAverager, iDate, iInit, offsetCode, locationCode, iVariable, 0, parAverager);
   Distribution::ptr uncD = mUncertainty->getDistribution(ens, parUnc, *mAverager, parAverager);

   if(mCalibrators.size() == 0 && mUpdaters.size() == 0)
      return uncD;

   if(iType == typeUnCalibrated)
      return uncD;
   
   /////////////////
   // Calibration //
   /////////////////
   std::vector<Distribution::ptr> cal;
   cal.push_back(uncD);

   // Chain calibrators together
   for(int i = 0; i < (int) mCalibrators.size(); i++) {
      Parameters parCal;
      getParameters(Component::TypeCalibrator, iDate, iInit, offsetCode, locationCode, iVariable, i, parCal);

      cal.push_back(mCalibrators[i]->getDistribution(cal[i], parCal));
   }

   if(iType == typeUnUpdated)
      return cal.back();

   //////////////
   // Updating //
   //////////////
   for(int i = 0; i < (int) mUpdaters.size(); i++) {
      Parameters par;
      Obs recentObs;
      mData.getObs(iDate, iInit, 0, iLocation, iVariable, recentObs);
      Distribution::ptr recentDist = getDistribution(iDate, iInit, 0, iLocation, iVariable, typeUnUpdated);
      getParameters(Component::TypeUpdater, iDate, iInit, offsetCode, locationCode, iVariable, i, par);

      int Iupstream = cal.size()-1;
      assert(Iupstream >= 0);

      Distribution::ptr dist = mUpdaters[i]->getDistribution(cal[Iupstream], recentObs, recentDist, par);
      cal.push_back(dist);
   }
   return cal.back();
}

std::string ConfigurationDefault::toString() const {
   std::stringstream ss;
   ss << "      Selector:    " << mSelector->getSchemeName() << std::endl;
   ss << "      Downscaler:  " << mData.getDownscaler()->getSchemeName() << std::endl;
   ss << "      Correctors:  ";
   for(int i = 0; i < (int) mCorrectors.size(); i++) {
      ss << mCorrectors[i]->getSchemeName() << "+";
   }
   ss << std::endl;
   // TODO:
   ss << "      Uncertainty: " << mUncertainty->getSchemeName() << std::endl;
   ss << "      Calibrators: ";
   for(int i = 0; i < (int) mCalibrators.size(); i++) {
      ss << mCalibrators[i]->getSchemeName() << "+";
   }
   ss << std::endl;
   ss << "      Updaters:    ";
   for(int i = 0; i < (int) mUpdaters.size(); i++) {
      ss << mUpdaters[i]->getSchemeName();
   }
   ss << std::endl;
   ss << "      Smoother:    ";
   for(int i = 0; i < (int) mSmoothers.size(); i++) {
      ss << mSmoothers[i]->getSchemeName();
   }
   ss << std::endl;
   return ss.str();
}

void ConfigurationDefault::getDeterministic(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      std::string iVariable,
      Deterministic& iDeterministic) const {

   int   locationCode = mRegion->find(iLocation);
   float offsetCode   = mRegion->find(iOffset);

   Ensemble ens;
   getEnsemble(iDate, iInit, iOffset, iLocation, iVariable, ens);

   /////////////
   // Average //
   /////////////
   Parameters par;
   getParameters(Component::TypeAverager, iDate, iInit, offsetCode, locationCode, iVariable, 0, par);

   float value = mAverager->average(ens, par);
   iDeterministic = Deterministic(value, iDate, iInit, iOffset, iLocation, iVariable);
}


bool ConfigurationDefault::isValid(std::string& iMessage) const {
   iMessage = "";
   // Does selector always give the same sized ensemble
   if(!mSelector->isConstSize()) {
      // Selector ensemble changes in size. This means that correctors with parameters operating on
      // each member cannot be used
      for(int i = 0; i < (int) mCorrectors.size(); i++) {
         if(mCorrectors[i]->needsConstantEnsembleSize()) {
            iMessage = "Selector gives varying ensemble sizes. One or more corrector requires a constant ensemble size to estimate parameters";
            return false;
         }
      }
      if(mUncertainty->needsConstantEnsembleSize()) {
         iMessage = "Selector gives varying ensemble sizes. One or more corrector requires a constant ensemble size to estimate parameters";
         return false;
      }
   }

   return true;
}

void ConfigurationDefault::updateParameters(int iDate, int iInit, const std::vector<float>& iOffsets, const std::vector<Location>& iLocations, const std::string& iVariable) {
   int numValid = 0;

   // Get all observations
   std::set<float> allOffsetsSet;
   for(int o = 0; o < iOffsets.size(); o++) {
      float offset = fmod(iOffsets[o]+iInit,24);
      allOffsetsSet.insert(offset);
   }
   std::vector<float> allOffsets(allOffsetsSet.begin(), allOffsetsSet.end());

   std::vector<Obs> allObs;
   for(int o = 0; o < allOffsets.size(); o++) {
      float offset = allOffsets[o];
      for(int i = 0; i < iLocations.size(); i++) {
         Obs obs;
         mData.getObs(iDate, 0, offset, iLocations[i], iVariable, obs);
         allObs.push_back(obs);
         if(Global::isValid(obs.getValue()))
            numValid++;
      }
   }

   // Create a vector of all region ids
   std::set<int> regionsSet;
   std::map<int,std::vector<int> > regionLocationMap; // region id, location index
   for(int i = 0; i < iLocations.size(); i++) {
      int region = mRegion->find(iLocations[i]);
      regionsSet.insert(region);
      regionLocationMap[region].push_back(i);
   }
   std::vector<int> regions(regionsSet.begin(), regionsSet.end());

   // Assign which obs to use for each region/offset
   std::vector<std::vector<std::vector<Obs> > > useObs; // region, offset, obs
   useObs.resize(regions.size());
   std::vector<std::vector<int> > numValidObs; // region, offset
   numValidObs.resize(regions.size());
   for(int r = 0; r < regions.size(); r++) {
      useObs[r].resize(iOffsets.size());
      numValidObs[r].resize(iOffsets.size(), 0);
      int region = regions[r];
      // Loop over all output offsets
      for(int o = 0; o < iOffsets.size(); o++) {
         float offset = iOffsets[o];
         float offsetObs = fmod(iOffsets[o]+iInit,24);
         // Select obs for this location/offset
         for(int i = 0; i < allObs.size(); i++) {
            Obs obs = allObs[i];
            int currRegion = mRegion->find(obs.getLocation());
            if(currRegion == regions[r] && obs.getOffset() == offsetObs) {
               useObs[r][o].push_back(obs);
               numValidObs[r][o] += Global::isValid(obs.getValue());
            }
         }
      }
   }

   // Update the parameters by using a suitable set of observations
   for(int r = 0; r < regions.size(); r++) {
      int region = regions[r];
      // Loop over all output offsets
      for(int o = 0; o < iOffsets.size(); o++) {
         float offset = iOffsets[o];

         // Try to use the observations at this offset, but if there are none available, use
         // neighbouring offsets
         int useO = o;   // Use obs at this offset
         bool found = false;
         int counter = 0;
         while(counter <= mNumOffsetsSpreadObs && !found) {
            // Search for obs before (-1) and after (+1) the current offset
            for(int sign = -1; sign <= 1 && !found; sign += 2) {
               int currO = o + sign*counter;
               if(currO >= 0 && currO < iOffsets.size()) {
                  if(numValidObs[r][currO] > 0) {
                     // There are valid obs as iOffsets[currO]. Use these obs
                     found = true;
                     useO = currO;
                  }
               }
            }
            counter++;
         }

         float offsetObs = fmod(iOffsets[useO]+iInit,24);
         int   dateFcst = Global::getDate(iDate, 0, -(iOffsets[useO] - offsetObs));
         updateParameters(useObs[r][useO], dateFcst, iInit, iOffsets[useO], region, iVariable, iDate, iDate, offset, offset);
         if(!found) {
            std::stringstream ss;
            ss << "ConfigurationDefault: No obs available to region " << region << " offset " << offset;
            Global::logger->write(ss.str(), Logger::message);
         }
      }
   }

   if(numValid == 0) {
      std::stringstream ss;
      ss << "ConfigurationDefault: No observations available for updating for date " << iDate;
      Global::logger->write(ss.str(), Logger::warning);
   }
   else {
      std::stringstream ss;
      ss << "ConfigurationDefault: Updating using " << numValid << " valid observations for date " << iDate;
      Global::logger->write(ss.str(), Logger::status);
   }

   mParameters->write();
}

void ConfigurationDefault::updateParameters(const std::vector<Obs>& iObs, int iDate, int iInit, float iOffset, int iRegion, const std::string& iVariable, int iDateGet, int iDateSet, float iOffsetGet, float iOffsetSet) {
   const std::vector<Obs>& useObs = iObs;
   int dateFcst = iDate;
   float offset = iOffset;
   int region   = iRegion;
   iDate = iDateGet;

   bool needEnsemble = getNeedEnsemble();

   // Check that we are updating the right forecast
   for(int k = 0; k < useObs.size(); k++) {
      Obs obs = useObs[k];
      assert(Global::getDate(obs.getDate(), obs.getInit(), obs.getOffset()) == Global::getDate(dateFcst, iInit, offset) &&
            Global::getTime(obs.getDate(), obs.getInit(), obs.getOffset()) == Global::getTime(dateFcst, iInit, offset));
   }
   // Selector
   if(mSelector->needsTraining()) {
      Parameters par;
      getParameters(Component::TypeSelector, iDate, iInit, iOffsetGet, region, iVariable, 0, par);
      // Create vector date and offsets for selector's update parameters
      std::vector<int> fcstDates(iObs.size(), iDate);
      std::vector<float> fcstOffsets(iObs.size(), iOffset);
      mSelector->updateParameters(fcstDates, iInit, fcstOffsets, useObs, par);
      setParameters(Component::TypeSelector, iDate, iInit, iOffsetSet, region, iVariable, 0, par);
   }

   if(needEnsemble) {
      // Correctors
      std::vector<Ensemble> ensembles;
      for(int n = 0; n < useObs.size(); n++) {
         Location obsLocation = useObs[n].getLocation();
         // Get the raw ensemble
         Ensemble ensemble;
         getEnsemble(dateFcst, iInit, offset, obsLocation, iVariable, ensemble, typeUnCorrected);
         ensembles.push_back(ensemble);
      }
      for(int k = 0; k < (int) mCorrectors.size(); k++) {
         Parameters parCorrector;
         getParameters(Component::TypeCorrector, iDate, iInit, iOffsetGet, region, iVariable, k, parCorrector);
         Parameters parOrig = parCorrector;
         if(mCorrectors[k]->needsTraining()) {
            mCorrectors[k]->updateParameters(ensembles, useObs, parCorrector);
            setParameters(Component::TypeCorrector, iDate, iInit, iOffsetSet, region, iVariable, k, parCorrector);
         }

         // Correct ensemble for next corrector
         for(int n = 0; n < ensembles.size(); n++) {
            mCorrectors[k]->correct(parOrig, ensembles[n]);
         }
      }

      // Uncertainty
      Parameters parUncertainty;
      getParameters(Component::TypeUncertainty, iDate, iInit, iOffsetGet, region, iVariable, 0, parUncertainty);
      Parameters parAverager;
      getParameters(Component::TypeAverager, iDate, iInit, iOffsetGet, region, iVariable, 0, parAverager);
      if(mUncertainty->needsTraining()) {
         mUncertainty->updateParameters(ensembles, useObs, parUncertainty);
         setParameters(Component::TypeUncertainty, iDate, iInit, iOffsetSet, region, iVariable, 0, parUncertainty);
      }

      // Calibrators
      if(mAverager->needsTraining() || mCalibrators.size() > 0 || mUpdaters.size() > 0) {
         std::vector<std::vector<Distribution::ptr> > upstreams;
         upstreams.resize(1 + mCalibrators.size() + mUpdaters.size());
         upstreams[0].resize(useObs.size());
         for(int n = 0; n < useObs.size(); n++) {
            Distribution::ptr uncD = mUncertainty->getDistribution(ensembles[n], parUncertainty, *mAverager, parAverager);
            upstreams[0][n] = uncD;
         }

         // Start with the uncertainty distributions. Then iteratively calibrate the previous
         // distirbution (upstream).
         int Iupstream = 0;
         for(int k = 0; k < (int) mCalibrators.size(); k++) {
            upstreams[Iupstream+1].resize(useObs.size());
            Parameters parCalibrator;
            getParameters(Component::TypeCalibrator, iDate, iInit, iOffsetGet, region, iVariable, k, parCalibrator);
            if(mCalibrators[k]->needsTraining()) {
               mCalibrators[k]->updateParameters(upstreams[Iupstream], useObs, parCalibrator);
               setParameters(Component::TypeCalibrator, iDate, iInit, iOffsetSet, region, iVariable, k, parCalibrator);
            }
            // Calibrate all distributions for the next calibrator
            for(int n = 0; n < useObs.size(); n++) {
               upstreams[Iupstream+1][n] = mCalibrators[k]->getDistribution(upstreams[Iupstream][n], parCalibrator);
            }
            Iupstream++;
         }
         // Updaters
         if(mUpdaters.size() > 0) {
            assert(mUpdaters.size() <= 1);
            std::vector<Distribution::ptr> recentDists(useObs.size());
            std::vector<Obs> recentObs;
            for(int n = 0; n < useObs.size(); n++) {
               Distribution::ptr recentDist = getDistribution(dateFcst, iInit, 0, useObs[n].getLocation(), iVariable, typeUnUpdated);
               recentDists[n] = recentDist;
               // Get the recent most observation
               Obs obs;
               mData.getObs(dateFcst, iInit, 0, useObs[n].getLocation(), iVariable, obs);
               recentObs.push_back(obs);
            }
            for(int k = 0; k < mUpdaters.size(); k++) {
               upstreams[Iupstream+1].resize(useObs.size());
               Parameters par;
               getParameters(Component::TypeUpdater, iDate, iInit, iOffsetGet, region, iVariable, k, par);
               if(mUpdaters[k]->needsTraining()) {
                  mUpdaters[k]->updateParameters(upstreams[Iupstream], useObs, recentDists, recentObs, par);
                  setParameters(Component::TypeUpdater, iDate, iInit, iOffsetSet, region, iVariable, k, par);
               }
               Iupstream++;
            }
         }
         // Averager
         if(mAverager->needsTraining()) {
            // Get ensembles that are adjusted by the distribution
            std::vector<Ensemble> ensemblesAdj;
            for(int n = 0; n < useObs.size(); n++) {
               Ensemble ens = upstreams[upstreams.size()-1][n]->getEnsemble();
               ensemblesAdj.push_back(ens);
            }

            Parameters parAverager;
            getParameters(Component::TypeAverager, iDate, iInit, iOffsetGet, region, iVariable, 0, parAverager);
            mAverager->updateParameters(ensemblesAdj, useObs, parAverager);
            setParameters(Component::TypeAverager, iDate, iInit, iOffsetSet, region, iVariable, 0, parAverager);
         }
      }
   }
}

bool ConfigurationDefault::getNeedEnsemble() const {
   bool needEnsemble = false;
   for(int k = 0; k < (int) mCorrectors.size(); k++) {
      needEnsemble = mCorrectors[k]->needsTraining() ? true : needEnsemble;
   }
   needEnsemble = mUncertainty->needsTraining() ? true : needEnsemble;
   for(int k = 0; k < (int) mCalibrators.size(); k++) {
      needEnsemble = mCalibrators[k]->needsTraining() ? true : needEnsemble;
   }
   needEnsemble = mAverager->needsTraining() ? true : needEnsemble;
   return needEnsemble;
}
