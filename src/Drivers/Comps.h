#include "../Global.h"
class Options;
class Data;
void getCommandLineOptions(int argc, const char *argv[], Options& iOptions);
void getDates(const Options& iCommandLineOptions, std::vector<int>& iDates);
void getLocations(const Options& iCommandLineOptions, const Data& iData, std::vector<Location>& iLocations);
void getUniqueObsOffsets(const std::vector<float>& iOffsets, std::vector<float>& iUniqueObsOffsets);
