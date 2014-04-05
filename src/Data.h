#ifndef DATA_H
#define DATA_H
#include "Global.h"
#include "Inputs/Input.h"
#include "InputContainer.h"
#include "Options.h"

class Variable;
class Configuration;
class Member;
class Location;
class Metric;
class Downscaler;
class Selector;
class Qc;
class InputContainer;

class Data {
   public:
      enum Type {typeObservation = 0, typeForecast = 1, typeNone = 2};
      Data(Options iOptions=Options(), InputContainer* iInputContainer = new InputContainer(Options()));
      ~Data();

      // Access data
      void getObs(int iDate,
                     int iInit,
                     float iOffset, 
                     const Location& iLocation,
                     std::string iVariable,
                     Obs& iObs) const;
      //! Get ensemble from a particular dataset
      Ensemble getEnsemble(int iDate,
                     int iInit,
                     float iOffset,
                     const Location& iLocation,
                     const std::string& iVariable,
                     const std::string& iDataset) const;
      //! Get ensemble from default forecast or obs dataset
      Ensemble getEnsemble(int iDate,
                     int iInit,
                     float iOffset,
                     const Location& iLocation,
                     const std::string& iVariable,
                     Input::Type iType=Input::typeForecast) const;
      //! Get a specific forecast from a specific ensemble member
      float getValue(int iDate,
                     int iInit,
                     float iOffset,
                     const Location& iLocation,
                     const Member& iMember,
                     const std::string& iVariable) const;
      //! Get most recent observation prior to date/init/offset
      void getMostRecentObs(int iDate,
                     int iInit,
                     float iOffset, 
                     const Location& iLocation,
                     std::string iVariable,
                     Obs& iObs) const;
      //! Get climatological value
      float getClim(int iDate,
                     int iInit,
                     float iOffset, 
                     const Location& iLocation,
                     std::string iVariable) const;

      //! Get the default forecast input
      Input*  getInput() const;
      Input*  getInput(const std::string& iDataset) const;
      // Get input (obs or forecast) for a specific variable
      Input*  getInput(const std::string& iVariable, Input::Type iType) const;
      Input*  getObsInput() const;
      void    getObsLocations(std::vector<Location>& iLocations) const;
      std::string getRunName() const;

      void  getMembers(const std::string& iVariable, Input::Type iType, std::vector<Member>& iMembers) const;
      //void    setCurrTime(int iDate, float iOffset);
      bool  hasVariable(const std::string& iVariable, Input::Type iType = Input::typeUnspecified) const;
      Downscaler* getDownscaler() const;

   private:
      InputContainer* mInputContainer;
      std::string mRunName;
      // Initialize an input
      void loadInput(const std::string& iDataset) const;
      bool hasVariable(const std::string& iVariable, const std::string& iDataset) const;

      mutable std::map<std::string,Input*> mInputs; // Variable, input
      mutable std::map<std::string,Input*> mInputsF; // Variable, input
      mutable std::map<std::string,Input*> mInputsO; // Variable, input
      mutable std::map<std::string, std::map<std::string, bool> > mHasVariables; // Input, Variable
      mutable Input* mMainInputF;
      mutable Input* mMainInputO;

      mutable std::map<std::string, Input*> mInputMapF; // Which forecast    input to use for given variable?
      mutable std::map<std::string, Input*> mInputMapO; // Which observation input to use for given variable?
      std::vector<std::string> mInputVariablesF;
      std::vector<std::string> mInputVariablesO;
      mutable std::map<std::string, bool> mInputNames; // Input name, true
      bool hasInput(const std::string& iInputName) const;

      int mCurrDate;
      float mCurrOffset;

      Downscaler* mDownscaler;
      Selector* mClimSelector;
      std::vector<const Qc*> mQcs;
      //! Return the quality controlled value, which is valid for a particular date/offset/ocation/variable
      float qc(float iValue, int iDate, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType=Input::typeForecast) const;
      //! Quality control the ensemble
      void qc(Ensemble& iEnsemble) const;

      static float mMaxSearchRecentObs;

};
#endif
