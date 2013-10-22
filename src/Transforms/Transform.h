#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "../Global.h"
#include "../Component.h"
#include "../Options.h"
class Data;
//! Schemes representing a verification metric for a single forecast/observation pair
class Transform : public Component {
   public:
      //! Apply transform function iValue
      float transform(float iValue) const;
      //! Apply transform function to each ensemble member
      transform(Ensemble& iEnsemble) const;
      //! Apply the inverse transform to iValue
      float inverse(float iValue) const;
      //! Apply the inverse transform to each ensemble member
      void inverse(Ensemble& iEnsemble) const;
      static Transform* getScheme(const Options& iOptions, const Data& iData);
      static Transform* getScheme(const std::string& iTag, const Data& iData);
      bool   isMandatory() const {return false;};
      bool   needsTraining() const {return false;};
   protected:
      Transform(const Options& iOptions, const Data& iData);
      virtual float transformCore(float iValue) const = 0;
      //! Defaults to giving an error message
      virtual float inverseCore(float iValue) const;
};
#endif
