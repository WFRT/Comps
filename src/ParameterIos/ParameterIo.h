#ifndef PARAMETER_IO_H
#define PARAMETER_IO_H
#include "../Component.h"
#include "../Location.h"
#include "../Parameters.h"
#include "../Key.h"
#include "../Cache.h"

class Configuration;
class Data;
class ParameterIo : public Component {
   public:
      ParameterIo(const Options& iOptions, const Data& iData);
      static ParameterIo* getScheme(const Options& iOptions, const Data& iData);
      static ParameterIo* getScheme(const std::string& iTag, const Data& iData);
      ~ParameterIo();

      //! Returns true if parameters found, otherwise false
      bool read(Component::Type iType,
                        int iDate,
                        int iInit,
                        float iOffset,
                        int iRegion,
                        const std::string iVariable,
                        const Configuration& iConfiguration,
                        int iIndex,
                        Parameters& iParameters) const;
      void add(Component::Type iType,
                         int iDate,
                         int iInit,
                         float iOffset,
                         int iRegion,
                         const std::string iVariable,
                         const Configuration& iConfiguration,
                         int iIndex,
                         Parameters iParameters);
      //! Write all queued parameters to file. Clear parameters afterwards.
      void write();
   protected:
      virtual void writeCore(const std::map<Key::Par,Parameters>& iParametersWrite) = 0;
      virtual bool readCore(const Key::Par& iKey, Parameters& iParameters) const = 0;

      static const std::string mBaseOutputDirectory;
      std::string mRunDirectory;
      mutable std::map<Component::Type, int> mComponentMap; // Type, index

      std::vector<Component::Type> mComponents;

      // Cannot be private, because some schemes pull extra data than whats being asked for
      // and need to add to cache
      mutable Cache<Key::Par,Parameters> mCache;
   private:
      mutable std::map<Key::Par,Parameters> mParametersWrite;
};
#endif
