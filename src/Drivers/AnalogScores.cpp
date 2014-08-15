#include "Comps.h"
#include "../Inputs/Input.h"
#include "../Options.h"
#include "../Loggers/Logger.h"
#include "../Loggers/Default.h"
#include "../Variables/Variable.h"
#include "../VarScores/VarScore.h"
#include "../Run.h"
#include <string.h>

int main(int argc, const char *argv[]) {
   double startTime = Global::clock();

   // Get command line options
   Options commandLineOptions;
   bool status = getCommandLineOptions(argc, argv, commandLineOptions);
   if(!status) {
      std::cout << "Community modular post-processing system" << std::endl;
      std::cout << std::endl;
      std::cout << "usage: comps.exe startDate endDate runName [initTime=0] [-gui]" << std::endl;
      std::cout << "   or: comps.exe date              runName [initTime=0] [-gui]" << std::endl;
      std::cout << std::endl;
      std::cout << "Arguments:" << std::endl;
      std::cout << "   date            Process this date (YYYYMMDD)" << std::endl;
      std::cout << "   startDate       Start processing from this date (YYYYMMDD)" << std::endl;
      std::cout << "   endDate         End processing on this date (YYYYMMDD)" << std::endl;
      std::cout << "   runName         Name of run to execute, specified in namelists/*/runs.nl" << std::endl;
      std::cout << "   initTime        Initialization time (format: [H]H)" << std::endl;
      std::cout << "   -gui            Use alternate user interface" << std::endl;
      return 1;
   }

   std::string runTag;
   commandLineOptions.getRequiredValue("runTag",   runTag);

   Global::setLogger(new LoggerDefault(Logger::message));

   // Run options
   Run run(runTag);
   Options runOptions;
   run.getRunOptions(runOptions);
   runOptions.addOptions(commandLineOptions);

   // Set up dates
   std::vector<int> dates;
   getDates(commandLineOptions, dates);

   // Locations
   std::vector<Location> locations;
   getLocations(commandLineOptions, run, locations);

   // Offsets
   std::vector<float> offsets = run.getOffsets();

   Data* data = run.getData();
   Input* input = data->getInput();
   std::vector<std::string> variables = input->getVariables();   

   std::vector<VarScore*> methods;
   methods.push_back(VarScore::getScheme(Options("class=VarScoreCorrelation")));
   methods.push_back(VarScore::getScheme(Options("class=VarScoreBins")));

   // Get observations
   std::string predictand = "T";
   float offset = 24;
   int init = 0;
   Location location = locations[0];
   std::vector<float> obs(dates.size(), Global::MV);
   for(int d = 0; d < dates.size(); d++) {
      int date = dates[d];
      Obs currObs;
      data->getObs(date, init, offset, location, predictand, currObs);
      obs[d] = currObs.getValue();
   }

   /////////////////////////////
   // Loop over each variable //
   //////////////////////////////
   std::vector<std::vector<float> > scores;
   scores.resize(variables.size());
   for(int v = 0; v < (int) variables.size(); v++) {
      std::string variable = variables[v];

      // Get forecasts
      std::vector<float> forecasts(dates.size(), Global::MV);
      for(int d = 0; d < dates.size(); d++) {
         int date = dates[d];
         Ensemble ens = data->getEnsemble(date, init, offset, location, variable);
         forecasts[d] = ens.getMoment(1);
      }

      scores[v].resize(methods.size());

      ///////////////////////
      // Loop over methods //
      ///////////////////////
      for(int m = 0; m < (int) methods.size(); m++) {
         VarScore* method = methods[m];
         scores[v][m] = method->score(obs, forecasts);
      }
   }

   // Report results
   for(int m = 0; m < (int) methods.size(); m++) {
      std::cout << methods[m]->getSchemeName() << " ";
   }
   std::cout << std::endl;

   for(int v = 0; v < (int) variables.size(); v++) {
      std::cout << variables[v] << " ";
      for(int m = 0; m < (int) methods.size(); m++) {
         std::cout << scores[v][m] << " ";
      }
      std::cout << std::endl;
   }
   
   for(int i = 0; i < methods.size(); i++) {
      delete methods[i];
   }
   return 0;
}

bool getCommandLineOptions(int argc, const char *argv[], Options& iOptions) {
   std::stringstream ss;
   std::string dateStart;
   std::string dateEnd;
   std::string runTag = "";
   std::string init   = "0";
   bool foundDateStart = false;
   bool foundDateEnd   = false;

   if(argc == 1)
      return false;

    for(int i = 1; i < argc; i++) {
       std::string option = std::string(argv[i]);
       // Process an option
       if(argv[i][0] == '-') {
           // Remove '-'
           option = option.substr(1,option.size());
           iOptions.addOptions(Options(option));
       }
       // Date 
       else if(argv[i][0] >= '0' && argv[i][0] <= '9') {
          if(option.length() == 8) {
             if(!foundDateStart) {
                dateStart = option;
                foundDateStart = true;
             }
             else if(!foundDateEnd) {
                dateEnd = option;
                foundDateEnd = true;
             }
          }
          else {
             init = option;
          }
       }
       // Run tag
       else {
          runTag = option;
       }
   }

   if(!foundDateStart) {
      // Can't use Global::logger since it hasn't been defined yet
      std::cout << "Missing start date" << std::endl;
      abort();
   }
   if(!foundDateEnd) {
      dateEnd = dateStart;
   }

   // Check dates
   if(dateStart > dateEnd) {
      std::cout << "Start date is after end date" << std::endl;
   }
   iOptions.addOption("dateStart", dateStart);
   iOptions.addOption("dateEnd", dateEnd);
   iOptions.addOption("runTag", runTag);
   iOptions.addOption("init", init);

   int initInt;
   iOptions.getValue("init", initInt);
   if(initInt > 24) {
      std::stringstream ss;
      ss << "Comps.exe: init time (" << initInt << ") cannot be greater than 24";
      Global::logger->write(ss.str(), Logger::error);
   }

   return true;
}

void getDates(const Options& iCommandLineOptions, std::vector<int>& iDates) {
   int dateStart;
   int dateEnd;
   iCommandLineOptions.getRequiredValue("dateStart", dateStart);
   iCommandLineOptions.getRequiredValue("dateEnd",   dateEnd);
   int currDate = dateStart;
   int counter = 0;
   while(currDate <= dateEnd && counter < 1e9) {
      iDates.push_back(currDate);
      counter++;
      currDate = Global::getDate(currDate, 24);
   }
}

void getLocations(const Options& iCommandLineOptions, const Run& iRun, std::vector<Location>& iLocations) {
   bool skipUpdate = false;
   Options runOptions;
   iRun.getRunOptions(runOptions);
   runOptions.getValue("skipUpdate", skipUpdate);

   iLocations = iRun.getLocations();
   std::vector<int> clLocations;
   if(iCommandLineOptions.getValues("locationIds", clLocations)) {
      if(!skipUpdate) {
         std::string message = "Don't use locationIds=[...] on the command line when updating parameters, otherwise they get corrupt";
         Global::logger->write(message, Logger::error);
      }
      std::vector<Location> newLocations;
      for(int i = 0; i < (int) iLocations.size(); i++) {
         for(int j = 0; j < (int) clLocations.size(); j++) {
            if(iLocations[i].getId() == clLocations[j]) {
               newLocations.push_back(iLocations[i]);
            }
         }
      }
      Global::logger->write("Locations filtered by command line options", Logger::warning);
      iLocations = newLocations;
   }
}
