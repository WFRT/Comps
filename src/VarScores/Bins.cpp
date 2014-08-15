#include "Bins.h"

VarScoreBins::VarScoreBins(const Options& iOptions) : VarScore(iOptions) {
}

float VarScoreBins::score(const std::vector<float>& iObs, const std::vector<float>& iForecasts) const {
   int T = iObs.size();
   std::vector<float> edges = getEdges(iForecasts);
   float totalFrac = 0;
   for(int t = 0; t < T; t++) {
      float obs = iObs[t];
      float fcst = iForecasts[t];

      // Find similar obs
      std::vector<int> similarFcstIndices = findSimilarIndices(iForecasts, t);

      // Find how many bins the forecasts hits
      std::vector<int> hits(edges.size()-1, 0);
      for(int i = 0; i < edges.size()-1; i++) {
         for(int j = 0; j < similarFcstIndices.size(); j++) {
            float fcst = iForecasts[similarFcstIndices[j]];
            if(Global::isValid(fcst) && fcst >= edges[i] && fcst < edges[i+1]) {
               hits[i]++;
            }
         }
      }

      // What fraction of bins have hits?
      int totalHitBins = 0;
      for(int i = 0; i < hits.size(); i++) {
         if(hits[i] > 0)
            totalHitBins++;
      }
      float frac = (float) totalHitBins / hits.size();
      totalFrac += frac;
   }
   float averageFrac = totalFrac / T;
   return averageFrac;
}
