#ifndef OUTPUT_FLAT_H
#define OUTPUT_FLAT_H
#include "Output.h"

class OutputFlat : public Output {
   public:
      OutputFlat(const Options& iOptions, const Data& iData, int iDate, int iInit, const std::string& iVariable, const Configuration& iConfiguration);
      void writeForecasts() const;
      void writeVerifications() const;
   private:
      void writeCdf() const;
      void writeEns() const;
      std::string getFilename(std::string iType) const;
};
#endif
