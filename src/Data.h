#ifndef DATA_H
#define DATA_H
#include "Global.h"
#include "Inputs/Input.h"
#include "Options.h"

class Variable;
class Configuration;
class Member;
class Location;
class Metric;
class Downscaler;
class SelectorClim;
class Qc;

/** Caches dates **/
class Data {
   public:
      enum Type {typeObservation = 0, typeForecast = 1, typeNone = 2};
      Data(const std::string& iRunTag);
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

      Input*  getInput() const;
      Input*  getInput(const std::string& iDataset) const;
      // Get input (obs or forecast) for a specific variable
      Input*  getInput(const std::string& iVariable, Input::Type iType) const;
      Input*  getObsInput() const;
      void    getDates(std::vector<int>& iDates) const;
      void    getOutputOffsets(std::vector<float>& iOffsets) const;
      void    getOutputLocations(std::vector<Location>& iLocations) const;
      void    getOutputMetrics(const std::string& iVariable, std::vector<Metric*>& iMetrics) const;
      void    getOutputVariables(std::vector<std::string>& iVariables) const;
      void    getOutputConfigurations(const std::string& iVariable, std::vector<Configuration*>& iConfigurations) const;
      std::string getRunName() const;

      //! 
      int getCurrentDate() const;
      float getCurrentOffset() const;
      void setCurrentTime(int iDate, float iOffset);

      //! Cached variables
      Options getRunOptions() const;
      void    getMembers(const std::string& iVariable, Input::Type iType, std::vector<Member>& iMembers) const;
      //void    setCurrTime(int iDate, float iOffset);
      bool hasVariable(const std::string& iVariable, Input::Type iType = Input::typeUnspecified) const;
      static std::string getParameterIo();
   private:
      void    getDates(std::string iDataset, std::vector<int>& iDates) const;
      // Initialize an input
      void init();
      Input* loadInput(std::string iTag, Data::Type iType=Data::typeNone) const;
      bool hasVariable(const std::string& iVariable, const std::string& iDataset) const;

      mutable std::map<std::string,Input*> mInputs; // Variable, input
      mutable std::map<std::string,Input*> mInputsF; // Variable, input
      mutable std::map<std::string,Input*> mInputsO; // Variable, input
      mutable std::map<std::string, std::map<std::string, bool> > mHasVariables; // Input, Variable
      Input* mMainInputF;
      Input* mMainInputO;

      mutable std::map<std::string, Input*> mInputMapF; // Which forecast    input to use for given variable?
      mutable std::map<std::string, Input*> mInputMapO; // Which observation input to use for given variable?
      std::vector<std::string> mInputVariablesF;
      std::vector<std::string> mInputVariablesO;
      mutable std::map<std::string, bool> mInputNames; // Input name, true
      bool hasInput(const std::string& iInputName) const;

      // Run variables/functions
      Options mRunOptions;

      Input* mOutputInput;
      std::vector<float> mOutputOffsets;
      std::vector<Location> mOutputLocations;
      std::map<std::string, std::vector<Configuration*> > mOutputConfigurations; // Variable, Configurations
      std::map<std::string, std::vector<Metric*> >        mOutputMetrics; // Variable, Metrics
      std::vector<std::string> mOutputVariables;
      std::string mRunTag;
      std::vector<int> mDates;

      int mCurrDate;
      float mCurrOffset;

      const Downscaler* mDownscaler;
      const SelectorClim* mClimSelector;
      std::vector<const Qc*> mQc;
      //! Return the quality controlled value, which is valid for a particular date/offset/ocation/variable
      float qc(float iValue, int iDate, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType=Input::typeForecast) const;
      //! Quality control the ensemble
      void  qc(Ensemble& iEnsemble) const;

      static std::string mParameterIo;
      static const float mMaxSearchRecentObs = 100;

};
#endif
