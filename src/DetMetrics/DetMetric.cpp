#include "DetMetric.h"
#include "SchemesHeader.inc"
DetMetric::DetMetric(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {}

#include "Schemes.inc"

float DetMetric::compute(const std::vector<std::pair<std::string, float> > & iData0,
      const std::vector<std::pair<std::string, float> >& iData1,
      const Parameters& iParameters,
      const Data& iData) const {
   int N = (int) iData1.size();
   if(iData0.size() != iData1.size()) {
      std::stringstream ss;
      ss << "DetMetricNorm: sizes: " << iData0.size() << " " << iData1.size();
      Global::logger->write(ss.str(), Logger::error);
   }

   // Check if we need to adjust circular variables
   /*
   bool doCircular = false;
   for(int i = 0; i < N; i++) {
      if(iData0[i].second != Global::MV && iData1[i].second != Global::MV) {
         totalWeight++;
         const Variable& var = iData.getVariable(iData0[i].first);
         if(var.isCircular()) {
            doCircular = true;
            break;
         }
      }
   }
   if(doCircular) {

   }
   else {
      computeCore(iData0, iData1, iParameters, iData);
   }

   */
   return computeCore(iData0, iData1, iParameters, iData);
}

float DetMetric::compute(float iData0, float iData1, const Parameters& iParameters, const Data& iData, const std::string& iVariable) const {
   std::vector<float> iDataVec0;
   iDataVec0.push_back(iData0);
   std::vector<float> iDataVec1;
   iDataVec1.push_back(iData1);

   return compute(iDataVec0, iDataVec1, iParameters, iData, iVariable);
}

float DetMetric::compute(const std::vector<float>& iData0, const std::vector<float>& iData1, const Parameters& iParameters, const Data& iData, const std::string& iVariable) const {
   assert(iData0.size() == iData1.size());
   std::vector<std::pair<std::string, float> > iData0Pair;
   std::vector<std::pair<std::string, float> > iData1Pair;
   iData0Pair.resize(iData0.size());
   iData1Pair.resize(iData1.size());
   for(int i = 0; i < (int) iData0.size(); i++) {
      std::pair<std::string, float> p0(iVariable, iData0[i]);
      std::pair<std::string, float> p1(iVariable, iData1[i]);
      iData0Pair[i] = p0;
      iData1Pair[i] = p1;
   }
   return compute(iData0Pair, iData1Pair, iParameters, iData);
}
