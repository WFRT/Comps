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
bool mShowAll;
bool mShowSpecific;
void showLocations(Input* input);
void showOffsets(Input* input);
void showDates(Input* input);
void showMembers(Input* input);
void showVariables(Input* input);

int main(int argc, const char *argv[]) {
   Global::setLogger(new LoggerDefault(Logger::message));

   if(argc != 2 && argc != 3 && argc != 4) {
      std::cout << "info.exe <dataset> [<date>] [--show-all]" << std::endl;
      return 0;
   }

   mShowAll = false;
   mShowSpecific = false;
   int date = 0;

   std::string dataset(argv[1]);
   if(argc >= 3) {
      if(!std::strcmp(argv[2], "--show-all")) {
         mShowAll = true;
      }
      else {
         std::stringstream ss;
         ss<< std::string(argv[2]);
         ss >> date;
         mShowSpecific = true;
      }
   }

   Data data("test.run");
   Options opt;
   Scheme::getOptions(dataset, opt);
   Input* input = Input::getScheme(opt, data);

   if(mShowSpecific) {
      // Show results for one specific date
      std::vector<float> offsets;
      input->getOffsets(offsets);
      std::vector<std::string> variables;
      input->getVariables(variables);
      std::vector<Location> locations;
      input->getLocations(locations);
      std::vector<Member> members;
      input->getMembers(members);

      std::vector<float> mean;
      mean.resize(variables.size(), 0);
      std::vector<int> num;
      num.resize(variables.size(), 0);

      std::cout << "Variable     mean       count (max "
                << offsets.size()*locations.size()*members.size() << ")" << std::endl;
      for(int v = 0; v < variables.size(); v++) {
         for(int i = 0; i < offsets.size(); i++) {
            for(int k = 0; k < locations.size(); k++) {
               for(int m = 0; m < members.size(); m++) {
                  float value = input->getValue(date, 0, offsets[i], locations[k].getId(), members[m].getId(), variables[v]);
                  if(Global::isValid(value)) {
                     mean[v] += value;
                     num[v]++;
                  }
               }
            }
         }
         if(num[v] > 0)
            mean[v] /= num[v];
         else
            mean[v] = Global::MV;
         std::cout << std::left << std::setfill(' ') << std::setw(12) << variables[v] << " " << std::setprecision(3) << std::setw(10) << mean[v] << " " << num[v] << std::endl;
      }
   }
   else {
      // Show general statistics
      showLocations(input);
      showOffsets(input);
      showDates(input);
      showVariables(input);
   }
}
void showLocations(Input* input) {
   std::cout << "Locations:" << std::endl;
   std::vector<Location> locations;
   input->getLocations(locations);
   std::cout << "Id       code   lat   lon" << std::endl;
   for(int i = 0; i < locations.size(); i++) {
      if(mShowAll || locations.size() < 100 || i < 5 || i >= locations.size()-5) {
         std::cout << std::setw(6) << locations[i].getId() << " "
                   << std::setw(6) << locations[i].getCode() << " " 
                   << std::setw(6) << std::fixed << std::setprecision(1) << locations[i].getLat() << " "
                   << std::setw(6) << locations[i].getLon() << std::endl;
      }
      else if(i == 5) {
         std::cout << "..." << std::endl;
      }
   }
   std::cout << std::endl;
}
void showOffsets(Input* input) {
   std::cout << "Offsets:" << std::endl;
   std::vector<float> offsets;
   input->getOffsets(offsets);
   std::cout << "Id       offset (h)" << std::endl;
   for(int i = 0; i < offsets.size(); i++) {
      if(mShowAll || offsets.size() < 100 || i < 5 || i >= offsets.size()-5) {
         std::cout << std::setw(6) << i << " "
                   << std::setw(6) << offsets[i] << std::endl;
      }
      else if(i == 5) {
         std::cout << "..." << std::endl;
      }
   }
   std::cout << std::endl;
}
void showDates(Input* input) {
   std::cout << "Dates:" << std::endl;
   std::vector<int> dates;
   input->getDates(dates);
   std::cout << "Id        dates" << std::endl;
   for(int i = 0; i < dates.size(); i++) {
      if(mShowAll || dates.size() < 100 || i < 5 || i >= dates.size()-5) {
         std::cout << std::setw(6) << i
                   << std::setw(9) << dates[i] << std::endl;
      }
      else if(i == 5) {
         std::cout << "..." << std::endl;
      }
   }
   std::cout << std::endl;
}
void showVariables(Input* input) {
   std::cout << "Variables:" << std::endl;
   std::vector<std::string> variables;
   input->getVariables(variables);
   std::cout << "   Id      variable" << std::endl;
   for(int i = 0; i < variables.size(); i++) {
      if(mShowAll || variables.size() < 100 || i < 5 || i >= variables.size()-5) {
         std::cout << std::setw(6) << i << " "
                   << std::setw(12) << variables[i] << std::endl;
      }
      else if(i == 5) {
         std::cout << "..." << std::endl;
      }
   }
}
