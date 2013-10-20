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
   mMetric = DetMetric::getScheme(optMetric, iData);

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
   iParameters[0] = 0;
   iParameters.setIsDefault(true);
}
int SelectorPerformance::getMaxMembers() const {
   return mNum;
}
void SelectorPerformance::updateParameters(int iDate,
      int iInit,
      float iOffset,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   // TODO
   Obs obs = iObs[0];
   std::string variable = obs.getVariable();
   Location location = obs.getLocation();

   if(obs.getValue() != Global::MV) {
      Ensemble ens;
      mData.getEnsemble(iDate, iInit, iOffset, location, variable, Input::typeForecast, ens);
      if(iParameters.getIsDefault()) {
         for(int i = 0; i < ens.size(); i++) {
            iParameters[i] = 100;
         }
      }
      assert(iParameters.size() == ens.size());
      for(int i = 0; i < (int) ens.size(); i++) {
         float fcst = ens[i];
         if(fcst != Global::MV) {
            float currPerformance = mMetric->compute(obs.getValue(), fcst, Parameters(), mData, variable);
            iParameters[i] = combine(iParameters[i], currPerformance);
         }
      }
      iParameters.setIsDefault(false);
   }
}
