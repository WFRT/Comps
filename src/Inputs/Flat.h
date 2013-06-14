#ifndef INPUTFLAT_H
#define INPUTFLAT_H
#include "Input.h"

class InputFlat : public Input {
   public:
      InputFlat(const Options& rOptions, const Data& iData);
      ~InputFlat();
   protected:
      virtual float getValueCore(const Key::Input& iKey) const;
      std::string getFilename(const Key::Input& iKey) const;
      bool mUseCodeInFilename;
};
#endif
