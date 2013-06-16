#ifndef SELECTOR_H
#define SELECTOR_H
#include "../Component.h"
#include "../Options.h"

class Data;
class Location;
class Parameters;
class Slice;
class Obs;

//! Does not need to check that there is valid data available for the slices returned
//! Does not need to check if date/init/offset/location/variable are available
class Selector : public Component {
   public:
      //! Options:
      //! - removeMissing (false): Removes ensemble members that value missing forecasts
      Selector(const Options& iOptions, const Data& iData);
      void select(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Slice>& iSlices) const;
      static Selector* getScheme(const Options& iOptions, const Data& iData);
      static Selector* getScheme(const std::string& iTag, const Data& iData);
      //! For efficiency reasons, specify how many ensemble members the scheme will provide
      //! (if known)
      virtual int getMaxMembers() const;
      virtual void updateParameters(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const {};
      //! Is this scheme allowed to give date/offsets in the future?
      virtual bool allowedToCheat() const {return false;};
      //! Does this scheme give a constant ensemble size?
      bool isConstSize() const;
   protected:
      const static int mDefaultMaxMembers = 40;
      virtual void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Slice>& iSlices) const = 0;
      virtual bool isConstSizeCore() const {return true;};
   private:
      bool mRemoveMissing;
};
#endif
