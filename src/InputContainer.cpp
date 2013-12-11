#include "InputContainer.h"
#include "Global.h"
#include "Options.h"
#include "Scheme.h"
#include "Inputs/Input.h"

InputContainer::InputContainer(const Options& iOptions) {
   //mNamelistFolder = Namelist::getDefaultPath();
   //iOptions.getValue("namelistFolder", mNamelistFolder);
}
InputContainer::~InputContainer() {
   std::map<std::string, Input*>::iterator it;
   for(it = mNamelistInputs.begin(); it != mNamelistInputs.begin(); it++) {
      delete it->second;
   }
   for(int i = 0; i < mCustomInputs.size(); i++) {
      delete mCustomInputs[i];
   }
}

Input* InputContainer::getInput(std::string iDataset) const {
   if(!hasBeenLoaded(iDataset)) {
      Options opt;//Scheme::getOptions(iDataset);//, mNamelistFolder);
      Scheme::getOptions(iDataset, opt);//, mNamelistFolder);
      return getInput(opt);
   }
   else {
      return mNamelistInputs[iDataset];
   }
}

Input* InputContainer::getInput(const Options& iOptions) const {
   Input* input =  Input::getScheme(iOptions);

   mCustomInputs.push_back(input);
   assert(input != NULL);
   return input;
}

bool InputContainer::hasBeenLoaded(std::string iDataset) const {
   bool has = mNamelistInputs.find(iDataset) != mNamelistInputs.end();
   return has;
}
