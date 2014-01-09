#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "../Global.h"
#include "../Component.h"
#include "../Options.h"
#include "../Ensemble.h"
class Data;
//! Applies a transformation function to a scalar value.
class Transform : public Component {
   public:
      static Transform* getScheme(const Options& iOptions, const Data& iData);
      static Transform* getScheme(const std::string& iTag, const Data& iData);

      //! Apply transform function iValue
      float transform(float iValue) const;
      //! Apply transform function to each ensemble member
      void transform(Ensemble& iEnsemble) const;
      //! Apply the inverse transform
      float inverse(float iValue) const;
      void inverse(Ensemble& iEnsemble) const;
      //! Get the derivative of the transform at iValue
      float derivative(float iValue) const;
      void  derivative(Ensemble& iEnsemble) const;

      bool   isMandatory() const {return false;};
      bool   needsTraining() const {return false;};
   protected:
      Transform(const Options& iOptions, const Data& iData);
      virtual float transformCore(float iValue) const = 0;
      //! Defaults to giving an error message
      virtual float inverseCore(float iValue) const;
      //! Defaults to giving an error message
      virtual float derivativeCore(float iValue) const;
};
#endif
