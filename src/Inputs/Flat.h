#ifndef INPUTFLAT_H
#define INPUTFLAT_H
#include "Input.h"

//! Data stored in flat files. Each row represents one offset, each column one ensemble member.
//! Files have the format: dateYYYYMMDD_stationID_variableName
class InputFlat : public Input {
   public:
      InputFlat(const Options& iOptions, const Data& iData);
   protected:
      virtual float getValueCore(const Key::Input& iKey) const;
      std::string getFilename(const Key::Input& iKey) const;
      bool mUseCodeInFilename;
};
#endif
