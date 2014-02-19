#include "SchemesHeader.inc"
#include "LocationSelector.h"

LocationSelector::LocationSelector(const Options& iOptions) : Component(iOptions) {

}
#include "Schemes.inc"

void LocationSelector::select(const Input* iInput, const Location& iLocation, std::vector<Location>& iLocationSelector) const {
   selectCore(iInput, iLocation, iLocationSelector);
}
