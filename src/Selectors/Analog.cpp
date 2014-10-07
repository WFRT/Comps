#include "Selector.h"
#include "Analog.h"
#include "../Variables/Variable.h"
#include "../Parameters.h"
#include "../DetMetrics/DetMetric.h"
#include "../DetMetrics/Norm.h"
#include "../Ensemble.h"
#include "../Scheme.h"
#include "../Data.h"
#include "../Member.h"
#include "../Field.h"
#include "../Obs.h"

SelectorAnalog::SelectorAnalog(const Options& iOptions, const Data& iData) :
      Selector(iOptions, iData),
      mDayWidth(365),
      mLocationIndependent(false),
      mAdjustOffset(0),
      mOffsetIndependent(false),
      mDoObsForward(false),
      mObsInput(NULL),
      mAllowFutureValues(false),
      mPrintDates(false),
      mComputeVariableVariances(false) {

   //! Which variables should be used to search for analogs? If none specified, use the
   //! variable being predicted.
   iOptions.getValues("variables", mVariables);
   //! Number of analogs to include in the ensemble
   iOptions.getRequiredValue("numAnalogs", mNumAnalogs);
   //! Allow analogs to come from the future
   iOptions.getValue("allowFutureValues", mAllowFutureValues);
   //! What weight should be given to each variable? If not specified, variables are
   //! weighted by their standard deviation of their climatology
   if(iOptions.getValues("weights", mWeights)) {
      int numWeights = mWeights.size();
      int numVariables = mVariables.size();
      if(numWeights != numVariables) {
         std::stringstream ss;
         ss << "SelectorAnalog: The number of weights (" << numWeights
            << ") must equal the number of variables (" << numVariables << ")";
         Global::logger->write(ss.str(), Logger::error);
      }
   }
   // Don't need weights when using predicand as analog variable
   else if(mVariables.size() == 0) {
      mWeights.push_back(1);
   }
   // Use the variable's climatological variance as weights (if not weights are not specified)
   else {
      for(int i = 0; i < (int) mVariables.size(); i++) {
         const Variable* var = mData.getVariable(mVariables[i]);
         if(!Global::isValid(var->getStd())) {
            std::stringstream ss;
            ss << "SelectorAnalog: Variable " << var->getName() << " does not have a std specified. ";
            ss << "Therefore the skills of the analog variables cannot be normalized.";
            Global::logger->write(ss.str(), Logger::error);
         }
         mWeights.push_back(1/var->getStd());
      }
   }

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

   //! Print the selected dates (for debugging)
   iOptions.getValue("printDates", mPrintDates);

   //! Use this (forecast) dataset to match analogs. Otherwise, the default datasets is used
   iOptions.getValue("dataset", mDataset);

   std::string metric;
   //! Which DetMetric scheme should be used to evaluate analog similarity?
   //! If unspecified, use absolute difference metric
   if(iOptions.getValue("analogMetric", metric))
      mMetric = DetMetric::getScheme(metric);
   else {
      mMetric = new DetMetricNorm(Options("order=1"));
   }

   iOptions.getValue("adjustOffset", mAdjustOffset);

      // Check that the input has these variables
   /*
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
   */

   // Which dataset to use as obs
   mObsInput = mData.getObsInput();
   if(mDataset == "" && mObsInput == NULL) {
      std::stringstream ss;
      ss << "SelectorAnalog: No observation dataset specified. Cannot produce forecasts.";
      Global::logger->write(ss.str(), Logger::critical);
   }

   mCache.setName("SelectorAnalog");
   iOptions.check();
}
SelectorAnalog::~SelectorAnalog() {
   delete mMetric;
}
void SelectorAnalog::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

   if(mObsInput == NULL) {
      return;
   }
   std::string obsInputName = mObsInput->getName();

   std::vector<float> allOffsets = mData.getInput()->getOffsets();
   std::vector<std::string> variables;
   if(mVariables.size() == 0)
      variables = std::vector<std::string>(1,iVariable);
   else
      variables = mVariables;

   // Weights
   std::vector<float> weights(variables.size());
   if(mComputeVariableVariances) {
      std::vector<float> values = iParameters.getAllParameters();
      assert(values.size() == variables.size()*2);
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
   assert(weights.size() == variables.size());

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
         for(int i = 0; i < (int) allOffsets.size(); i++) {
            if(allOffsets[i] < 24) {
               useOffsets.push_back(i);
            }
         }
      }
      else {
         useOffsets.resize(allOffsets.size());
         for(int i = 0; i < (int) allOffsets.size(); i++) {
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
      float targetOffset = allOffsets[oI]; 
      for(int v = 0; v < (int) variables.size(); v++) {
         std::vector<float> ensValues;
         Ensemble ensemble;
         if(mDataset == "") {
            // const Variable* var = Variable::get(variables[v]);
            ensemble = mData.getEnsemble(targetDate, targetInit, targetOffset, iLocation, variables[v]);
            // ensemble = mData.getEnsemble(targetDate, targetInit, targetOffset, iLocation, var);
         }
         else
            ensemble = mData.getEnsemble(targetDate, targetInit, targetOffset, iLocation, variables[v], mDataset);
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
      ss << "No forecast data available for " << iDate << " " << iOffset << " " << iLocation.getId() << ": No analogs found";
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

      if((mAllowFutureValues || currDate < iDate) && validDayDiff) {
         // Only keep availOffsets and availVariables
         for(int k = 0; k < (int) availOffsets.size(); k++) {
            int oI = availOffsets[k];
            int v  = availVariables[k];
            float offset = allOffsets[oI];
            const std::vector<float>& values = getData(currDate, iInit, offset, iLocation, variables);

            int currDateWithOffset = Global::getDate(currDate, iInit, allOffsets[oI]);
            // Invalid date if in the future
            if(currDateWithOffset > iDate || (currDateWithOffset == iDate && allOffsets[oI] > 0)) {
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

   if(mPrintDates) {
      Obs obs;
      mData.getObs(iDate, iInit, iOffset, iLocation, iVariable, obs);
      std::cout << "Date: " << iDate << " offset: " << iOffset << " location: " << iLocation.getId() << " obs: " << obs.getValue() << std::endl;
      std::cout << "Available variables:";
      for(int i = 0; i < availVariables.size(); i++) {
         std::cout << " " << variables[availVariables[i]];
      }
      std::cout << " = (";
      for(int i = 0; i < targetValues.size(); i++) {
         std::cout << targetValues[i]/mWeights[availVariables[i]] << " ";
      }
      std::cout << ")";
      std::cout << " Variable weights: (";
      for(int i = 0; i < availVariables.size(); i++) {
         std::cout << " " << mWeights[availVariables[i]];
      }
      std::cout << ")" << std::endl;

   }
   while(iFields.size() < mNumAnalogs && i < (int) metrics.size()) {
      int date = metrics[i].first;
      float skill = metrics[i].second;
      if(skill != Global::MV) {
         //std::cout << "Best date:  " << date << " -- " << metrics[i].second << std::endl;
         // TODO
         float analogInit = iInit;
         float analogOffset;
         Member member(obsInputName);
         int analogDate;
         if(mDoObsForward) {
            analogOffset = fmod(iOffset, 24);
            analogDate = Global::getDate(date, 24);
         }
         else {
            analogOffset = iOffset;
            analogDate = date;
         }
         if(mPrintDates) {
            Ensemble ensemble;
            if(mDataset == "")
               ensemble = mData.getEnsemble(analogDate, analogInit, analogOffset, iLocation, iVariable);
            else
               ensemble = mData.getEnsemble(analogDate, analogInit, analogOffset, iLocation, iVariable, mDataset);
            float obs = ensemble.getMoment(1);
            std::cout << analogDate << " " << obs << "(";
            const std::vector<float>& values = getData(analogDate, analogInit, analogOffset, iLocation, variables);
            for(int i = 0; i < availVariables.size(); i++) {
               std::cout << values[availVariables[i]] << " ";
            }
            std::cout << ")" << std::endl;
         }
         Field slice(analogDate, analogInit, analogOffset, member, skill);
         iFields.push_back(slice);
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
         const Variable* var = mData.getVariable(mVariables[v]);
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

int SelectorAnalog::numParametersCore() const {
   if(mComputeVariableVariances)
      return 2 * mVariables.size();
   else
      return 0;
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
         Ensemble ensemble = mData.getEnsemble(iDate, iInit, iOffset, iLocation, mVariables[v]);

         Parameters parAverager;
         float ensMean  = ensemble.getMoment(1);
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

const std::vector<float>& SelectorAnalog::getData(int iDate, int iInit, float iOffset, const Location& iLocation, const std::vector<std::string>& iVariables) const {
   // TODO: Speed up by using one large vector instead of map
   //double s = Global::clock();
   Key::Three<int,float,int> key(iDate, iOffset, iLocation.getId());
   if(!mCache.isCached(key)) {
      //std::cout << "Cache miss " << iDate << " " << iOffset << " " << iLocation.getId() << std::endl;
      std::vector<float> currentValues;
      currentValues.resize(iVariables.size());
      for(int v = 0; v < (int) iVariables.size(); v++) {
         std::string var = iVariables[v];
         Ensemble ensemble;
         if(mDataset == "")
            ensemble = mData.getEnsemble(iDate, iInit, iOffset, iLocation, var);
         else
            ensemble = mData.getEnsemble(iDate, iInit, iOffset, iLocation, var, mDataset);

         float value = ensemble.getMoment(1);
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
