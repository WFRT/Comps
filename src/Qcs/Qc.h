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
      // Constructors
      static Qc* getScheme(const Options& iOptions, const Data& iData);
      static Qc* getScheme(const std::string& iTag, const Data& iData);

      //! Return true if value passed quality control
      bool check(const Value& iValue) const;

      //! Return true if ensemble quality control
      bool qc(Ensemble& iEnsemble) const;
   protected:
      Qc(const Options& iOptions, const Data& iData);
      virtual bool checkCore(const Value& iValue) const = 0;
      const Data& mData;
};
#endif
