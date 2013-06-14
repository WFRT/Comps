#ifndef PARAMETER_IO_H
#define PARAMETER_IO_H
#include "../Component.h"
#include "../Location.h"
#include "../Parameters.h"
#include "../Key.h"
#include "../Cache.h"

class Configuration;
class Data;
class Finder;
class ParameterIo : public Component {
   public:
      ParameterIo(const Options& iOptions, const Data& iData);
      ~ParameterIo();
      //! Returns true if parameters found, otherwise false
      bool read(Component::Type iType,
                        int iDate,
                        int iInit,
                        float iOffset,
                        const Location& iLocation,
                        const std::string iVariable,
                        const Configuration& iConfiguration,
                        int iIndex,
                        Parameters& iParameters) const;
      void add(Component::Type iType,
                         int iDate,
                         int iInit,
                         float iOffset,
                         const Location& iLocation,
                         const std::string iVariable,
                         const Configuration& iConfiguration,
                         int iIndex,
                         Parameters iParameters);
      void write();
      static ParameterIo* getScheme(const Options& iOptions, const Data& iData);
      static ParameterIo* getScheme(const std::string& iTag, const Data& iData);
      const static int maxNumParameters = 10;
   protected:
      virtual void writeCore() = 0;
      virtual bool readCore(const Key::Par& iKey, Parameters& iParameters) const = 0;

      static const std::string mBaseOutputDirectory;
      std::string mRunDirectory;
      mutable std::map<float,int> mOffsetMap;
      mutable std::map<int, int> mLocationMap; // LocationID, index
      mutable std::map<Component::Type, int> mComponentMap; // Type, index

      mutable std::map<Key::Par,Parameters> mParametersWrite;

      std::vector<Location> mLocations;
      std::vector<float> mOffsets;
      std::vector<Component::Type> mComponents;

      std::vector<Location> mOutLocations; // Output locations
      std::vector<Location> mParLocations; // Parameter locations
      mutable std::map<int,int> mOutParMap;// Output location, par location
      // Cannot be private, because some schemes pull extra data than whats being asked for
      // and need to add to cache
      mutable Cache<Key::Par,Parameters> mCache;
      Finder* mFinder;
   private:
      void createOutParMap();

};
#endif

