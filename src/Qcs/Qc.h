#ifndef QC_H
#define QC_H
#include "../Global.h"
#include "../Component.h"

class Data;
class Value;
class Options;
class Ensemble;

//! Quality control component that checks if values are reasonable
class Qc : public Component {
   public:
      Qc(const Options& iOptions, const Data& iData);
      bool check(const Value& iValue) const;
      bool qc(Ensemble& iEnsemble) const;
      static Qc* getScheme(const Options& rOptions, const Data& iData);
      static Qc* getScheme(const std::string& iTag, const Data& iData);
   protected:
      virtual bool checkCore(const Value& iValue) const;
};
#endif

