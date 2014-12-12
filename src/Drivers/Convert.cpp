#include "../Global.h"
#include "../Data.h"
#include "../Inputs/Input.h"
#include "../Options.h"
#include "../Loggers/Logger.h"
#include "../Loggers/Default.h"
#include "../Loggers/Ncurses.h"
#include "../Scheme.h"
#include "../Location.h"
#include "../Variables/Variable.h"
#include <string.h>
#ifdef WITH_NCURSES
#include <curses.h>
#endif
////////////////////////////
// Convert between inputs //
////////////////////////////
bool getCommandLineOptions(int argc, const char *argv[], Options& iOptions);

int main(int argc, const char *argv[]) {
   double startTime = Global::clock();

   Options commandLineOptions;
   bool status = getCommandLineOptions(argc, argv, commandLineOptions);
   if(!status) {
      std::cout << "Convert data from one COMPS dataset to another" << std::endl;
      std::cout << std::endl;
      std::cout << "usage: convert.exe startDate [endDate] init -in=dataset -out=dataset [-dim=dataset] [-loc=dataset] [-o=offsets] [-v=variables] [-d=downscalers]" << std::endl;
      std::cout << std::endl;
      std::cout << "Arguments:" << std::endl;
      std::cout << "   startDate    Starting date of data retrival (YYYYMMDD)" << std::endl;
      std::cout << "   endDate      Ending date of data retrival (YYYYMMDD)" << std::endl;
      std::cout << "   -in          Take data from this dataset" << std::endl;
      std::cout << "   -out         Write data to this dataset" << std::endl;
      std::cout << "   -loc         Use locations from dataset" << std::endl;
      std::cout << "   offsets      Retrieve data for these offsets" << std::endl;
      std::cout << "   variables    Only convert these variables" << std::endl;
      std::cout << "   downscalers  Use these downscalers for the variables (must be size 1 or same size as variables)" << std::endl;
      return 1;
   }

   Global::setLogger(new LoggerDefault(Logger::message));

   std::string inTag;
   std::string outTag;
   std::string dimTag="";
   std::string locTag="";
   int dateStart;
   int dateEnd;
   int init;
   std::vector<std::string> diagnosticVariables;
   commandLineOptions.getRequiredValue("in", inTag);
   commandLineOptions.getRequiredValue("out", outTag);
   commandLineOptions.getRequiredValue("dateStart", dateStart);
   commandLineOptions.getRequiredValue("dateEnd",   dateEnd);
   commandLineOptions.getRequiredValue("init",  init);
   commandLineOptions.getValue("loc",  locTag);
   commandLineOptions.getValue("dim", dimTag);
   commandLineOptions.getValues("diagnosticVariables", diagnosticVariables);

   // Variables
   std::string downscalerVariablesLine;
   commandLineOptions.getValue("v", downscalerVariablesLine);
   std::vector<std::string> variables;
   commandLineOptions.getValues("v", variables);

   // Downscalers
   std::string downscalersLine;
   commandLineOptions.getValue("d", downscalersLine);
   std::vector<std::string> downscalers;
   commandLineOptions.getValues("d", downscalers);

   // Offsets
   std::vector<float> offsets;
   commandLineOptions.getValues("o", offsets);

   // Check inputs
   if(dateStart > dateEnd) {
      Global::logger->write("Start date is after end date", Logger::error);
   }

   // Set up dates
   std::vector<int> dates;
   int currDate = dateStart;
   int counter = 0;
   while(currDate <= dateEnd && counter < 1000) {
      dates.push_back(currDate);
      counter++;
      currDate = Global::getDate(currDate, 24);
   }

   // Set up inputs
   Options dataOptions;
   dataOptions.addOption("inputs", inTag);
   if(downscalers.size() == 1) {
      dataOptions.addOption("downscaler", downscalers[0]);
   }
   else if(downscalers.size() > 1) {
      dataOptions.addOption("downscalerVariables", downscalerVariablesLine);
      dataOptions.addOption("downscalers", downscalersLine);
   }
   if(diagnosticVariables.size() > 0) {
      dataOptions.addOptions("variables", diagnosticVariables);
   }
   Data data(dataOptions);
   Input* in = data.getInput(inTag);
   Input* out = data.getInput(outTag);
   Input* dim = NULL;
   if(dimTag != "")
      dim = data.getInput(dimTag);

   Input* loc;
   if(locTag != "") {
      loc = data.getInput(locTag);
   }
   else {
      loc = in;
   }
   std::vector<Location> locations = loc->getLocations();
   if(offsets.size() == 0) {
      offsets = in->getOffsets();
   }
   if(variables.size() == 0) {
      variables = in->getVariables();
   }
   std::vector<Member> members = in->getMembers();

   out->write(data, dates, init, offsets, locations, members, variables);

   endwin();
   return 0;
}

bool getCommandLineOptions(int argc, const char *argv[], Options& iOptions) {
   std::stringstream ss;
   std::string dateStart;
   std::string dateEnd;
   std::string init;
   std::vector<std::string> options;

   // TODO: Deal with multiple threads
   int numThreads = 1;

   std::vector<std::string> numbers;

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
          numbers.push_back(std::string(argv[i]));
       }
    }

    if(numbers.size() != 2 && numbers.size() != 3) {
       return false;
    }

    dateStart = numbers[0];
    if(numbers.size() == 2) {
       dateEnd = dateStart;
       init    = numbers[1];
    }
    else if(numbers.size() == 3)  {
       dateEnd = numbers[1];
       init    = numbers[2];
    }

   ss << " dateStart=" << dateStart;
   ss << " dateEnd="   << dateEnd;
   ss << " init="      << init;
   // Add boolean options
   for(int i = 0; i < (int) options.size(); i++) {
       ss << " " << options[i];
   }

   iOptions = Options(ss.str());
   return true;
}
