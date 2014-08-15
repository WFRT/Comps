#ifndef VAR_SCORE_H
#define VAR_SCORE_H
#include "../Processor.h"
#include "../Options.h"

class VarScore : public Component {
   public:
      VarScore(const Options& iOptions);
      // Low values should be better
      virtual float score(const std::vector<float>& iObs, const std::vector<float>& iForecasts) const = 0;
      static VarScore* getScheme(const Options& iOptions);
      static VarScore* getScheme(const std::string& iTag);
   protected:
      std::vector<int> findSimilarIndices(const std::vector<float>& iObs, int iIndex) const;
      std::vector<float> getEdges(const std::vector<float>& iObs) const;
      float getMin(const std::vector<float>& iArray) const;
      float getMax(const std::vector<float>& iArray) const;
   private:
      int mNumBins;
};
#endif
