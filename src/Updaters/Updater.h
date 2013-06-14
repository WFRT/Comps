#ifndef UPDATER_H
#define UPDATER_H
#include "../Global.h"
#include "../Component.h"

class Distribution;
class Data;
class Options;
class Obs;
class Parameters;

class Updater : public Component {
   public:
      Updater(const Options& iOptions, const Data& iData);
      ~Updater();
      //void getDistribution(const Distribution::ptr iUpstream, const Parameters& iParameters, Distribution& iDistribution) const;
      virtual float update (float rCdf, int n, float rPit, const Parameters& rParameters) const = 0;
      //virtual float unUpdate (float rCdf, int n, float rPit, const Parameters& rParameters) const;
      virtual float amplify(float rCdf, int n, float rPit, const Parameters& rParameters) const;
      virtual void updateParameters(const Obs& iObs, float iCdf, int n, float iPit, Parameters& iParameters) const;
      static Updater* getScheme(const Options& rOptions, const Data& iData);
      static Updater* getScheme(const std::string& iTag, const Data& iData);
   private:
};
#endif
