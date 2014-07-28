#ifndef INPUTFLAT_H
#define INPUTFLAT_H
#include "Input.h"

//! Data stored in flat files. Each row represents one offset, each column one ensemble member.
//! Files have the format: dateYYYYMMDD_stationID_variableName
class InputFlat : public Input {
   public:
      InputFlat(const Options& iOptions);
   protected:
      virtual float getValueCore(const Key::Input& iKey) const;
      std::string getDefaultFileExtension() const {return "txt";};
      void  writeCore(const Data& iData, const Input& iDimensions, const std::vector<Location>& iLocations, int iDate, int iInit) const;
};
#endif
