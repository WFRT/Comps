#ifndef INPUT_FLAT_OBS_H
#define INPUT_FLAT_OBS_H
#include "Input.h"

class DataKey;
class InputFlatObs : public Input {
   public:
      InputFlatObs(const Options& rOptions, const Data& iData);
      ~InputFlatObs();
      virtual float getValueCore(const Key::Input& iKey) const;
   protected:
      std::string getFilename(const Key::Input& iKey) const;
      bool mUseCodeInFilename;
      bool getDatesCore(std::vector<int>& iDates) const;
};
#endif
