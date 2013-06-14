#include "Comps.h"
#include "../Data.h"
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
#include "../Variables/Variable.h"
#include "../Distribution.h"
#include "../CustomOutput.h"
#include "../Ensembler.h"
#include <string.h>
#ifdef WITH_NCURSES
#include <curses.h>
#endif

int main(int argc, const char *argv[]) {
   double startTime = Global::clock();

   // Get command line options
   Options commandLineOptions;
   getCommandLineOptions(argc, argv, commandLineOptions);
   int dateStart;
   int dateEnd;
   std::string runTag;
   bool useNcurses = false;
   commandLineOptions.getRequiredValue("runTag",   runTag);
   commandLineOptions.getValue("ncurses", useNcurses);

   // Set up logger
   if(useNcurses) {
      Global::setLogger(new LoggerNcurses(Logger::message));
   }
   else {
      Global::setLogger(new LoggerDefault(Logger::message));
   }

   // Run options
   Data data(runTag);
   Options runOptions = data.getRunOptions();
   bool writeForecasts = false;
   bool writeVerifications = false;
   bool getCdf = false;
   bool getPdf = false;
   bool doCustomOutput = false;
   bool skipUpdate = false;
   bool skipObs = false;
   // How far back should you use obs to update (in number of days)
   // The problem is that in some cases obs aren't available for several days
   int delayUpdate = 1;
   runOptions.getValue("writeForecasts", writeForecasts);
   runOptions.getValue("writeVerifications", writeVerifications);
   runOptions.getValue("getCdf", getCdf);
   runOptions.getValue("getPdf", getPdf);
   runOptions.getValue("customOutput", doCustomOutput);
   runOptions.getValue("skipUpdate", skipUpdate);
   runOptions.getValue("skipObs", skipObs);
   runOptions.getValue("delayUpdate", delayUpdate);
   bool doUpdate = !skipUpdate;
   bool doObs = !skipObs;

   // Set up dates
   std::vector<int> dates;
   getDates(commandLineOptions, dates);

   // Locations
   std::vector<Location> locations;
   getLocations(commandLineOptions, data, locations);

   // Offsets
   std::vector<float> offsets;
   data.getOutputOffsets(offsets);

   // Metrics
   std::vector<Metric*> metrics;
   data.getOutputMetrics(metrics);

   // Loop over variables
   std::vector<std::string> variables;
   data.getOutputVariables(variables);

   // Figure out which offsets to pull obs for updating parameters
   std::vector<float> uniqueObsOffsets;
   getUniqueObsOffsets(offsets, uniqueObsOffsets);

   // For now only allow runs with init 00UTC
   int init = 0;

   /////////////////////////
   // Loop over variables //
   /////////////////////////
   std::map<Key::DateLocVar, CustomOutput> customOutputs;
   int customOutputYear = dateStart / 10000;
   std::cout << "Custom output year = " << customOutputYear << std::endl;
   for(int v = 0; v < (int) variables.size(); v++) {
      std::string variable = variables[v];

      std::vector<float> cdfX;
      std::vector<float> cdfInv;
      const Variable* var = Variable::get(variable);
      var->getCdfX(cdfX);
      var->getCdfInv(cdfInv);

      std::vector<float> pdfX;
      var->getPdfX(pdfX);

      //////////////////////////////
      // Loop over configurations //
      //////////////////////////////
      std::vector<Configuration*> configurations;
      data.getOutputConfigurations(variables[v], configurations);
      Global::logger->setConfigurations(configurations);
      for(int i = 0; i < (int) configurations.size(); i++) {
         Configuration& conf = *configurations[i];
         Global::logger->setConfigurationInfo(i);

         /////////////////////
         // Loop over dates //
         /////////////////////
         for(int d = 0; d < (int) dates.size(); d++) {
            int date = dates[d];
            // TODO
            //data.setCurrTime(20120101, 0);
            Global::logger->setDateInfo(date, d, dates.size());
            std::stringstream ss;
            ss << "Processing date: " << date;
            Global::logger->write(ss.str(), Logger::status);

            Output* output = data.getOutput(date, init, variable, conf);

            // Update parameters based on yesterday's obs
            if(doUpdate) {
               // TODO: Yesterday's obs may not be available, get the most recent one
               int prevDate = Global::getDate(date, -24*delayUpdate);
               conf.updateParameters(locations, variable, prevDate);
            }

            /////////////////////////
            // Loop over locations //
            /////////////////////////
            double startTime = Global::clock();
            for(int l = 0; l < (int) locations.size(); l++) {
               Key::DateLocVar customOutputKey(customOutputYear, locations[l].getId(), variable);
               std::map<Key::DateLocVar, CustomOutput>::const_iterator customOutputIt = customOutputs.find(customOutputKey);
               if(customOutputIt == customOutputs.end()) {
                  std::string localVariableName = data.getInput()->getLocalVariableName(variable);
                  customOutputs[customOutputKey] = CustomOutput(customOutputYear, locations[l], localVariableName);
               }
               std::stringstream ssProgress;
               ssProgress << "Configuration: " << conf.getName() << std::endl;
               ssProgress << "   Date: " << date << " (" << d << "/" << dates.size() << ")" << std::endl;
               ssProgress << "   Location: " << l << "/" << locations.size() << std::endl;
               Global::logger->setLocationInfo(&locations[l], l+1, locations.size() );

               Location location = locations[l];
               std::stringstream ss;
               ss << "Location " << location.getId();
               Global::logger->write(ss.str(), Logger::debug);
               // Loop over offsets
               for(int t = 0; t < (int) offsets.size(); t++) {
                  float offset = offsets[t];
                  std::stringstream ss;
                  ss << "      Offset " << t;
                  Global::logger->write(ss.str(), Logger::debug);
                  if(writeForecasts) {
                     // Get slices
                     // TODO:
                     //std::vector<Slice> slices;
                     //conf.getSelectorIndicies(location, date, init, offset, variable, slices);
                     //output->addSelectorData(offset, location, slices);

                     // Get ensemble
                     Ensemble ensemble;
                     conf.getEnsemble(date, init, offset, location, variable, ensemble);
                     // TODO: Add ensemble instead of its attributes separately
                     output->addEnsembleData(offset, location, ensemble.getValues());

                     // Get deterministic forecast
                     float value = conf.getDeterministic(date, init, offset, location, variable);
                     output->addDetData(offset, location, value);
                  }

                  if(doCustomOutput || getCdf || getPdf) {
                     Distribution::ptr dist = conf.getDistribution(date, init, offset, location, variable);

                     if(doCustomOutput) {
                        Ensemble ens;
                        conf.getEnsemble(date, init, offset, location, variable, ens);
                        Ensembler::getEnsemble(dist, 42, ens);
                        customOutputs[customOutputKey].add(ens);
                     }

                     if(getCdf) {
                        // Get CdfInv
                        for(int c = 0; c < (int) cdfInv.size(); c++) {
                           float cdf = cdfInv[c];
                           float x = dist->getInv(cdf);
                           output->addCdfInvData(offset, location, cdf, x);
                        }

                        // Get P0 and P1
                        if(var->isLowerDiscrete()) {
                           float p0 = dist->getCdf(var->getMin());
                           output->addDiscreteData(offset, location, p0, Discrete::TypeLower);
                        }
                        if(var->isUpperDiscrete()) {
                           float p1 = 1 - dist->getCdf(var->getMax());
                           output->addDiscreteData(offset, location, p1, Discrete::TypeUpper);
                        }
                     }
                     if(getPdf) {
                        // Get CdfInv
                        for(int c = 0; c < (int) pdfX.size(); c++) {
                           float x = pdfX[c];
                           float pdf = dist->getPdf(x);
                           output->addPdfData(offset, location, x, pdf);
                        }

                        // Get P0 and P1
                        if(var->isLowerDiscrete()) {
                           float p0 = dist->getCdf(var->getMin());
                           output->addDiscreteData(offset, location, p0, Discrete::TypeLower);
                        }
                        if(var->isUpperDiscrete()) {
                           float p1 = 1 - dist->getCdf(var->getMax());
                           output->addDiscreteData(offset, location, p1, Discrete::TypeUpper);
                        }
                     }
                  }
                  // Observation
                  if(doCustomOutput || (writeForecasts && doObs) || writeVerifications) {
                     Obs obs;
                     data.getObs(date, init, offset, location, variable, obs);
                     if(doCustomOutput && offset == 0) {
                        customOutputs[customOutputKey].add(obs);
                     }

                     if(writeForecasts || writeVerifications) {
                        output->addObs(obs);
                     }

                     if(writeVerifications) {
                        for(int m = 0; m < (int) metrics.size(); m++) {
                           float score = metrics[m]->compute(date, init, offset, obs, conf);
                           output->addMetricData(offset, location, score, *metrics[m]);
                        }
                     }
                  }
                  // Get deterministic value
                  if(writeForecasts) {
                     float smoothedValue = conf.getDeterministic(date, init, offset, location, variable);
                     output->addDetData(offsets[t], location, smoothedValue);
                  }
               }
            }
            double endTime = Global::clock();

            if(writeForecasts) {
               double startTime = Global::clock();
               output->writeForecasts();
               double endTime = Global::clock();
               std::stringstream ss;
               ss << "Writing forecasts to: " << output->getOutputFileName();
               Global::logger->write(ss.str(), Logger::message);
            }
            if(writeVerifications) {
               double startTime = Global::clock();
               output->writeVerifications();
               double endTime = Global::clock();
            }

            delete output;
         }
      }
   }
   for(int i = 0; i < (int) metrics.size(); i++) {
      delete metrics[i];
   }
   if(doCustomOutput) {
      std::map<Key::DateLocVar, CustomOutput>::const_iterator it;
      for(it = customOutputs.begin(); it != customOutputs.end(); it++) {
         it->second.write();
      }
   }

   double endTime = Global::clock();
   std::cout << "Total time: " << endTime - startTime << std::endl;

   Variable::destroy();

   endwin();
   return 0;
}

void getCommandLineOptions(int argc, const char *argv[], Options& iOptions) {
   std::stringstream ss;
   std::string dateStart;
   std::string dateEnd;
   std::string runTag;
   std::vector<std::string> options;
   bool foundDateStart = false;
   bool foundDateEnd   = false;

   if(argc == 1) {
      std::cout << "Community modular post-processing system" << std::endl;
      std::cout << "comps.exe startDate [endDate] runName [-ncurses]" << std::endl;
      std::cout << "   startDate, endDate format: YYYYMMDD" << std::endl;
      std::cout << "   runName: specified in namelists/*/runs.nl" << std::endl;
      std::cout << "   -ncurses: Use alternate user interface" << std::endl;
      abort();
   }

    for(int i = 1; i < argc; i++) {
       // Process an option
       if(argv[i][0] == '-') {
           std::string option = std::string(argv[i]);
           // Remove '-'
           option = option.substr(1,option.size());
           options.push_back(option);
       }
       // Date 
       else if(argv[i][0] >= '0' && argv[i][0] <= '9') {
          if(!foundDateStart) {
             dateStart = std::string(argv[i]);
             foundDateStart = true;
          }
          else if(!foundDateEnd) {
             dateEnd = std::string(argv[i]);
             foundDateEnd = true;
          }
       }
       // Run tag
       else {
          runTag = std::string(argv[i]);
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
   // Add boolean options
   for(int i = 0; i < (int) options.size(); i++) {
       ss << " " << options[i];
   }

   iOptions = Options(ss.str());
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

void getLocations(const Options& iCommandLineOptions, const Data& iData, std::vector<Location>& iLocations) {
   Options runOptions = iData.getRunOptions();
   bool skipUpdate = false;
   runOptions.getValue("skipUpdate", skipUpdate);

   iData.getOutputLocations(iLocations);
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
