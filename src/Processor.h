#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "Global.h"
#include "Component.h"

class Options;
class Parameters;
class Data;

//! Abstract base class representing a processing component in the modeling system.
//! Processing components have parameters that can be updated based on observations.
class Processor : public Component {
   public:
      Processor(const Options& iOptions, const Data& iData);
      virtual ~Processor();
      //! Specifies parameters used when initializing scheme
      //! @param iParameters Writes parameters into this object
      virtual void getDefaultParameters(Parameters& iParameters) const {};

      //! Does this scheme need to be trained?
      virtual bool needsTraining() const {return true;};

      //! Does this scheme need to be run separately for each location?
      virtual bool isLocationDependent() const {return true;};

      //! Does this scheme need to be run separately for each member?
      virtual bool isMemberDependent() const {return true;};

      //! Does this scheme need to be run separately for each offset?
      virtual bool isOffsetDependent() const {return true;};

      //! Does this scheme need to be run separately for each variable?
      virtual bool isVariableDependent() const {return true;};

      //! Can this component be chained?
      virtual bool isChainable() const {return false;};

   protected:
      //! Combine and old and a new parameter by exponential smoothing
      //! iNumNew is a weight multiplier for the new parameters
      float combine(float iOldParameter, float iNewParameter, float iNumNew=1) const;

      // Global scheme options
      float mEfold;
      const Data& mData;
   private:
      //! Default e-folding time used when a scheme does not specify an e-folding time
      const static float mDefaultEfold = 30;
};
#endif

