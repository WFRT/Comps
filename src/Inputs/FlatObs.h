#ifndef INPUT_FLAT_OBS_H
#define INPUT_FLAT_OBS_H
#include "Input.h"

//! Data stored in flat files. Each row represents one date, each column one offset.
//! Useful for observations, since only one ensemble member is supported.
//! Files have the format: dateYYYYMMDD_variableName
class InputFlatObs : public Input {
   public:
      InputFlatObs(const Options& iOptions);
      virtual float getValueCore(const Key::Input& iKey) const;
   protected:
      std::string getFilename(const Key::Input& iKey) const;
      bool mUseCodeInFilename;
      bool getDatesCore(std::vector<int>& iDates) const;
};
#endif
