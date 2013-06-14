#include "../Global.h"
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
#include "../DetMetrics/DetMetric.h"
#include "../Scheme.h"
#include "../Variables/Variable.h"
#include "../VarSelectors/Correlation.h"
#include "../VarSelectors/Forward.h"
#if HAVE_MPI
#include <mpi.h>
#endif
int main(int argc, const char *argv[]) {
   double startTime = Global::clock();

   bool useNcurses = false;
   if(useNcurses) {
      Global::setLogger(new LoggerNcurses(Logger::message));
   }
   else {
      Global::setLogger(new LoggerDefault(Logger::message));
   }

   Global::logger->write("test", Logger::critical);

   Data data("an.vars");
   Options runOptions = data.getRunOptions();

   // Metrics
   std::vector<DetMetric*> metrics;
   std::vector<std::string> detMetricTags;
   runOptions.getRequiredValues("detMetrics", detMetricTags);
   for(int i = 0; i < (int) detMetricTags.size(); i++) {
      Options opt;
      Scheme::getOptions(detMetricTags[i], opt);
      DetMetric* metric = DetMetric::getScheme(opt, data);
      metrics.push_back(metric);
   }

   /*
   std::vector<std::string> metricTags;
   runOptions.getRequiredValues("metrics", metricTags);
   for(int i = 0; i < (int) metricTags.size(); i++) {
      Options metricOptions;
      Scheme::getOptions(metricTags[i], metricOptions);
      DetMetric* metric = DetMetric::getScheme(metricOptions);
      metrics.push_back(metric);
   }
   */

   // Variable selectors
   std::vector<std::string> selectorTags;
   runOptions.getRequiredValues("variableSelectors", selectorTags);
   std::vector<VarSelector*> selectors;
   for(int i = 0; i < (int) selectorTags.size(); i++) {
      Options selectorOptions;
      Scheme::getOptions(selectorTags[i], selectorOptions);
      VarSelector* selector = VarSelector::getScheme(selectorOptions, data);
      selectors.push_back(selector);
   }

   int init = 0;
   float offset = 24;
   std::vector<Location> locations;
   data.getOutputLocations(locations);

   Options runSelectorOptions;
   Scheme::getOptions(selectorTags[0], runSelectorOptions);
   VarSelector* runSelector = VarSelector::getScheme(runSelectorOptions, data);

   // Loop over variable selectors
   for(int i = 0; i < (int) selectors.size(); i++) {
      for(int q = 0; q < (int) locations.size(); q++) {
      //for(int q = 0; q < 1; q++) {
         Location location = locations[q];
         Global::logger->setLocationInfo(&locations[q], q+1, locations.size());
         std::vector<std::string> variables;
         selectors[i]->getVariables(data, init, offset, location, variables);

         std::cout << "\033[31m" << selectors[i]->getSchemeName() << "\033[0m" << std::endl;
         for(int k = 0; k < (int) variables.size(); k++) {
            const Variable* var = Variable::get(variables[k]);
            std::cout << "  variables[" << k << "] = " << var->getName() << " (" << var->getDescription() << ")" << std::endl;
         }

         // Run with the selector
         float score = runSelector->run(data, init, offset, locations[0], variables, *metrics[0]);

         std::cout << "\033[31mScore: \033[0m" << score << std::endl;

      }
      delete selectors[i];
   }
   delete runSelector;

   for(int i = 0; i < (int) metrics.size(); i++) {
      delete metrics[i];
   }

   double endTime = Global::clock();
   std::cout << "Total time: " << endTime - startTime << std::endl;
   return 0;
}
