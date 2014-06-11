#include "SchemesHeader.inc"
#include "Neighbourhood.h"

Neighbourhood::Neighbourhood(const Options& iOptions) : Component(iOptions) {

}
#include "Schemes.inc"

std::vector<Location> Neighbourhood::select(const Input* iInput, const Location& iLocation) const {
   return selectCore(iInput, iLocation);
}
