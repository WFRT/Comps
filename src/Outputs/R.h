#ifndef OUTPUT_R_H
#define OUTPUT_R_H
#include "Output.h"
#include <netcdf.hh>

//! Writes data into a text file that can easily be imported 
//! into the R statistical software. Each line has the ensemble
//! and observation for one date/init/offset/location. Writes
//! one file for each configuration. If the file exists, it will
//! append more data to it. The file will include a header row.
//! To read the file in R, use:
//!    x = read.table(filename, header=TRUE)
class OutputR : public Output {
   public:
      OutputR(const Options& iOptions, const Data& iData);
   private:
      void writeCore() const;
      std::string getFilename(std::string iConfiguration) const;
      bool mValidOnly;
};
#endif
