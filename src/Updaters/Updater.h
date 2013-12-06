#ifndef UPDATER_H
#define UPDATER_H
#include "../Global.h"
#include "../Component.h"
#include "../Distribution.h"

class Data;
class Options;
class Obs;
class Parameters;

class Updater : public Component {
   public:
      Updater(const Options& iOptions, const Data& iData);
      ~Updater();
      Distribution::ptr getDistribution(const Distribution::ptr iUpstream, const Obs& iRecentObs, const Distribution::ptr iRecent, const Parameters& iParameters) const;
      virtual float update(float iCdf, const Obs& iRecentObs, const Distribution::ptr iRecent, const Distribution::ptr iDist, const Parameters& iParameters) const = 0;
      virtual float unUpdate(float iCdf, const Obs& iRecentObs, const Distribution::ptr iRecent, const Distribution::ptr iDist, const Parameters& iParameters) const;
      virtual float amplify(float iCdf, const Obs& iRecentObs, const Distribution::ptr iRecent, const Distribution::ptr iDist, const Parameters& iParameters) const;
      virtual void updateParameters(
            const std::vector<Distribution::ptr>& iDists,
            const std::vector<Obs>& iObs,
            const std::vector<Distribution::ptr>& iRecentDists,
            const std::vector<Obs>& iRecentObs,
            Parameters& iParameters) const;
      static Updater* getScheme(const Options& rOptions, const Data& iData);
      static Updater* getScheme(const std::string& iTag, const Data& iData);
   private:
};
#endif
