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
#include "../Slice.h"
#include "../ParameterIos/ParameterIo.h"

ConfigurationDefault::ConfigurationDefault(const Options& iOptions, const Data& iData) : Configuration(iOptions, iData) {

   mSelectorCache.setName("selectorCache");
   mCorrectorCache.setName("correctorCache");
   // Selector
   {
      std::string tag;
      iOptions.getRequiredValue("selector", tag);
      mSelector = Selector::getScheme(tag, mData);
      addComponent(mSelector, Component::TypeSelector);
   }
   // Downscaler
   {
      std::string tag;
      iOptions.getRequiredValue("downscaler", tag);
      mDownscaler = Downscaler::getScheme(tag, mData);
      addComponent(mDownscaler, Component::TypeDownscaler);
   }
   // Correctors
   {
      std::vector<std::string> tags;
      iOptions.getValues("correctors", tags);
      for(int i = 0; i < (int) tags.size(); i++) {
         const Corrector* corrector = Corrector::getScheme(tags[i], mData);
         mCorrectors.push_back(corrector);
         addComponent(corrector, Component::TypeCorrector);
      }
   }
   // Averager
   {
      std::string tag;
      iOptions.getRequiredValue("averager", tag);
      mAverager = Averager::getScheme(tag, mData);
      addComponent(mAverager, Component::TypeAverager);
   }
   // Smoother
   {
      std::vector<std::string> tags;
      iOptions.getValues("smoothers", tags);
      for(int i = 0; i < (int) tags.size(); i++) {
         Smoother* smoother = Smoother::getScheme(tags[i], mData);
         mSmoothers.push_back(smoother);
         addComponent(smoother, Component::TypeSmoother);
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
      addComponent(mUncertainty, Component::TypeUncertainty);
   }

   // Calibrator
   {
      std::vector<std::string> tags;
      iOptions.getValues("calibrators", tags);
      for(int i = 0; i < (int) tags.size(); i++) {
         const Calibrator* calibrator = Calibrator::getScheme(tags[i], mData);
         mCalibrators.push_back(calibrator);
         addComponent(calibrator, Component::TypeCalibrator);
      }
   }

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
            Ensemble& iEnsemble) const {

   std::vector<Slice> slices;
   getSelectorIndicies(iDate, iInit, iOffset, iLocation, iVariable, slices);
   std::vector<float> ensArray;
   std::vector<float> skillArray;
   Ensemble ensDownscaled;

   ///////////////
   // Downscale //
   ///////////////
   if(slices.size() == 0) {
      ensArray.push_back(Global::MV);
      skillArray.push_back(Global::MV);
   }
   else {
      Parameters parDownscaler;
      int downscalerIndex = 0; // Only one downscaler
      getParameters(Component::TypeDownscaler, iDate, iInit, iOffset, iLocation, iVariable, downscalerIndex, parDownscaler);
      for(int i = 0; i < (int) slices.size(); i++) {
         Slice slice = slices[i];
         float value = mDownscaler->downscale(slice, iVariable, iLocation, parDownscaler);
         ensArray.push_back(value);
         skillArray.push_back(slice.getSkill());
      }
   }
   ensDownscaled.setValues(ensArray);
   ensDownscaled.setSkills(skillArray);
   ensDownscaled.setInfo(iDate, iInit, iOffset, iLocation, iVariable);

   /////////////
   // Correct //
   /////////////
   Key::Ensemble key(iDate, iInit, iOffset, iLocation.getId(), iVariable);
   if(mCorrectorCache.isCached(key)) {
      const Ensemble& ens = mCorrectorCache.get(key);
      iEnsemble = ens;
   }
   else {
      Ensemble ensCorrected = ensDownscaled;
      // Do all correctors in sequence
      for(int i = 0; i < (int) mCorrectors.size(); i++) {
         Parameters parCorrector;
         getParameters(Component::TypeCorrector, iDate, iInit, iOffset, iLocation, iVariable, i, parCorrector);
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
      std::string iVariable) const {
   Ensemble ens;
   getEnsemble(iDate, iInit, iOffset, iLocation, iVariable, ens);

   /////////////////
   // Uncertainty //
   /////////////////
   Parameters parUnc;
   getParameters(Component::TypeUncertainty, iDate, iInit, iOffset, iLocation, iVariable, 0, parUnc);
   Distribution::ptr uncD = mUncertainty->getDistribution(ens, parUnc);

   if(mCalibrators.size() == 0)
      return uncD;
   
   /////////////////
   // Calibration //
   /////////////////
   std::vector<Distribution::ptr> cal;
   cal.push_back(uncD);

   // Chain calibrators together
   for(int i = 0; i < (int) mCalibrators.size(); i++) {
      Parameters parCal;
      getParameters(Component::TypeCalibrator, iDate, iInit, iOffset, iLocation, iVariable, i, parCal);

      cal.push_back(mCalibrators[i]->getDistribution(cal[i], parCal));
   }
   return cal.back();
}

std::string ConfigurationDefault::toString() const {
   std::stringstream ss;
   ss << "      Selector:    " << mSelector->getSchemeName() << std::endl;
   ss << "      Downscaler:  " << mDownscaler->getSchemeName() << std::endl;
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
   //ss << "      Updater:     " << mUpdater->getSchemeName() << std::endl;
   ss << "      Smoother:    ";
   for(int i = 0; i < (int) mSmoothers.size(); i++) {
      ss << mSmoothers[i]->getSchemeName();
   }
   ss << std::endl;
   return ss.str();
}

float ConfigurationDefault::getDeterministic(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      std::string iVariable) const {
   Ensemble ens;
   getEnsemble(iDate, iInit, iOffset, iLocation, iVariable, ens);

   /////////////
   // Average //
   /////////////
   Parameters par;
   getParameters(Component::TypeAverager, iDate, iInit, iOffset, iLocation, iVariable, 0, par);

   return mAverager->average(ens, par);
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

void ConfigurationDefault::getSelectorIndicies(int iDate,
      int iInit,
      float iOffset, 
      const Location& iLocation,
      std::string iVariable,
      std::vector<Slice>& iSlices) const {

   // If selector gives the same results regardless of location or offset
   // only compute these once
   int locationId = mSelector->isLocationDependent() ? iLocation.getId() : 0;
   float offset   = mSelector->isOffsetDependent()   ? iOffset : 0;

   Key::Ensemble key(iDate, iInit, offset, locationId, iVariable);

   // Get selector indices
   if(mSelectorCache.isCached(key)) {
      const std::vector<Slice>& slices = mSelectorCache.get(key);
      iSlices = slices;
   }
   else {
      Parameters parSelector;
      int selectorIndex = 0; // Only one selector
      getParameters(Component::TypeSelector, iDate, iInit, offset, iLocation, iVariable, selectorIndex, parSelector);
      mSelector->select(iDate, iInit, offset, iLocation, iVariable, parSelector, iSlices);
      mSelectorCache.add(key, iSlices);
   }

   // TODO: Is this the best way to interpret isOffsetDependent? Because it could also
   // mean that selector returns the same offset in the slice regardless of the search offset
   if(!mSelector->isOffsetDependent()) {
      // Adjust offset back
      for(int i = 0; i < (int) iSlices.size(); i++) {
         iSlices[i].setOffset(iOffset);
      }
   }

   // Check that selector doesn't violate by selecting future indices
   // Perhaps this slows it down?
   if(0 && !mSelector->allowedToCheat()) {
      for(int i = 0; i < (int) iSlices.size(); i++) {
         // TODO:
         // Its ok to pick future dates if forecasts
         //if(iSlices[i].getMember().getDataset()) {
         //}

         float timeDiff = Global::getTimeDiff(iDate, iInit, iInit, iSlices[i].getDate(), iSlices[i].getInit(), iSlices[i].getOffset());
         if(timeDiff < 0) {
            std::stringstream ss;
            ss << "ConfigurationDefault: Selector picked a date/offset ("
               << iSlices[i].getDate() << " " << iSlices[i].getOffset() << ") "
               << "which would not be available operationally for date "
               << iDate;
            Global::logger->write(ss.str(), Logger::error);
         }
      }
   }
}

void ConfigurationDefault::updateParameters(const std::vector<Obs>& iObs, int iDate, int iInit) {

   std::vector<float> offsets;
   mData.getInput()->getOffsets(offsets);
   assert(offsets.size() > 0);
   float lastOffset = offsets[offsets.size()-1];
   int numDays = ceil(lastOffset / 24);
#if 0
   std::map<float,std::vector<std::vector<Obs> > > parObs; // offset, index, obs
   for(int o = 0; o < offsets.size(); o++) {
      parObs[offsets[o]].resize(mFinder->size());
   }

   // Assign each obs to a parameter index
   for(int i = 0; i < (int) iObs.size(); i++) {
      int index = mFinder->find(iObs[i].getLocation());
      float obsOffset = iObs[i].getOffset();
      // Duplicate obs for later offsets
      for(int d = 0; d <= numDays; d++) {
         float offset = obsOffset + d*24;
         if(offset <= lastOffset) {
            parObs[offset][index].push_back(iObs[i]);
         }
      }
   }

   // Loop over offsets
   for(int o = 0; o < (int) parObs.size(); o++) {
      float offset = offsets[o];
      // Loop over indices
      for(int index = 0; index < (int) mFinder->size(); index++) {
         int init = 0;
         const std::vector<Obs>& currObs = parObs[o][index];

         // Downscaler
         const Downscaler* downscaler = mConfiguration.getDownscaler();
         /*
         if(downscaler->needsTraining()) {
            Parameters parDownscaler;
            getParameters(Component::TypeDownscaler, iDate, iInit, offset, index, variable, parDownscaler);
            std::vector<Slice> allSlices;

            // Loop over all obs
            for(int k = 0; k < (int) currObs.size(); k++) {
               std::vector<Slice> slices;
               getSelectorIndicies(location, iDate, iInit, offset, variable, slices);
               for(int j = 0; j < (int) slices.size(); j++) {
                  allSlices.push_back(slices[j]);
               }
               downscaler->updateParameters(allSlices, variable, , obs, parDownscaler);
            }
            setParameters(Component::TypeDownscaler, iDate, iInit, offset, location, variable, parDownscaler);
         }
         */
         // Corrector
         Ensemble unCorrected;
         getDownscaledValues(location, iDate, iInit, offset, variable, unCorrected);
         const Corrector* corrector = mConfiguration.getCorrector();

         Parameters parCorrector;
         getParameters(Component::TypeCorrector, iDate, iInit, offset, location, variable, parCorrector);
         corrector->updateParameters(unCorrected, currObs, parCorrector);
         setParameters(Component::TypeCorrector, iDate, iInit, offsetFcst, location, variable, parCorrector);
      }
   }
#else
   for(int i = 0; i < (int) iObs.size(); i++) {
      Obs obs = iObs[i];
      int date = obs.getDate();
      float offset = iObs[i].getOffset();
      // Update using todays obs only
      if(date <= iDate) {
         // Reuse obs for all forecast days
         for(int d = 0; d <= numDays; d++) {
            float offsetFcst   = offset + 24*d;
            int   dateFcst     = Global::getDate(date, 0, -24*d);
            // Check that we are updating the right forecast
            assert(Global::getDate(iObs[i].getDate(), 0, iObs[i].getOffset()) == Global::getDate(dateFcst, 0, offsetFcst) &&
                   Global::getTime(iObs[i].getDate(), 0, iObs[i].getOffset()) == Global::getTime(dateFcst, 0, offsetFcst));
            if(offsetFcst <= lastOffset) {

               // Compute date/offset for which parameters to adjust
               int   dateParGet  = Global::getDate(iDate, -24*(d+1));
               float offsetParGet = offset + 24*d;
               int   dateParPut   = Global::getDate(iDate, -24*(d));
               float offsetParPut = offset + 24*d;

               std::stringstream ss;
               ss << "Updating parameters [" << dateParGet << " " << offsetParGet << " " << dateParPut << " " << offsetParPut << "]";
               Global::logger->write(ss.str(), Logger::message);

               assert(offset < 24);
               std::vector<Obs> currObs;
               currObs.push_back(obs);

               Location location = obs.getLocation();
               int init = 0;
               std::string  variable = obs.getVariable();

               // Selector
               if(mSelector->needsTraining()) {
                  Parameters par;
                  getParameters(Component::TypeSelector, iDate, iInit, offsetFcst, location, variable, 0, par);
                  mSelector->updateParameters(dateFcst, iInit, offsetFcst, location, variable, currObs, par);
                  setParameters(Component::TypeSelector, iDate, iInit, offsetFcst, location, variable, 0, par);
               }

               // Downscaler
               if(mDownscaler->needsTraining()) {
                  std::vector<Slice> slices;
                  getSelectorIndicies(dateFcst, iInit, offsetFcst, location, variable, slices);
                  Parameters par;
                  getParameters(Component::TypeDownscaler, iDate, iInit, offsetFcst, location, variable, 0, par);
                  // TODO: slices[0]
                  for(int s = 0; s < slices.size(); s++) {
                     mDownscaler->updateParameters(slices[s], variable, location, obs, par);
                  }
                  setParameters(Component::TypeDownscaler, iDate, iInit, offsetFcst, location, variable, 0, par);
               }

               // Determine if we need to get the ensemble
               bool needEnsemble = false;
               for(int k = 0; k < (int) mCorrectors.size(); k++) {
                  needEnsemble = mCorrectors[k]->needsTraining() ? true : needEnsemble;
               }
               needEnsemble = mAverager->needsTraining() ? true : needEnsemble;
               needEnsemble = mUncertainty->needsTraining() ? true : needEnsemble;
               for(int k = 0; k < (int) mCalibrators.size(); k++) {
                  needEnsemble = mCalibrators[k]->needsTraining() ? true : needEnsemble;
               }

               if(needEnsemble) {
                  // Correctors
                  std::vector<Ensemble> ensembles;
                  Ensemble ensemble;
                  getEnsemble(dateFcst, iInit, offsetFcst, location, variable, ensemble);
                  ensembles.push_back(ensemble);
                  for(int k = 0; k < (int) mCorrectors.size(); k++) {
                     if(mCorrectors[k]->needsTraining()) {

                        Parameters parCorrector;
                        getParameters(Component::TypeCorrector, iDate, iInit, offsetFcst, location, variable, k, parCorrector);
                        // TODO: Does unCorrected have the right offset?? I think so yes because it gets set
                        // above
                        Parameters parOrig = parCorrector;
                        mCorrectors[k]->updateParameters(ensembles, currObs, parCorrector);
                        setParameters(Component::TypeCorrector, iDate, iInit, offsetFcst, location, variable, k, parCorrector);
                        // Correct ensemble for next corrector
                        mCorrectors[k]->correct(parOrig, ensemble);
                     }
                  }

                  // Averager
                  if(mAverager->needsTraining()) {
                     Parameters parAverager;
                     getParameters(Component::TypeAverager, iDate, iInit, offsetFcst, location, variable, 0, parAverager);
                     mAverager->updateParameters(ensemble, currObs, parAverager);
                     setParameters(Component::TypeAverager, iDate, iInit, offsetFcst, location, variable, 0, parAverager);
                  }

                  // Uncertainty
                  Parameters parUncertainty;
                  getParameters(Component::TypeUncertainty, iDate, iInit, offsetFcst, location, variable, 0, parUncertainty);
                  Distribution::ptr uncD = mUncertainty->getDistribution(ensemble, parUncertainty);

                  if(mUncertainty->needsTraining()) {
                     mUncertainty->updateParameters(ensemble, obs, parUncertainty);
                     setParameters(Component::TypeUncertainty, iDate, iInit, offsetFcst, location, variable, 0, parUncertainty);
                  }

                  // Calibrators
                  std::vector<Distribution::ptr> calDs;
                  calDs.push_back(uncD);
                  for(int k = 0; k < (int) mCalibrators.size(); k++) {
                     Distribution::ptr upstream = calDs[k];
                     Parameters parCalibrator;
                     getParameters(Component::TypeCalibrator, iDate, iInit, offsetFcst, location, variable, k, parCalibrator);
                     calDs.push_back(mCalibrators[k]->getDistribution(upstream, parCalibrator));
                     if(mCalibrators[k]->needsTraining()) {
                        mCalibrators[k]->updateParameters(upstream, obs, parCalibrator);
                        setParameters(Component::TypeCalibrator, iDate, iInit, offsetFcst, location, variable, k, parCalibrator);
                     }
                  }
               }
            }
         }
      }
   }
#endif
   mParameters->write();
}

void ConfigurationDefault::updateParameters(const std::vector<Location>& iLocations, std::string iVariable, int iDate) {

   // Loop over every offset
   // Loop over every location

   std::vector<float> offsets;
   mData.getOutputOffsets(offsets);
   int init = 0;
   int numValid = 0;

   for(int i = 0; i < iLocations.size(); i++) {
      Location location = iLocations[i];
      for(int o = 0; o < offsets.size(); o++) {
         float offset = offsets[o];
         // Get the obs for today
         int dateObs   = iDate;
         float offsetObs = fmod(offsets[o],24);
         Obs obs;
         mData.getObs(dateObs, init, offsetObs, iLocations[i], iVariable, obs);

         // Figure out what forecast this obs corresponds to
         float offsetFcst = offset;
         int   dateFcst   = Global::getDate(iDate, 0, -(offset - offsetObs));

         // Check that we are updating the right forecast
         assert(Global::getDate(obs.getDate(), 0, obs.getOffset()) == Global::getDate(dateFcst, 0, offsetFcst) &&
                Global::getTime(obs.getDate(), 0, obs.getOffset()) == Global::getTime(dateFcst, 0, offsetFcst));

         if(Global::isValid(obs.getValue())) {
            numValid++;
         }

         // Put the one obs in a vector
         std::vector<Obs> currObs;
         currObs.push_back(obs);

         // Selector
         if(mSelector->needsTraining()) {
            Parameters par;
            getParameters(Component::TypeSelector, iDate, init, offsetFcst, location, iVariable, 0, par);
            mSelector->updateParameters(dateFcst, init, offsetFcst, location, iVariable, currObs, par);
            setParameters(Component::TypeSelector, iDate, init, offsetFcst, location, iVariable, 0, par);
         }

         // Downscaler
         if(mDownscaler->needsTraining()) {
            std::vector<Slice> slices;
            getSelectorIndicies(dateFcst, init, offsetFcst, location, iVariable, slices);
            Parameters par;
            getParameters(Component::TypeDownscaler, iDate, init, offsetFcst, location, iVariable, 0, par);
            // TODO: slices[0]
            for(int s = 0; s < slices.size(); s++) {
               mDownscaler->updateParameters(slices[s], iVariable, location, obs, par);
            }
            setParameters(Component::TypeDownscaler, iDate, init, offsetFcst, location, iVariable, 0, par);
         }

         // Determine if we need to get the ensemble
         bool needEnsemble = false;
         for(int k = 0; k < (int) mCorrectors.size(); k++) {
            needEnsemble = mCorrectors[k]->needsTraining() ? true : needEnsemble;
         }
         needEnsemble = mAverager->needsTraining() ? true : needEnsemble;
         needEnsemble = mUncertainty->needsTraining() ? true : needEnsemble;
         for(int k = 0; k < (int) mCalibrators.size(); k++) {
            needEnsemble = mCalibrators[k]->needsTraining() ? true : needEnsemble;
         }

         if(needEnsemble) {
            // Correctors
            std::vector<Ensemble> ensembles;
            Ensemble ensemble;
            getEnsemble(dateFcst, init, offsetFcst, location, iVariable, ensemble);
            ensembles.push_back(ensemble);
            for(int k = 0; k < (int) mCorrectors.size(); k++) {
               if(mCorrectors[k]->needsTraining()) {

                  Parameters parCorrector;
                  getParameters(Component::TypeCorrector, iDate, init, offsetFcst, location, iVariable, k, parCorrector);
                  // TODO: Does unCorrected have the right offset?? I think so yes because it gets set
                  // above
                  Parameters parOrig = parCorrector;
                  mCorrectors[k]->updateParameters(ensembles, currObs, parCorrector);
                  setParameters(Component::TypeCorrector, iDate, init, offsetFcst, location, iVariable, k, parCorrector);
                  // Correct ensemble for next corrector
                  mCorrectors[k]->correct(parOrig, ensemble);
               }
            }

            // Averager
            if(mAverager->needsTraining()) {
               Parameters parAverager;
               getParameters(Component::TypeAverager, iDate, init, offsetFcst, location, iVariable, 0, parAverager);
               mAverager->updateParameters(ensemble, currObs, parAverager);
               setParameters(Component::TypeAverager, iDate, init, offsetFcst, location, iVariable, 0, parAverager);
            }

            // Uncertainty
            Parameters parUncertainty;
            getParameters(Component::TypeUncertainty, iDate, init, offsetFcst, location, iVariable, 0, parUncertainty);
            Distribution::ptr uncD = mUncertainty->getDistribution(ensemble, parUncertainty);

            if(mUncertainty->needsTraining()) {
               mUncertainty->updateParameters(ensemble, obs, parUncertainty);
               setParameters(Component::TypeUncertainty, iDate, init, offsetFcst, location, iVariable, 0, parUncertainty);
            }

            // Calibrators
            std::vector<Distribution::ptr> calDs;
            calDs.push_back(uncD);
            for(int k = 0; k < (int) mCalibrators.size(); k++) {
               Distribution::ptr upstream = calDs[k];
               Parameters parCalibrator;
               getParameters(Component::TypeCalibrator, iDate, init, offsetFcst, location, iVariable, k, parCalibrator);
               calDs.push_back(mCalibrators[k]->getDistribution(upstream, parCalibrator));
               if(mCalibrators[k]->needsTraining()) {
                  mCalibrators[k]->updateParameters(upstream, obs, parCalibrator);
                  setParameters(Component::TypeCalibrator, iDate, init, offsetFcst, location, iVariable, k, parCalibrator);
               }
            }
         }
      }
   }

   if(numValid == 0) {
      std::stringstream ss;
      ss << "ConfigurationDefault: No observations available for updating for date " << iDate;
      Global::logger->write(ss.str(), Logger::warning);
   }
   mParameters->write();
}