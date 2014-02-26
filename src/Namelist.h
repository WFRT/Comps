#ifndef NAMELIST_H
#define NAMELIST_H
#include "Global.h"
#include "Options.h"

//! Represents information stored in a namelist file
//! Each line has a tag followed by options (key/values):
//! someTag key1=attributes key2=attributes 
//!
//! When options are created, the tag is added as follows (depending if project has been specified):
//! tag=someTag key1=attributes key2=attributes
//! tag=project.someTag key1=attributes key2=attributes
class Namelist {
   public:
      //! Create a namelist from the ./namelists/<iProject>/<iType>.nl file
      Namelist(const std::string& iType, const std::string& iProject);
      //! Create a namelist from a specific file
      Namelist(const std::string& iFilename);

      //! Get a list of all tags in the namelist
      std::vector<std::string> getTags() const;
      //! Get options corresponding to a tag
      bool getOptions(const std::string& iTag, Options& iOptions) const;

      //! Get the path to where the namelists are
      static std::string getDefaultLocation();

   private:
      void parse(const std::string& iFilename);
      // Store tags and corresponding options
      mutable std::map<std::string,Options> mMap;
      std::string mProject;
};
#endif
