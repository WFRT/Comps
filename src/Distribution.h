#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H
#include "Global.h"
#include "Ensemble.h"
#include "Parameters.h"
#include "Entity.h"
#include "Obs.h"
#include "Averagers/Averager.h"
#include <boost/shared_ptr.hpp>
class Calibrator;
class Uncertainty;
class Updater;

class Distribution : public Entity {
   public:
      typedef boost::shared_ptr<Distribution> ptr;
      Distribution(Ensemble iEnsemble, const Averager& iAverager, Parameters iAveragerParameters);
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

      //! Returns the ensemble adjusted by the uncertainty
      Ensemble getEnsemble() const;
      //! Returns the deterministic forecast adjusted by the uncertainty
      float getDeterministic() const;

      //! Get the ensemble used to generate the distribution
      Ensemble getBaseEnsemble() const {return mEnsemble;};
      const Averager& getAverager() const {return mAverager;};
      Parameters getAveragerParameters() const {return mAveragerParameters;};
   protected:
      Ensemble mEnsemble;
      const Averager& mAverager;
      Parameters mAveragerParameters;
};
class DistributionUncertainty : public Distribution {
   public:
      DistributionUncertainty(const Uncertainty& iUncertainty, Parameters iParameters, Ensemble iEnsemble, const Averager& iAverager, Parameters iAveragerParameters);
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
class DistributionUpdater : public Distribution {
   public:
      DistributionUpdater(const Distribution::ptr iUpstream, const Updater& iUpdater, Obs iRecentObs, const Distribution::ptr iRecent, Parameters iParameters);
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
      const Updater&       mUpdater;
      Obs mRecentObs;
      const Distribution::ptr mRecent;
      const Parameters        mParameters;
};
#endif
