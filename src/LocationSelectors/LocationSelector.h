#ifndef LOCATION_SELECTOR_H
#define LOCATION_SELECTOR_H
#include "../Global.h"
#include "../Component.h"
#include "../Location.h"
#include "../Inputs/Input.h"
#include "../Options.h"

// Finds suitable neighbouring locations to a given location
class LocationSelector : public Component {
   public:
      LocationSelector(const Options& iOptions);
      static        LocationSelector* getScheme(const Options& iOptions);
      static        LocationSelector* getScheme(const std::string& iTag);
      //! Creates a vector of locations representing the neighbourhood of 'iLocation'
      void select(const Input* iInput, const Location& iLocation, std::vector<Location>& iLocationSelector) const;
   protected:
      virtual void selectCore(const Input* iInput, const Location& iLocation, std::vector<Location>& iLocationSelector) const = 0;
};
#endif
