#ifndef PARAMETER_IO_FLAT_H
#define PARAMETER_IO_FLAT_H
#include "ParameterIo.h"

class ParameterIoFlat : public ParameterIo {
   public:
      ParameterIoFlat(const Options& iOptions, const Data& iData);
      bool read(Component::Type iType,
                int iDate,
                int iInit,
                float iOffset,
                const Location& iLocation,
                const std::string iVariable,
                const Configuration& iConfiguration,
                Parameters& iParameters) const;
      void write(Component::Type iType,
                 int iDate,
                 int iInit,
                 float iOffset,
                 const Location& iLocation,
                 const std::string iVariable,
                 const Configuration& iConfiguration,
                 const Parameters& iParameters) const;
   private:
      void writeCore();
      virtual bool readCore(const Key::Par& iKey, Parameters& iParameters) const {};
      std::string getFilename(Component::Type iType,
                              int iDate,
                              int iInit,
                              float iOffset,
                              const Location& iLocation,
                              const std::string iVariable,
                              const Configuration& iConfiguration) const;
};
#endif

