#ifndef SCHEME_H
#define SCHEME_H
#include "Global.h"
#include "Namelist.h"
#include "Options.h"

//! Used to retrieve scheme options from the schemes namelist
class Scheme {
   public:
      //! Retrive options corresponding to scheme
      //! @param iTag Tag of scheme
      //! @param iOptions Options returned here
      static Options getOptions(const std::string iTag);
};
#endif

