#include "SelectorSkill.h"
#include <numeric>
#include <functional>
MetricSelectorSkill::MetricSelectorSkill(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   Component::underDevelopment();
   std::string type;
   if(!iOptions.getValue("type", type)) {
      type = "mean";
   }
   if(type == "min") {
      mType = typeMin;
   }
   else if(type == "mean") {
      mType = typeMean;
   }
   else if(type == "max") {
      mType = typeMax;
   }
}
float MetricSelectorSkill::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   /*
   Location    location = iObs.getLocation();
   float       offset   = iObs.getOffset();
   std::string variable = iObs.getVariable();
   std::vector<Field> slices;
   iConfiguration.getSelectorIndicies(iDate, iInit, iOffset, location, variable, slices);
   std::vector<float> skills;
   if(slices.size() == 0) {
      return Global::MV;
   }
   for(int i = 0; i < (int) slices.size(); i++) {
      skills.push_back(slices[i].getSkill());
   }
   float returnValue;
   if(mType == typeMin) {
      returnValue = *(std::min_element(skills.begin(), skills.end()));
   }
   else if(mType == typeMean) {
      // TODO
      assert(0);
      returnValue = (float) (std::accumulate(skills.begin(), skills.end(), 0));///skills.size();
   }
   else if(mType == typeMax) {
      returnValue = *(std::max_element(skills.begin(), skills.end()));
   }
   return returnValue;
   */  
   return 0;
}
