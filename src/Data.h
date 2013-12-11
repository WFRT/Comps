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
class SelectorClim;
class Qc;
class InputContainer;

/** Caches dates **/
class Data {
   public:
      enum Type {typeObservation = 0, typeForecast = 1, typeNone = 2};
      Data(Options iOptions, InputContainer* iInputContainer);
      ~Data();
      float getValue(int iDate,
                     int iInit,
                     float iOffset, 
                     const Location& iLocation,
                     const Member& iMember,
                     std::string iVariable,
                     Input::Type iType = Input::typeUnspecified) const;
      void getObs(int iDate,
                     int iInit,
                     float iOffset, 
                     const Location& iLocation,
                     std::string iVariable,
                     Obs& iObs) const;
      float getClim(int iDate,
                     int iInit,
                     float iOffset, 
                     const Location& iLocation,
                     std::string iVariable) const;
      void getRecentObs(const Location& iLocation,
                     std::string iVariable,
                     Obs& iObs) const;
      void getEnsemble(int iDate,
                     int iInit,
                     float iOffset,
                     const Location& iLocation,
                     const std::string& iDataset,
                     const std::string& iVariable,
                     Ensemble& iEnsemble) const;
      void getEnsemble(int iDate,
                     int iInit,
                     float iOffset,
                     const Location& iLocation,
                     std::string iVariable,
                     Input::Type iType,
                     Ensemble& iEnsemble) const;

      //! Get the default forecast input
      Input*  getInput() const;
      Input*  getInput(const std::string& iDataset) const;
      // Get input (obs or forecast) for a specific variable
      Input*  getInput(const std::string& iVariable, Input::Type iType) const;
      Input*  getObsInput() const;
      void    getObsLocations(std::vector<Location>& iLocations) const;
      std::string getRunName() const;

      //! 
      int   getCurrentDate() const;
      float getCurrentOffset() const;
      void  setCurrentTime(int iDate, float iOffset);

      void  getMembers(const std::string& iVariable, Input::Type iType, std::vector<Member>& iMembers) const;
      //void    setCurrTime(int iDate, float iOffset);
      bool  hasVariable(const std::string& iVariable, Input::Type iType = Input::typeUnspecified) const;

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
      const SelectorClim* mClimSelector;
      std::vector<const Qc*> mQcs;
      //! Return the quality controlled value, which is valid for a particular date/offset/location/variable
      float qc(float iValue, int iDate, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType=Input::typeForecast) const;
      //! Quality control the ensemble
      void qc(Ensemble& iEnsemble) const;

      static const float mMaxSearchRecentObs = 100;

};
#endif
