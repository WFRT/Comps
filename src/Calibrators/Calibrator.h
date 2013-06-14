#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#include "../Component.h"
#include "../Distribution.h"

class Options;
class Parameters;
class Obs;

class Calibrator : public Component {
   public:
      Calibrator(const Options& iOptions, const Data& iData);
      Distribution::ptr getDistribution(const Distribution::ptr iUpstream, Parameters iParameters) const;

      virtual float calibrate(float iCdf, const Distribution::ptr iDist,  const Parameters& iParameters) const = 0;
      // Returns the amplification factor at a given CDF
      virtual float amplify(float iCdf, const Distribution::ptr iDist, const Parameters& iParameters) const;
      // Iterative approach to reversing the process of calibrating
      virtual float unCalibrate(float iCdf, const Distribution::ptr iDist, const Parameters& iParameters) const;
      static Calibrator* getScheme(const Options& iOptions, const Data& iData);
      static Calibrator* getScheme(const std::string& iTag, const Data& iData);
      virtual void  updateParameters(const Distribution::ptr iDist, const Obs& iObs, Parameters& iParameters) const {};
      bool   isMandatory() const {return false;};
   protected:
      static const float mMaxAmplify = 1e5;
      float mInvTol;
};
#endif

