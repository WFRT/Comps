#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "../Component.h"
#include "../Processor.h"
#include "../Options.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Distribution.h"
class Obs;
class ParameterIo;
class Ensemble;
class Pooler;
class Spreader;

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
      virtual void updateParameters(int iDate, int iInit, const std::vector<float>& iOffsets, const std::vector<Location>& iLocations, const std::string& iVariable) = 0;

      std::string getName() const;
      virtual std::string toString() const {return "";};

      // Status
      virtual bool isValid(std::string& iMessage) const = 0;

      //! Returns all processing components that the configuration uses
      std::vector<const Processor*> getAllProcessors() const;
      //! In addition to all processors, return any components that the configuration relies on
      std::vector<const Component*> getAllComponents() const;

      static void getOptions(const std::string& iTag, Options& iOptions);
   protected:
      void addProcessor(const Processor* iProcessor);
      void addExtraComponent(const Component* iComponent);
      std::vector<const Processor*> getProcessors(Component::Type iType) const;

      std::string mName;
      const Data& mData;

      // Downscale parameters
      void getParameters(Component::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         const Location& iLocation,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const;
      // Get parameters for a specific pool id
      void getParameters(Component::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         int iPoolId,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const;
      void setParameters(Component::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         int iPoolId,
         const std::string iVariable,
         int iIndex,
         const Parameters& iParameters);
      ParameterIo* mParameters;
      Pooler* mPooler;
      Spreader* mSpreader;
   private:
      int     mNumDaysParameterSearch; // Number of days in the past to search for parameters
      std::vector<const Processor*> mProcessors;
      std::vector<const Component*> mExtraComponents;
};
#endif
