#ifndef COMPONENT_H
#define COMPONENT_H
#include "Global.h"

class Options;

//! Abstract base class representing a component in the modeling system
class Component {
   public:
      Component(const Options& iOptions);
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
         TypeMetric,
         TypeUnspecified};
      const static int numComponents = 8;

      //! Translates enumerated type to string
      static std::string getComponentName(Component::Type iType);

      std::string getSchemeName() const;
      std::string getTag() const;

      //! Notify that the scheme iClassName does not exist
      static void doesNotExist(const std::string& iClassName);

   protected:

      //! Call this in a schemes constructor if the scheme is under development to prevent its use
      void  underDevelopment() const;

      // Global scheme options
      std::string mTag;
};
#endif

