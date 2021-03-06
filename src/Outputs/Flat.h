#ifndef OUTPUT_FLAT_H
#define OUTPUT_FLAT_H
#include "Output.h"

class OutputFlat : public Output {
   public:
      OutputFlat(const Options& iOptions, const Data& iData);
   private:
      void writeCore() const;
      void writeCdf() const;
      void writeEns() const;
      std::string getFilename(int iDate, int iInit, std::string iVariable, std::string iConfiguration) const;
};
#endif
