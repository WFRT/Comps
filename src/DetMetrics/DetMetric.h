#ifndef DET_METRIC_H
#define DET_METRIC_H
#include "../Options.h"
#include "../Component.h"
#include "../Variables/Variable.h"

class DetMetric : public Component {
   public:
      DetMetric(const Options& iOptions);
      float compute(const std::vector<std::pair<std::string, float> > & iData0,
            const std::vector<std::pair<std::string, float> >& iData1) const;
      float compute(const std::vector<float>& iData0, const std::vector<float>& iData1, const std::string& iVariable) const;
      float compute(float iData0, float iData1, const std::string& iVariable) const;
      static DetMetric* getScheme(const Options& iOptions);
      static DetMetric* getScheme(const std::string& iTag);
      bool   isMandatory() const {return false;};
      //! Do higher scores mean better skill?
      virtual bool isPositiveOriented() const {return false;};
   protected:
      virtual float computeCore(const std::vector<std::pair<std::string, float> > & iData0,
            const std::vector<std::pair<std::string, float> >& iData1) const = 0;
};
#endif
