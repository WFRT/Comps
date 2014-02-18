#include "Selector.h"
#include "Analog.h"
#include "../Averagers/Averager.h"
#include "../Variables/Variable.h"
#include "../DetMetrics/DetMetric.h"
#include "../Ensemble.h"
#include "../Scheme.h"
#include "../Data.h"
#include "../Member.h"
#include "../Field.h"

SelectorAnalog::SelectorAnalog(const Options& iOptions, const Data& iData) :
      Selector(iOptions, iData),
      mCheckIfObsExists(false),
      mDayWidth(365),
      mLocationIndependent(false),
      mAdjustOffset(0),
      mOffsetIndependent(false),
      mDoObsForward(false),
      mObsInput(NULL),
      mComputeVariableVariances(false),
      mDontNormalize(false) {
   std::string metric;
   //! Tag of metric to evaluate analog similarity
   iOptions.getRequiredValue("analogMetric", metric);
   //! Number of analogs to include in the ensemble
   iOptions.getRequiredValue("numAnalogs", mNumAnalogs);
   //! If true, weighs variables evenly
   iOptions.getValue("dontNormalize", mDontNormalize);
   //! Only find analogs within +- number of days
   iOptions.getValue("dayWidth", mDayWidth);
   iOptions.getValue("locationIndependent", mLocationIndependent);
   //! Should the same analogs be picked for all offsets?
   iOptions.getValue("offsetIndependent", mOffsetIndependent);
   iOptions.getValue("doObsForward", mDoObsForward);
   //! Adaptively compute variable weights, instead of using
   //! constant variances from variable namelist
   iOptions.getValue("computeVariableVariances", mComputeVariableVariances);
   if(mComputeVariableVariances)
      Component::underDevelopment();

   //! Use this (forecast) dataset to match analogs. Otherwise, use the default datasets specified
   //! in the run
   iOptions.getValue("dataset", mDataset);

   Options optDetMetric;
   Scheme::getOptions(metric, optDetMetric);
   mMetric = DetMetric::getScheme(optDetMetric);

   iOptions.getValue("adjustOffset", mAdjustOffset);

   // Averager used for analog
   std::string averagerTag;
   averagerTag = "mean";
   //! Tag of method for averaging over ensemble members when searching for analogs
   iOptions.getValue("averager", averagerTag);
   Options optAverager;
   Scheme::getOptions(averagerTag, optAverager);
   mAverager = Averager::getScheme(optAverager, iData);
   if(mAverager->needsTraining()) {
      Global::logger->write("SelectorAnalog does not currently support averagers with parameters.", Logger::error);
   }

   //! Which variables should be used to search for analogs?
   iOptions.getRequiredValues("variables", mVariables);
   // Check that the input has these variables
   std::vector<std::string> allVariables = mData.getInput()->getVariables();
   bool hasAllVariables = true;
   std::stringstream ssMissingVariables;
   for(int i = 0; i < (int) mVariables.size(); i++) {
      std::vector<std::string>::const_iterator it = find(allVariables.begin(), allVariables.end(), mVariables[i]);
      if(it == allVariables.end()) {
         hasAllVariables = false;
         ssMissingVariables << " " << mVariables[i];
      }
   }
   if(!hasAllVariables) {
      std::stringstream ss;
      ss << "SelectorAnalog: Input dataset " << mData.getInput()->getName();
      ss << " does not have analog variable(s): " << ssMissingVariables.str();
      Global::logger->write(ss.str(), Logger::error);
   }

   // Use even mWeights
   for(int i = 0; i < (int) mVariables.size(); i++) {
      if(!mDontNormalize) {
         const Variable* var = Variable::get(mVariables[i]);
         if(!Global::isValid(var->getStd())) {
            std::stringstream ss;
            ss << "SelectorAnalog: Variable " << var->getName() << " does not have a std specified. ";
            ss << "Therefore the skills of the analog variables cannot be normalized.";
            Global::logger->write(ss.str(), Logger::error);
         }
         mWeights.push_back(1/var->getStd());
      }
      else {
         mWeights.push_back(1);
      }
   }

   // Which dataset to use as obs
   mObsInput = mData.getObsInput();
   if(mObsInput == NULL) {
      std::stringstream ss;
      ss << "SelectorAnalog: No observation dataset specified. Cannot produce forecasts.";
      Global::logger->write(ss.str(), Logger::error);
   }

   // Offsets
   std::vector<float> offsets = mData.getInput()->getOffsets();
   mAllOffsets = offsets;

   mCache.setName("SelectorAnalog");
}
SelectorAnalog::~SelectorAnalog() {
   delete mAverager;
   delete mMetric;
}
void SelectorAnalog::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

   // Weights
   std::vector<float> weights(mVariables.size());
   if(mComputeVariableVariances) {
      std::vector<float> values = iParameters.getAllParameters();
      assert(values.size() == mVariables.size()*2);
      for(int i = 0; i < (int) values.size()/2; i++) {
         // variance = E(X^2) - E(X)^2
         float currVariance = iParameters[1 + i*2] - pow(iParameters[0 + i*2],2);
         assert(currVariance > 0);
         weights[i] = 1 / pow(currVariance, 0.5);
      }
   }
   else {
      weights = mWeights;
   }
   assert(weights.size() == mVariables.size());

   if(!mData.hasVariable(iVariable, Input::typeObservation)) {
      std::stringstream ss;
      ss << "SelectorAnalog: Variable " << iVariable << " is not available in any observation datasets. ";
      ss << "Therefore no analog forecasts can be created.";
      //Global::logger->write(ss.str(), Logger::critical);
   }

   std::vector<int> dates;
   mData.getInput()->getDates(dates);
   std::stringstream ss;
   ss << "Analog: Searching through " << dates.size() << " potential analogs";
   Global::logger->write(ss.str(), Logger::debug);
   std::vector<float> scores;

   // Get reference location
   std::vector<Location> locations;
   mData.getInput()->getSurroundingLocations(iLocation, locations, 1);
   Location location = locations[0];
   std::vector<float> targetValues;
   std::vector<std::pair<int, float> > metrics;

   // Adjust offset, by looking earlier in time
   float offsetAdjusted = iOffset - mAdjustOffset;
   if(offsetAdjusted < 0) {
      std::stringstream ss;
      ss << "SelectorAnalog: adjustOffset is greater than retrival offset. Pull offset 0.";
      Global::logger->write(ss.str(), Logger::warning);
   }

   // Determine which offsets we should use to find analogs
   // Store as indices into 'offsets' array
   // TODO: The method of using useOffset bypasses interpolation done by Data. Therefore, forecasts
   // will only be created for offsets that exists for mainInput.
   std::vector<int> useOffsets;
   if(isOffsetDependent()) {
      int index = mData.getInput()->getNearestOffsetIndex(offsetAdjusted);
      if(Global::isValid(index)) {
         useOffsets.push_back(index);
      }
   }
   else {
      if(mDoObsForward) {
         // Only use offsets less than 24h
         for(int i = 0; i < (int) mAllOffsets.size(); i++) {
            if(mAllOffsets[i] < 24) {
               useOffsets.push_back(i);
            }
         }
      }
      else {
         useOffsets.resize(mAllOffsets.size());
         for(int i = 0; i < (int) mAllOffsets.size(); i++) {
            useOffsets[i] = i;
         }
      }
   }

   // Compute target values
   // Only store the variables that are not missing. Store the available offset/variable
   // combinations in availOffsets and availVariables
   int targetDate;
   if(mDoObsForward) {
      targetDate = Global::getDate(iDate, -24);
   }
   else {
      targetDate = iDate;
   }
   int targetInit = iInit;
   std::vector<int> availOffsets;
   std::vector<int> availVariables;
   for(int o = 0; o < (int) useOffsets.size(); o++) {
      int oI = useOffsets[o];
      float targetOffset = mAllOffsets[oI]; 
      for(int v = 0; v < (int) mVariables.size(); v++) {
         std::vector<float> ensValues;
         Ensemble ensemble;
         if(mDataset == "")
            mData.getEnsemble(targetDate, targetInit, targetOffset, location, mVariables[v], Input::typeForecast, ensemble);
         else
            mData.getEnsemble(targetDate, targetInit, targetOffset, location, mDataset, mVariables[v], ensemble);
         float value = ensemble.getMoment(1);
         if(value != Global::MV) {
            value *= mWeights[v];
            targetValues.push_back(value);
            availOffsets.push_back(oI);
            availVariables.push_back(v);
         }
      }
   }
   if(availVariables.size() == 0) {
      std::stringstream ss;
      ss << "No forecast data available for " << iDate << " " << iOffset << " " << location.getId() << ": No analogs found";
      Global::logger->write(ss.str(), Logger::warning);
      return;
   }

   // Compute metrics for all potential analogs
   double totalTime = 0;
   std::vector<float> useValues;
   useValues.resize(targetValues.size());

   for(int i = 0; i < (int) dates.size(); i++) {
      int currDate = dates[i];
      bool isValidDate = true;
      double currTime = Global::clock();
      // TODO: Date selection code here

      bool validDayDiff = true;
      if(mDayWidth < 365) {
         int dayDiff = fabs(Global::getJulianDay(iDate) - Global::getJulianDay(currDate));
         if(dayDiff > 180)
            dayDiff = 365 - dayDiff;
         validDayDiff = (dayDiff <= mDayWidth);
      }

      if(currDate < iDate && validDayDiff) {

         // Only keep availOffsets and availVariables
         for(int k = 0; k < (int) availOffsets.size(); k++) {
            int oI = availOffsets[k];
            int v  = availVariables[k];
            const std::vector<float>& values = getData(currDate, iInit, oI, location);

            int currDateWithOffset = Global::getDate(currDate, iInit, mAllOffsets[oI]);
            // Invalid date if in the future
            if(currDateWithOffset > iDate || (currDateWithOffset == iDate && mAllOffsets[oI] > 0)) {
               isValidDate = false;
               break;
            }
            else {
               int index = v;
               assert(index >= 0 && index < values.size());
               float value = values[index];
               if(value != Global::MV) {
                  value *= mWeights[v];
               }
               else {
                  isValidDate = false;
                  break;
               }
               useValues[k] = value;
            }
         }

         if(isValidDate) {
            float metric = mMetric->compute(targetValues, useValues, iVariable);
            std::pair<int,float> p(currDate, metric);
            metrics.push_back(p);
         }
      }
      double endTime = Global::clock();
      totalTime += endTime - currTime;
   }

   // Analog selection: select the slices with the best metric
   std::sort(metrics.begin(), metrics.end(), Global::sort_pair_second<int, float>());
   int i = 0;
   while(iFields.size() < mNumAnalogs && i < (int) metrics.size()) {
      int date = metrics[i].first;
      float skill = metrics[i].second;
      if(skill != Global::MV) {
         //std::cout << "Best date:  " << date << " -- " << metrics[i].second << std::endl;
         // TODO
         float analogInit = iInit;
         float analogOffset;
         Member member(mObsInput->getName());
         int analogDate;
         if(mDoObsForward) {
            analogOffset = fmod(iOffset, 24);
            analogDate = Global::getDate(date, 24);
         }
         else {
            analogOffset = iOffset;
            analogDate = date;
         }
         // Check if observation is defined
         // TODO: There's already code in Selector that will do this
         if(mCheckIfObsExists) {
            Input* input = mData.getInput(iVariable, Input::typeObservation);
            assert(input);
            assert(input->getName() == iLocation.getDataset());
            if(input->getValue(analogDate, analogInit, analogOffset, iLocation.getId(), 0, iVariable) != Global::MV) {
               Field slice(analogDate, analogInit, analogOffset, member, skill);
               iFields.push_back(slice);
            }
            else {
               //std::cout << "SelectorAnalog: No obs found for: " << analogDate << " " << analogOffset << " " << iLocation.getId() << " " << iVariable<< std::endl;
            }
         }
         else {
            Field slice(analogDate, analogInit, analogOffset, member, skill);
            iFields.push_back(slice);
         }
      }
      i++;
   }
}
bool SelectorAnalog::isLocationDependent() const {
   return !mLocationIndependent;
}
bool SelectorAnalog::isOffsetDependent() const {
   return !mOffsetIndependent;
}
void SelectorAnalog::getDefaultParameters(Parameters& iParameters) const {
   if(mComputeVariableVariances) {
      std::vector<float> parameters;
      int V = (int) mVariables.size();

      // Variable variance parameters
      std::vector<float> params;
      for(int v = 0; v <  V; v++) {
         const Variable* var = Variable::get(mVariables[v]);
         float mean = var->getMean();
         float variance = var->getStd()*var->getStd();
         float mean2 = variance + mean*mean; // variance = E(X^2) - E(X)^2

         if(!Global::isValid(mean) || !Global::isValid(var->getStd())) {
            std::stringstream ss;
            ss << "SelectorAnalog: Variable " << var->getName() << " does not have a std or mean specified. ";
            ss << "These values will be initialized to 1. A training period will therefore be needed.";
            Global::logger->write(ss.str(), Logger::warning);
         }
         if(!Global::isValid(mean))
            mean = 1;
         if(!Global::isValid(mean2))
            mean2 = 1;
         params.push_back(mean);        // E(X)
         params.push_back(mean2);       // E(X^2)
      }
      iParameters.setAllParameters(params);
   }
}

int SelectorAnalog::getMaxMembers() const {
   return mNumAnalogs;
}

void SelectorAnalog::updateParameters(const std::vector<int>& iDates,
      int iInit,
      const std::vector<float>& iOffsets,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   assert(0);
   /*
   if(mComputeVariableVariances) {
      // TODO: Either data must downscale the location, or it must be done here
      assert(0);

      int V = (int) mVariables.size();
      // Update variable variances
      for(int v = 0; v < V; v++) {
         Ensemble ensemble;
         mData.getEnsemble(iDate, iInit, iOffset, iLocation, mVariables[v], Input::typeForecast, ensemble);

         Parameters parAverager;
         float ensMean  = mAverager->average(ensemble, parAverager);
         if(Global::isValid(ensMean)) {
            float newMean  = ensMean;
            float newMean2 = ensMean*ensMean;
            iParameters[0 + 2*v] = combine(iParameters[0 + 2*v],  newMean);
            iParameters[1 + 2*v] = combine(iParameters[1 + 2*v], newMean2);
         }
      }
   }
   */
}

const std::vector<float>& SelectorAnalog::getData(int iDate, int iInit, int iOffsetId, const Location& iLocation) const {

   // TODO: Speed up by using one large vector instead of map
   //double s = Global::clock();
   Key::Three<int,int,int> key(iDate, iOffsetId, iLocation.getId());
   if(!mCache.isCached(key)) {
      //std::cout << "Cache miss " << iDate << " " << iOffsetId << " " << iLocation.getId() << std::endl;
      std::vector<float> currentValues;
      currentValues.resize(mVariables.size());
      float offset = mAllOffsets[iOffsetId]; 
      for(int v = 0; v < (int) mVariables.size(); v++) {
         std::string var = mVariables[v];
         Ensemble ensemble;
         if(mDataset == "")
            mData.getEnsemble(iDate, iInit, offset, iLocation, var, Input::typeForecast, ensemble);
         else
            mData.getEnsemble(iDate, iInit, offset, iLocation, mDataset, var, ensemble);

         // TODO: We don't really want these parameters to ever change from one call to
         // select to another
         Parameters parAverager;
         float value = mAverager->average(ensemble, parAverager);
         int index = v;
         currentValues[index] = value;
      }
      mCache.add(key, currentValues);
   }
   //double e = Global::clock();
   //std::cout << "getData: " << e - s << std::endl;
   return mCache.get(key);
}
bool SelectorAnalog::needsTraining() const {
    return mComputeVariableVariances;
 }
