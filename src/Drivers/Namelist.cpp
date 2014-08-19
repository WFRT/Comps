#include <iostream>
#include <iomanip>
#include "../Global.h"
#include "../Options.h"
#include "../Data.h"
#include "../Scheme.h"
#include "../Inputs/Input.h"
#include "../Loggers/Logger.h"
#include "../Loggers/Default.h"
#include "../Location.h"
#include "../Member.h"
#include <cstring>

int main(int argc, const char *argv[]) {
   Global::setLogger(new LoggerDefault(Logger::message));

   if(argc < 4) {
      std::cout << "Creates a location namelist by removing locations without data. Any location" << std::endl;
      std::cout << "with at least one valid data point will be kept. Unless specified otherwise," << std::endl;
      std::cout << "data for all offsets, variables, and members are checked." << std::endl;
      std::cout << std::endl;
      std::cout << "usage: namelist.exe dataset outputDir startDate [endDate] [-v variables] [--remove--locations]";
      std::cout << std::endl;
      std::cout << "Arguments:" << std::endl;
      std::cout << "   outputDir           Place new namelists here" << std::endl;
      std::cout << "   startDate           Start searching for data for this date (YYYYMMDD)" << std::endl;
      std::cout << "   endDate             End date for search" << std::endl;
      std::cout << "   variables           Only check for data for these variables" << std::endl;
      // std::cout << "   --remove-offsets    Tag of dataset from namelist" << std::endl;
      // std::cout << "   --remove-variables  Remove variabless without data" << std::endl;
      std::cout << "   --remove-locations  Remove locations with no data" << std::endl;

      return 0;
   }

   int startDate = Global::MV;
   int endDate   = Global::MV;

   std::string dataset(argv[1]);
   std::string outputDir(argv[2]);
   std::stringstream ss;
   ss << std::string(argv[3]);
   ss >> startDate;
   endDate = startDate;
   bool removeOffsets   = false;
   bool removeVariables = false;
   bool removeLocations = false;
   std::vector<std::string> keepVariables;

   for(int i = 4; i < argc; i++) {
      if(!std::strcmp(argv[i], "--remove-variabless")) {
         removeVariables = true;
      }
      else if(!std::strcmp(argv[i], "--remove-locations")) {
         removeLocations = true;
      }
      else if(!std::strcmp(argv[i], "-v")) {
         std::string keepVariablesString = argv[i+1];
         std::stringstream ss(keepVariablesString);
         Options opt("test="+keepVariablesString);
         opt.getValues("test", keepVariables);
         assert(argc > i+1);
         i++;
      }
      // else if(!std::strcmp(argv[i], "--remove-offsets")) {
      //    removeOffsets = true;
      //}
      else {
         std::stringstream ss;
         ss<< std::string(argv[i]);
         ss >> endDate;
      }
   }

   InputContainer inputContainer(Options(""));
   Input* input = inputContainer.getInput(dataset);

   std::vector<float> offsets = input->getOffsets();
   std::vector<std::string> variables = input->getVariables();
   std::vector<Location> locations = input->getLocations();
   std::vector<Member> members = input->getMembers();

   std::vector<int> numLocations(locations.size(), 0);
   std::vector<int> numVariables(variables.size(), 0);
   std::vector<int> numOffsets(offsets.size(), 0);

   if(keepVariables.size() > 0) {
      variables = keepVariables;
   }

   // Check that variables are valid

   for(int v = 0; v < variables.size(); v++) {
      if(!input->hasVariable(variables[v])) {
         Global::logger->write("Input does not contain variable: " + variables[v], Logger::warning);
      }
      else {
         int date = startDate;
         while(date <= endDate) {
            for(int i = 0; i < offsets.size(); i++) {
               for(int k = 0; k < locations.size(); k++) {
                  for(int m = 0; m < members.size(); m++) {
                     float value = input->getValue(date, 0, offsets[i], locations[k].getId(), members[m].getId(), variables[v]);
                     if(Global::isValid(value)) {
                        numLocations[k]++;
                        numVariables[v]++;
                        numOffsets[i]++;
                     }
                  }
               }
            }
            date = Global::getDate(date, 24);
         }
      }
   }

   // Write locations namelist
   std::string locationsFile = outputDir + "/" + "locations.nl";
   std::ofstream ofs(locationsFile.c_str(), std::ios_base::out);
   if(ofs) {
      for(int i = 0; i < locations.size(); i++) {
         if(numLocations[i] > 0) {
            ofs << locations[i].getNamelistLine() << std::endl;
         }
      }
      ofs.close();
   }
   else {
      Global::logger->write("Could not write file: " + locationsFile, Logger::error);
   }
}
