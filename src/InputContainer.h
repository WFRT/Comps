#ifndef INPUT_CONTAINER_H
#define INPUT_CONTAINER_H
#include "Global.h"
class Options;
class Input;

//! Container for all inputs. All data instances should share the same container
//! so that Inputs can be shared between the data instances.
class InputContainer {
   public:
      InputContainer(const Options& iOptions);
      ~InputContainer();
      //! Get input by looking up iDataset from namelists
      Input* getInput(std::string iDataset) const;
      //! Create a custom input
      Input* getInput(const Options& iOptions) const;
   private:
      //! Has dataset from namelist been loaded yet?
      bool hasBeenLoaded(std::string iDataset) const;
      mutable std::map<std::string, Input*> mNamelistInputs; // Dataset name, input

      //! Non-namelist inputs
      mutable std::vector<Input*> mCustomInputs;

      std::string mNamelistFolder;

};
#endif

