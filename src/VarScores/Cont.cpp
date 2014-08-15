#include "Cont.h"

VarScoreCont::VarScoreCont(const Options& iOptions) : VarScore(iOptions) {
}

float VarScoreCont::score(const std::vector<float>& iObs, const std::vector<float>& iForecasts) const {
   int T = iObs.size();
   std::vector<float> edges = getEdges(iForecasts);
   float totalFrac = 0;
   float obsDiff = 1;
   float fcstDiff = 1;

   int hit=0, miss=0, fa=0, cr=0;
   for(int t = 0; t < T; t++) {
      float obs0  = iObs[t];
      float fcst0 = iForecasts[t];

      if(Global::isValid(obs0) && Global::isValid(fcst0)) {
         for(int i = 0; i < T; i++) {
            float obs  = iObs[t];
            float fcst = iForecasts[t];
            if(i != T && Global::isValid(obs) && Global::isValid(fcst)) {
               bool o = fabs(obs - iObs[i]) < obsDiff ;
               bool f = fabs(fcst - iForecasts[i]) < fcstDiff ;
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
   //std::cout << hit << " " << miss << " " << fa << " " << cr << " " << (float) (hit+miss) / total << " --> " << (float) (hit) / (hit+fa) << std::endl;
   return before-after;
}
