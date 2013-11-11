#ifndef OUTPUT_FLAT_H
#define OUTPUT_FLAT_H
#include "Output.h"

class OutputFlat : public Output {
   public:
      OutputFlat(const Options& iOptions, const Data& iData, const Configuration& iConfiguration);
      void write() const;
   private:
      void writeCdf() const;
      void writeEns() const;
      std::string getFilename(std::string iType) const;
};
#endif
