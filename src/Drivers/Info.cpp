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
bool mShowStat;
bool mShowSpecific;
void showLocations(Input* input);
void showOffsets(Input* input);
void showDates(Input* input);
void showMembers(Input* input);
void showVariables(Input* input);
int getNumValid(Input* input, std::string variable, int locationId, int startDate, int endDate);

int main(int argc, const char *argv[]) {
   Global::setLogger(new LoggerDefault(Logger::message));

   if(argc != 2 && argc != 3 && argc != 4) {
      std::cout << "Show information about a COMPS dataset" << std::endl;
      std::cout << std::endl;
      std::cout << "usage: info.exe dataset [--show-all]       show information about dataset" << std::endl;
      std::cout << "   or: info.exe dataset date               show detailed information for a specific date" << std::endl;
      std::cout << "   or: info.exe dataset startDate endDate  show detailed information for a date range" << std::endl;
      std::cout << std::endl;
      std::cout << "Arguments:" << std::endl;
      std::cout << "   dataset         Tag of dataset from namelist" << std::endl;
      std::cout << "   date            Date in YYYYMMDD format" << std::endl;
      std::cout << "   --show-all      Do not limit variables/dates/offsets to the first 100" << std::endl;

      return 0;
   }

   mShowAll = false;
   mShowSpecific = false;
   int startDate = Global::MV;
   int endDate   = Global::MV;

   std::string dataset(argv[1]);
   if(argc >= 3) {
      for(int i = 2; i < argc; i++) {
         if(!std::strcmp(argv[i], "--show-all")) {
            mShowAll = true;
         }
         else if(!Global::isValid(startDate)) {
            std::stringstream ss;
            ss<< std::string(argv[i]);
            ss >> startDate;
            endDate = startDate;
         }
         else {
            std::stringstream ss;
            ss<< std::string(argv[i]);
            ss >> endDate;
         }
      }
   }
   if(Global::isValid(startDate)) {
      mShowSpecific = true;
   }

   InputContainer inputContainer(Options(""));
   Input* input = inputContainer.getInput(dataset);

   if(mShowSpecific) {
      // Show results for one specific date
      std::vector<float> offsets = input->getOffsets();
      std::vector<std::string> variables = input->getVariables();
      std::vector<Location> locations = input->getLocations();
      std::vector<Member> members = input->getMembers();

      std::vector<double> min(variables.size(), Global::INF);
      std::vector<double> mean(variables.size(), 0);
      std::vector<double> max(variables.size(), -Global::INF);
      std::vector<long> num(variables.size(), 0);

      std::cout << "Variable     min   mean  max        count (max "
                << offsets.size()*locations.size()*members.size() << ")" << std::endl;
      for(int v = 0; v < variables.size(); v++) {
         int date = startDate;
         while(date <= endDate) {
            for(int i = 0; i < offsets.size(); i++) {
               for(int k = 0; k < locations.size(); k++) {
                  for(int m = 0; m < members.size(); m++) {
                     float value = input->getValue(date, 0, offsets[i], locations[k].getId(), members[m].getId(), variables[v]);
                     if(Global::isValid(value)) {
                        min[v] = std::min(min[v], value);
                        mean[v] += value;
                        max[v] = std::max(max[v], value);
                        num[v]++;
                     }
                  }
               }
            }
            date = Global::getDate(date, 24);
         }
         if(num[v] > 0)
            mean[v] /= num[v];
         else {
            min[v] = Global::MV;
            mean[v] = Global::MV;
            max[v] = Global::MV;
         }
         std::cout << std::left << std::setfill(' ') << std::setw(12) << variables[v] << " "
                   << std::setprecision(3) << std::setw(5) << min[v] << " "
                   << std::setprecision(3) << std::setw(5) << mean[v]<< " "
                   << std::setprecision(3) << std::setw(10) << max[v] << " " << num[v] << std::endl;
      }

      // Show results for each location
      std::cout << std::endl;
      std::cout << "Counts for each location and variable" << std::endl;
      std::cout << "Id    ";
      for(int v = 0; v < variables.size(); v++) {
         std::cout << std::setw(12) << variables[v];
      }
      std::cout << std::endl;
      for(int k = 0; k < locations.size(); k++) {
         std::cout << std::setw(6) << locations[k].getId();
         for(int v = 0; v < variables.size(); v++) {
            int num = 0;
            int date = startDate;
            while(date <= endDate) {
               for(int i = 0; i < offsets.size(); i++) {
                  for(int m = 0; m < members.size(); m++) {
                     float value = input->getValue(startDate, 0, offsets[i], locations[k].getId(), members[m].getId(), variables[v]);
                     if(Global::isValid(value)) {
                        num++;
                     }
                  }
               }
               date = Global::getDate(date, 24);
            }
            std::cout << std::setw(12) << num;
         }
         std::cout << std::endl;
      }
   }
   else {
      // Show general statistics
      showLocations(input);
      showOffsets(input);
      showDates(input);
      showVariables(input);
      showMembers(input);
   }
}

void showLocations(Input* input) {
   const std::vector<Location>& locations = input->getLocations();
   std::cout << "Locations (" << locations.size() << " total):" << std::endl;
   std::cout << "Id       code   lat     lon   elev gradient";
   std::cout << std::endl;
   for(int i = 0; i < locations.size(); i++) {
      if(mShowAll || locations.size() < 100 || i < 5 || i >= locations.size()-5) {
         std::cout << std::setw(6) << locations[i].getId() << " "
                   << std::setw(6) << locations[i].getCode() << " " 
                   << std::setw(6) << std::fixed << std::setprecision(1) << locations[i].getLat() << " "
                   << std::setw(6) << locations[i].getLon() << " "
                   << std::setw(6) << std::fixed << std::setprecision(1) << locations[i].getElev() << " ";
         float gradX = locations[i].getGradientX();
         float gradY = locations[i].getGradientY();
         if(Global::isValid(gradX) && Global::isValid(gradY)) {
            std::cout << std::setw(6) << std::fixed << std::setprecision(3) << gradX << ","
                      << std::setw(6) << std::fixed << std::setprecision(3) << gradY;
         }
         else {
            std::cout << "            ";
         }
         std::cout << std::endl;
      }
      else if(i == 5) {
         std::cout << "..." << std::endl;
      }
   }
   std::cout << std::endl;
}
void showOffsets(Input* input) {
   std::vector<float> offsets = input->getOffsets();
   std::cout << "Offsets (" << offsets.size() << " total):" << std::endl;
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
   std::vector<int> dates;
   std::cout << "Dates (" << dates.size() << " total):" << std::endl;
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
   std::vector<std::string> variables = input->getVariables();
   std::cout << "Variables (" << variables.size() << " total):" << std::endl;
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
void showMembers(Input* input) {
   std::vector<Member> members = input->getMembers();
   std::cout << "Members (" << members.size() << " total):" << std::endl;
   std::cout << "   Id         model resolution" << std::endl;
   for(int i = 0; i < members.size(); i++) {
      if(mShowAll || members.size() < 100 || i < 5 || i >= members.size()-5) {
         float res = members[i].getResolution();
         std::cout << std::setw(6) << i << " "
                   << std::setw(12) << members[i].getModel();
         if(Global::isValid(res))
            std::cout << std::setw(9) << res << "km";
         std::cout << std::endl;
      }
      else if(i == 5) {
         std::cout << "..." << std::endl;
      }
   }
   std::cout << std::endl;
}

int getNumValid(Input* input, std::string variable, int locationId, int startDate, int endDate) {
   std::vector<float> offsets = input->getOffsets();
   std::vector<Location> locations = input->getLocations();
   std::vector<Member> members = input->getMembers();

   // float min=0,mean=0,max=0,num=0;
   float num = 0;

   int date = startDate;
   while(date <= endDate) {
      for(int i = 0; i < offsets.size(); i++) {
         for(int k = 0; k < locations.size(); k++) {
            for(int m = 0; m < members.size(); m++) {
               float value = input->getValue(date, 0, offsets[i], locationId, members[m].getId(), variable);
               if(Global::isValid(value)) {
                  // min = std::min(min, value);
                  // mean += value;
                  // max = std::max(max, value);
                  num++;
               }
            }
         }
      }
      date = Global::getDate(date, 24);
   }
   return num;
}
