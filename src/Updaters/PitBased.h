#ifndef UPDATERPITBASED_H
#define UPDATERPITBASED_H
#include "Updater.h"

class UpdaterPitBased : public Updater {
   public:
      UpdaterPitBased(const Options& iOptions, const Data& iData);
      void getDefaultParameters(Parameters& iParameters) const;
      float update(float iCdf, const Obs& iRecentObs, const Distribution::ptr iRecent, const Distribution::ptr iDist, const Parameters& rParameters) const;
      void updateParameters(
            const std::vector<Distribution::ptr>& iDists,
            const std::vector<Obs>& iObs,
            const std::vector<Distribution::ptr>& iRecentDists,
            const std::vector<Obs>& iRecentObs,
            Parameters& iParameters) const;
      float evidence(float iCdf, int n, float iPit, const Parameters& iParameters, const Obs& iObs) const;
   private:
      float getSigma(float iSigma0) const;
      float mSigma0;
      const static int mNumIterations = 4;
};
#endif
