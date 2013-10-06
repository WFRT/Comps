#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H
#include "Global.h"
#include "Ensemble.h"
#include "Parameters.h"
#include "Entity.h"
#include <boost/shared_ptr.hpp>
class Calibrator;
class Uncertainty;

class Distribution : public Entity {
   public:
      typedef boost::shared_ptr<Distribution> ptr;
      Distribution();
      // Use functions specified by Uncertainty scheme
      virtual float getCdf(float iX) const = 0;
      virtual float getPdf(float iX) const = 0;
      virtual float getInv(float iCdf) const = 0;
      //! Compute moment numerically if necessary
      virtual float getMoment(int iMoment) const = 0;
      virtual int   getDate() const = 0;
      virtual int   getInit() const = 0;
      virtual float getOffset() const = 0;
      virtual Location getLocation() const = 0;
      virtual std::string getVariable() const = 0;
      float getP0() const;
      float getP1() const;
   protected:
};
class DistributionUncertainty : public Distribution {
   public:
      DistributionUncertainty(const Uncertainty& iUncertainty, Ensemble iEnsemble, Parameters iParameters);
      // Use functions specified by Uncertainty scheme
      float getCdf(float iX) const;
      float getPdf(float iX) const;
      float getInv(float iCdf) const;
      float getMoment(int iMoment) const;
      std::string getVariable() const;
      Location getLocation() const;
      int   getDate() const;
      int   getInit() const;
      float getOffset() const;
   protected:
      const Uncertainty& mUncertainty;
      const Ensemble mEnsemble;
      const Parameters mParameters;
};
class DistributionCalibrator : public Distribution {
   public:
      DistributionCalibrator(const Distribution::ptr iUpstream, const Calibrator& iCalibrator, Parameters iParameters);
      // Use calibration function to modify upstream distribution
      float getCdf(float iX) const;
      float getPdf(float iX) const;
      float getInv(float iCdf) const;
      //! Compute moment numerically if necessary
      float getMoment(int iMoment) const;
      std::string getVariable() const;
      Location getLocation() const;
      int   getDate() const;
      int   getInit() const;
      float getOffset() const;
   private:
      const Distribution::ptr mUpstream;
      const Calibrator&       mCalibrator;
      const Parameters        mParameters;
};
#endif
