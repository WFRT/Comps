#include "Cont.h"

VarScoreCont::VarScoreCont(const Options& iOptions) : VarScore(iOptions) {
}

float VarScoreCont::score(const std::vector<float>& iObs, const std::vector<float>& iForecasts) const {
   int T = iObs.size();
   std::vector<float> edges = getEdges(iForecasts);
   float totalFrac = 0;
   float obsDiff = 1;
   float fcstDiff = 0.1;

   int hit=0, miss=0, fa=0, cr=0;
   std::vector<float> percentiles = getPercentiles(iForecasts);
   for(int t = 0; t < T; t++) {
      float obs0  = iObs[t];
      float fcst0 = iForecasts[t];
      float perc0 = percentiles[t];

      if(Global::isValid(obs0) && Global::isValid(fcst0)) {
         for(int i = 0; i < T; i++) {
            float obs  = iObs[i];
            float fcst = iForecasts[i];
            float perc = percentiles[i];
            if(i != T && Global::isValid(obs) && Global::isValid(fcst)) {
               bool o = fabs(obs0 - obs) < obsDiff ;
               // bool f = fabs(fcst0 - fcst) < fcstDiff ;
               bool f = fabs(perc - perc0) < fcstDiff ;
               if(o && f)
                  hit++;
               else if(o && !f)
                  miss++;
               else if(!o && f)
                  fa++;
               else
                  cr++;
            }
         }
      }
   }
   int total = hit + miss + fa + cr;
   int goodDays = hit + miss;
   int badDays  = fa + cr;
   float before = (float) (hit+miss) / total;
   float after  = (float) (hit) / (hit+fa);
   std::cout << hit << " " << miss << " " << fa << " " << cr << " " << (float) (hit+miss) / total << " --> " << (float) (hit) / (hit+fa) << std::endl;
   return before-after;
}

std::vector<float> VarScoreCont::getPercentiles(const std::vector<float>& iForecasts) const {
   std::vector<float> percentiles;
   std::vector<std::pair<float,int> > pairs;
   for(int i = 0; i < iForecasts.size(); i++) {
      pairs.push_back(std::pair<float, int>(iForecasts[i], i));
      percentiles.push_back(Global::MV);
   }

   std::sort(pairs.begin(), pairs.end(), Global::sort_pair_first<float, int>());
   int numValid = 0;
   for(int i = 0; i < pairs.size(); i++) {
      float fcst = pairs[i].first;
      if(Global::isValid(fcst)) {
         numValid++;
      }
   }
   if(numValid == 0)
      return percentiles;

   int counter = 0;
   for(int i = 0; i < pairs.size(); i++) {
      float fcst = pairs[i].first;
      int index = pairs[i].second;
      assert(index >= 0 && index < percentiles.size());
      if(Global::isValid(fcst)) {
         percentiles[index] = (float) counter / (numValid-1);
         counter++;
      }
   }
   return percentiles;
}
