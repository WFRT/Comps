#include "Performance.h"
#include "../Scheme.h"
#include "../Data.h"
#include "../DetMetrics/DetMetric.h"
#include "../Metrics/Metric.h"
#include "../Member.h"
#include "../Field.h"

SelectorPerformance::SelectorPerformance(const Options& iOptions, const Data& iData) : Selector(iOptions, iData) {
   //Component::underDevelopment();
   if(!iOptions.getValue("num", mNum)) {
      mNum = Global::MV;
   }
   std::string metric;
   iOptions.getRequiredValue("detMetric", metric);

   Options optMetric;
   Scheme::getOptions(metric, optMetric);
   mMetric = DetMetric::getScheme(optMetric);

   assert(mNum == Global::MV || mNum > 0);
}

SelectorPerformance::~SelectorPerformance() {
   delete mMetric;
}

void SelectorPerformance::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

   int numMembers = mData.getInput()->getNumMembers();

   std::vector<std::pair<int, float> > metrics;
   for(int i = 0; i < (int) numMembers; i++) {
      float skill;
      if(iParameters.getIsDefault()) {
         skill = 1;
      }
      else {
         skill = iParameters[i];
      }
      std::pair<int,float> p(i, skill);
      metrics.push_back(p);
   }

   std::sort(metrics.begin(), metrics.end(), Global::sort_pair_second<int, float>());

   std::string datasetName = mData.getInput()->getName();

   int useNum = numMembers;
   if(mNum != Global::MV)
      useNum = mNum;

   for(int i = 0; i < useNum; i++) {
      Member member(mData.getInput()->getName(), metrics[i].first);
      float skill = metrics[i].second;
      Field slice(iDate, iInit, iOffset, member, skill);
      iFields.push_back(slice);
   }
}

void SelectorPerformance::getDefaultParameters(Parameters& iParameters) const {
   // We do not know how many parameters to use, since it depends on the ensemble size
   iParameters[0] = 0;
   iParameters.setIsDefault(true);
}
int SelectorPerformance::getMaxMembers() const {
   return mNum;
}
void SelectorPerformance::updateParameters(const std::vector<int>& iDates,
      int iInit,
      const std::vector<float>& iOffsets,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   int N = iDates.size();
   std::vector<float> currPerformance;
   std::vector<int> counter;
   for(int n = 0; n < N; n++) {
      Obs obs = iObs[n];
      int date = iDates[n];
      float offset = iOffsets[n];
      std::string variable = obs.getVariable();
      Location location = obs.getLocation();

      if(obs.getValue() != Global::MV) {
         Ensemble ens = mData.getEnsemble(date, iInit, offset, location, variable);
         if(iParameters.getIsDefault()) {
            // Initialize parameters for the first time
            for(int i = 0; i < ens.size(); i++) {
               iParameters[i] = 100;
            }
         }
         assert(iParameters.size() == ens.size());
         if(currPerformance.size() < ens.size()) {
            currPerformance.resize(N, 0);
            counter.resize(N, 0);
         }

         for(int i = 0; i < (int) ens.size(); i++) {
            float fcst = ens[i];
            if(fcst != Global::MV) {
               assert(currPerformance.size() > n);
               currPerformance[n] += mMetric->compute(obs.getValue(), fcst, variable);
               counter[n]++;
            }
         }
      }
   }
   // Update
   for(int i = 0; i < (int) iParameters.size(); i++) {
      if(counter[i] > 0) {
         iParameters[i] = combine(iParameters[i], currPerformance[i]/counter[i]);
      }
   }
   iParameters.setIsDefault(false);
}
