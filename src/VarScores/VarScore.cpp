#include "VarScore.h"
#include "SchemesHeader.inc"

VarScore::VarScore(const Options& iOptions) : Component(iOptions),
      mNumBins(10) {
   iOptions.getValue("numBins", mNumBins);

}
#include "Schemes.inc"
std::vector<int> VarScore::findSimilarIndices(const std::vector<float>& iObs, int iIndex) const {
   float obs = iObs[iIndex];
   std::vector<int> similarIndices;
   float min = getMin(iObs);
   float max = getMax(iObs);
   float threshold = (max - min)/20;
   for(int i = 0; i < iObs.size(); i++) {
      if(i != iIndex) {
         if(fabs(iObs[i]- obs) < threshold) {
            similarIndices.push_back(i);
            // std::cout << i << " ";
         }
      }
   }
   // std::cout << std::endl;
   return similarIndices;
}
std::vector<float> VarScore::getEdges(const std::vector<float>& iObs) const {
   std::vector<float> edges(mNumBins+1, 0);
   float min = getMin(iObs);
   float max = getMax(iObs);
   if(Global::isValid(min) && Global::isValid(max)) {
      edges[0] = min;
      edges[mNumBins] = max;
      for(int i = 1; i < mNumBins; i++) {
         edges[i] = min + i*(max - min)/mNumBins;
      }
   }

   // for(int i = 0; i < edges.size(); i++) {
   //    std::cout << edges[i] << " ";
   // }
   // std::cout << std::endl;
   return edges;
}

float VarScore::getMin(const std::vector<float>& iArray) const {
   float min = Global::MV;
   for(int i = 0; i < iArray.size(); i++) {
      float value = iArray[i];
      if(!Global::isValid(min) || value < min)
         min = value;
   }
   return min;
}
float VarScore::getMax(const std::vector<float>& iArray) const {
   float max = Global::MV;
   for(int i = 0; i < iArray.size(); i++) {
      float value = iArray[i];
      if(!Global::isValid(max) || value > max)
         max = value;
   }
   return max;
}
