#ifndef COMPONENT_H
#define COMPONENT_H
#include "Global.h"

class Options;
class Parameters;
class Data;

//! Abstract base class representing a component in the modeling system
class Component {
   public:
      Component(const Options& iOptions, const Data& iData);
      virtual ~Component();
      enum Type {TypeInput,
         TypeDownscaler,
         TypeSelector,
         TypeCorrector,
         TypeUncertainty,
         TypeContinuous,
         TypeDiscrete,
         TypeDiscreteLower,
         TypeDiscreteUpper,
         TypeCalibrator,
         TypeUpdater,
         TypeAverager,
         TypeSmoother,
         TypeDetMetric,
         TypeEstimator,
         TypeMetric};
      const static int numComponents = 8;
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

      //! Translates enumerated type to string
      static std::string getComponentName(Component::Type iType);

      std::string getSchemeName() const;
      std::string getTag() const;
   protected:
      //! Combine and old and a new parameter by exponential smoothing
      //! iNumNew is a weight multiplier for the new parameters
      float combine(float iOldParameter, float iNewParameter, float iNumNew=1) const;

      //! Call this in a schemes constructor if the scheme is under development to prevent its use
      void  underDevelopment() const;

      //! Notify that the scheme iClassName does not exist
      static void doesNotExist(const std::string& iClassName);

      // Global scheme options
      float mEfold;
      const Data& mData;
      std::string mTag;
   private:
      //! Default e-folding time used when a scheme does not specify an e-folding time
      const static float mDefaultEfold = 30;
};
#endif

