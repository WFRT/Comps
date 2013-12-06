#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "../Component.h"
#include "../Options.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Distribution.h"
class Component;
class Obs;
class ParameterIo;
class Ensemble;
class Deterministic;
class Region;

/** Contains all schemes for a particular configuration
 *  Does not know how components are linked */
class Configuration : public Component {
   public:
      enum ProcTypeEns  {typeUnCorrected = 0, typeCorrected = 10};
      enum ProcTypeDist {typeUnUpdated = 10, typeUnCalibrated = 20, typeCalibrated = 30};
      Configuration(const Options& iOptions, const Data& iData);
      virtual ~Configuration();
      static Configuration* getScheme(const Options& iOptions, const Data& iData);
      static Configuration* getScheme(const std::string& iTag, const Data& iData);
      void init();

      virtual Distribution::ptr getDistribution(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable,
            ProcTypeDist iType=typeCalibrated) const = 0;
      virtual void getEnsemble(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable,
            Ensemble& iEnsemble,
            ProcTypeEns iType = typeCorrected) const = 0;
      virtual void getDeterministic(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable,
            Deterministic& iDeterministic) const = 0;
      //! Simpler way if only the actual value is needed
      float getDeterministic(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::string iVariable) const;
      virtual void updateParameters(int iDate, int iInit, const std::string& iVariable) = 0;

      std::string getName() const;
      virtual std::string toString() const {return "";};

      // Status
      virtual bool isValid(std::string& iMessage) const = 0;

      void getAllComponents(std::vector<const Component*>& iComponents, std::vector<Component::Type>& iTypes) const;

      static void getOptions(const std::string& iTag, Options& iOptions);
   protected:
      void addComponent(const Component* iComponent, Component::Type iType);
      std::vector<const Component*> getComponents(Component::Type iType) const;

      std::string mName;

      void getParameters(Component::Type iType,
         int iDate,
         int iInit,
         float iOffset,
         int iRegion,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const;
      void setParameters(Component::Type iType,
         int iDate,
         int iInit,
         float iOffset,
         int iRegion,
         const std::string iVariable,
         int iIndex,
         const Parameters& iParameters);
      ParameterIo* mParameters;
      Region* mRegion;
   private:
      int     mNumDaysParameterSearch; // Number of days in the past to search for parameters
      std::vector<const Component*> mComponents;
      std::vector<Component::Type> mComponentTypes;
};
#endif
