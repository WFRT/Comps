#include "Comps.h"
#include "../Inputs/Input.h"
#include "../Outputs/Output.h"
#include "../Options.h"
#include "../Loggers/Logger.h"
#include "../Loggers/Default.h"
#include "../Loggers/Ncurses.h"
#include "../Selectors/Selector.h"
#include "../Downscalers/Downscaler.h"
#include "../Estimators/Estimator.h"
#include "../Metrics/Metric.h"
#include "../Configurations/Configuration.h"
#include "../Scheme.h"
#include "../Deterministic.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
#include "../Ensembler.h"
#include "../Run.h"
#include <string.h>
#ifdef WITH_NCURSES
#include <curses.h>
#endif

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
   bool useNcurses = false;
   commandLineOptions.getRequiredValue("runTag",   runTag);
   commandLineOptions.getValue("gui", useNcurses);

   // Set up logger
   if(useNcurses) {
      Global::setLogger(new LoggerNcurses(Logger::message));
   }
   else {
      Global::setLogger(new LoggerDefault(Logger::message));
   }

   // Run options
   Run run(runTag);
   Options runOptions;
   run.getRunOptions(runOptions);
   bool skipUpdate = false;
   bool writeAtEnd = false;
   //! How far back should you use obs to update (in number of days)
   //! The problem is that in some cases obs aren't available for several days
   int delayUpdate = 1;
   std::vector<std::string> outputTags;
   //! Don't update parameters
   runOptions.getValue("skipUpdate", skipUpdate);
   runOptions.getValue("delayUpdate", delayUpdate);
   //! Delay the writing of output files until the end, instead of after each date
   runOptions.getValue("writeAtEnd", writeAtEnd);
   runOptions.getValues("outputs", outputTags);
   bool doUpdate = !skipUpdate;

   // Set up dates
   std::vector<int> dates;
   getDates(commandLineOptions, dates);

   // Locations
   std::vector<Location> locations;
   getLocations(commandLineOptions, run, locations);

   // Offsets
   std::vector<float> offsets = run.getOffsets();

   // Loop over variables
   std::vector<std::string> variables = run.getVariables();

   // Figure out which offsets to pull obs for updating parameters
   std::vector<float> uniqueObsOffsets;
   getUniqueObsOffsets(offsets, uniqueObsOffsets);

   // For now only allow runs with init 00UTC
   int init = 0;
   commandLineOptions.getRequiredValue("init", init);

   // Set up Outputs
   std::vector<Output*> outputs = run.getOutputs();

   // Fetch all configurations
   std::vector<Configuration*> allConfigurations = run.getConfigurations();
   Global::logger->setConfigurations(allConfigurations);

   /////////////////////
   // Loop over dates //
   /////////////////////
   for(int d = 0; d < (int) dates.size(); d++) {
      // TODO: Might not need to do this, if we use init in obs instead to determine if 
      // and obs is available
      run.getData()->setCurrentTime(dates[d], 0.01);

      int date = dates[d];
      // TODO
      //run.getData()->setCurrTime(20120101, 0);
      Global::logger->setCurrentDate(date, d, dates.size());

      /////////////////////////
      // Loop over variables //
      /////////////////////////
      for(int v = 0; v < (int) variables.size(); v++) {
         std::string variable = variables[v];

         // Metrics
         std::vector<Metric*> metrics = run.getMetrics(variables[v]);

         //////////////////////////////
         // Loop over configurations //
         //////////////////////////////
         std::vector<Configuration*> configurations = run.getConfigurations(variables[v]);
         for(int i = 0; i < (int) configurations.size(); i++) {
            Configuration& conf = *configurations[i];
            Global::logger->setCurrentConfiguration(i);
            std::stringstream ss0;
            ss0 << variable << "_" << conf.getName();
            std::string configurationName = ss0.str();

            // Update parameters based on yesterday's obs
            if(doUpdate) {
               // TODO: Yesterday's obs may not be available, get the most recent one
               int prevDate = Global::getDate(date, -24*delayUpdate);
               conf.updateParameters(prevDate, init, offsets, locations, variable);
            }

            /////////////////////////
            // Loop over locations //
            /////////////////////////
            for(int l = 0; l < (int) locations.size(); l++) {
               Location location = locations[l];
               Global::logger->setCurrentLocation(&location, l+1, locations.size() );

               // Loop over offsets
               for(int t = 0; t < (int) offsets.size(); t++) {
                  float offset = offsets[t];
                  std::stringstream ss;
                  ss << "      Offset " << t;
                  Global::logger->write(ss.str(), Logger::debug);
                  Ensemble ensemble;
                  // Get probability distribution
                  Distribution::ptr dist = conf.getDistribution(date, init, offset, location, variable);
                  // Observation
                  Obs obs;
                  run.getData()->getObs(date, init, offset, location, variable, obs);
                  for(int o = 0; o < outputs.size(); o++) {
                     // Get slices
                     // TODO:
                     //std::vector<Field> slices;
                     //conf.getSelectorIndicies(location, date, init, offset, variable, slices);
                     //output->addSelectorData(offset, location, slices);
                     outputs[o]->add(dist, configurationName);
                     outputs[o]->add(obs);
                  }

                  for(int m = 0; m < (int) metrics.size(); m++) {
                     //float score = metrics[m]->compute(date, init, offset, obs, conf);
                     Score score;
                     metrics[m]->compute(obs, dist, score);
                     for(int i = 0; i < outputs.size(); i++) {
                        outputs[i]->add(score, configurationName);
                     }
                  }
               }
            }
         }
      }
      // Force write at the end of each day
      if(!writeAtEnd) {
         for(int i = 0; i < outputs.size(); i++) {
            outputs[i]->write();
            std::stringstream ss;
            ss << "Writing forecasts to: " << outputs[i]->getOutputDirectory();
            Global::logger->write(ss.str(), Logger::message);
         }
      }
   }
   if(writeAtEnd) {
      for(int i = 0; i < outputs.size(); i++) {
         outputs[i]->write();
         std::stringstream ss;
         ss << "Writing forecasts to: " << outputs[i]->getOutputDirectory();
         Global::logger->write(ss.str(), Logger::message);
      }
   }
   for(int i = 0; i < outputs.size(); i++) {
      delete outputs[i];
   }

   double endTime = Global::clock();
   std::cout << "Total time: " << endTime - startTime << std::endl;

   Variable::destroy();

   endwin();
   return 0;
}

bool getCommandLineOptions(int argc, const char *argv[], Options& iOptions) {
   std::stringstream ss;
   std::string dateStart;
   std::string dateEnd;
   std::string runTag = "";
   std::string init   = "0";
   std::vector<std::string> options;
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
           options.push_back(option);
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

   ss << " dateStart=" << dateStart;
   ss << " dateEnd="   << dateEnd;
   ss << " runTag="    << runTag;
   ss << " init="      << init;
   // Add boolean options
   for(int i = 0; i < (int) options.size(); i++) {
       ss << " " << options[i];
   }

   iOptions = Options(ss.str());
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
   if(iCommandLineOptions.getValues("locations", clLocations)) {
      if(!skipUpdate) {
         Global::logger->write("Don't use location=[...] on the command line when updating parameters, otherwise they get corrupt", Logger::error);
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

void getUniqueObsOffsets(const std::vector<float>& iOffsets, std::vector<float>& iUniqueObsOffsets) {
   for(int t = 0; t < (int) iOffsets.size(); t++) {
      float offset = fmod(iOffsets[t],24);
      // Check that we don't already have this obs
      bool found = false;
      for(int tt = 0; tt < iUniqueObsOffsets.size(); tt++) {
         if(offset == iUniqueObsOffsets[tt])
            found = true;
      }
      if(!found) {
         iUniqueObsOffsets.push_back(offset);
      }
   }
}
