#include "Performance.h"
#include "../Scheme.h"
#include "../Data.h"
#include "../DetMetrics/DetMetric.h"
#include "../Metrics/Metric.h"
#include "../Member.h"
#include "../Slice.h"

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
      std::vector<Slice>& iSlices) const {

   std::vector<Member> members;
   mData.getInput()->getMembers(members);

   std::vector<std::pair<int, float> > metrics;
   for(int i = 0; i < (int) members.size(); i++) {
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

   int useNum = members.size();
   if(mNum != Global::MV)
      useNum = mNum;

   for(int i = 0; i < useNum; i++) {
      Member member(mData.getInput()->getName(), metrics[i].first);
      float skill = metrics[i].second;
      Slice slice(iDate, iInit, iOffset, member, skill);
      iSlices.push_back(slice);
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
      const Location& iLocation,
      const std::string& iVariable,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   // TODO
   Obs obs = iObs[0];

   if(obs.getValue() != Global::MV) {
      std::vector<Member> members;
      mData.getInput()->getMembers(members);
      if(iParameters.getIsDefault()) {
         for(int i = 0; i < (int) members.size(); i++) {
            iParameters[i] = 100;
         }
      }
      assert(iParameters.size() == members.size());
      std::vector<Location> locations;
      Location location;
      mData.getInput()->getSurroundingLocations(iLocation, locations, 1);
      location = locations[0];
      for(int i = 0; i < (int) members.size(); i++) {
         float fcst = mData.getInput()->getValue(iDate, iInit, iOffset, location.getId(), i, obs.getVariable());
         if(fcst != Global::MV) {
            float currPerformance = mMetric->compute(obs.getValue(), fcst, Parameters(), mData, iVariable);
            iParameters[i] = combine(iParameters[i], currPerformance);
         }
      }
      iParameters.setIsDefault(false);
   }
}
