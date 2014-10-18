#ifndef SELECTOR_H
#define SELECTOR_H
#include "../Processor.h"
#include "../Options.h"

class Data;
class Location;
class Parameters;
class Field;
class Obs;
class Ensemble;
class Neighbourhood;


// Does not need to check that there is valid data available for the slices returned
// Does not need to check if date/init/offset/location/variable are available
//! Represents schemes that define how an ensemble is constructed. The schemes specifies which
//! timepoints and datasets are used to make the ensemble.
class Selector : public Processor {
   public:
      Selector(const Options& iOptions, const Data& iData);
      virtual ~Selector();
      virtual Ensemble select(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters) const;
      static Selector* getScheme(const Options& iOptions, const Data& iData);
      static Selector* getScheme(const std::string& iTag, const Data& iData);
      //! For efficiency reasons, specify how many ensemble members the scheme will provide
      //! (if known)
      virtual int getMaxMembers() const;
      virtual void updateParameters(const std::vector<int>& iDates,
            int iInit,
            const std::vector<float>& iOffsets,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const {};
      //! Is this scheme allowed to give date/offsets in the future?
      virtual bool allowedToCheat() const {return false;};
      //! Does this scheme give a constant ensemble size?
      bool isConstSize() const;
      virtual void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Field>& iFields) const = 0;
   protected:
      const static int mDefaultMaxMembers = 40;
      virtual bool isConstSizeCore() const {return true;};
   private:
      bool mRemoveMissing;
      std::vector<Neighbourhood*> mNeighbourhoods;
};
#endif
