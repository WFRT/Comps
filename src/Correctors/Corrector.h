#ifndef CORRECTOR_H
#define CORRECTOR_H
#include "../Component.h"
#include "../Ensemble.h"

class Averager;
class Data;
class Location;
class Parameters;
class Obs;

//! Represents schemes that bias-correct an ensemble. 
class Corrector : public Component {
   public:
      Corrector(const Options& iOptions, const Data& iData);

      void correct(const Parameters& iParameters, Ensemble& iEnsemble) const;
      void updateParameters(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const;

      static Corrector* getScheme(const Options& iOptions, const Data& iData);
      static Corrector* getScheme(const std::string& iTag, const Data& iData);
      virtual bool isMemberSpecific() const;
      //! Does this method need the input ensemble to be the same size everytime?
      //! E.g. for parameter estimation
      bool needsConstantEnsembleSize() const;
   protected:
      virtual void correctCore(const Parameters& iParameters, Ensemble& iEnsemble) const = 0;
      virtual void getDefaultParametersCore(Parameters& iParameters) const {};
      //! Default: Don't update
      //! Guarantee: Number of ensembles and obs are the same
      virtual void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const {};
      bool mMemberSpecific; ///< Does correction apply to each ensemble member separately?
      bool mPoolParameters; ///< Are the same parameters used on different members?
      bool mEnforceLimits;    ///< Should corrected value be forced to be within variables limits?
   private:
      void enforceLimits(Ensemble& iEnsemble) const;
};
#endif
