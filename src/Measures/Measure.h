#ifndef MEASURE_H
#define MEASURE_H
#include "../Component.h"
#include "../Transforms/Transform.h"

class Data;
class Parameters;
class Ensemble;
class Obs;
class Options;

//! Computes some scalar measure over an ensemble of values
class Measure : public Component {
   public:
      Measure(const Options& iOptions, const Data& iData);
      ~Measure();
      //! Compute the measure
      float measure(const Ensemble& iEnsemble) const;

      static Measure* getScheme(const Options& iOptions, const Data& iData);
      static Measure* getScheme(const std::string& iTag, const Data& iData);
   protected:
      virtual float measureCore(const Ensemble& iEnsemble) const = 0;
   private:
      mutable float mLastMeasure;
      mutable std::vector<float> mLastEnsemble;
      std::vector<Transform*> mPostTransforms;
      std::vector<Transform*> mPreTransforms;
};
#endif
