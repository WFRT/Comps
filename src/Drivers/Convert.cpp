#include "../Global.h"
#include "../Data.h"
#include "../Inputs/Input.h"
#include "../Options.h"
#include "../Loggers/Logger.h"
#include "../Loggers/Default.h"
#include "../Loggers/Ncurses.h"
#include "../Scheme.h"
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
      std::cout << "usage: convert.exe startDate endDate -in=dataset -out=dataset [-dim=dataset]" << std::endl;
      std::cout << "   or: convert.exe date              -in=dataset -out=dataset [-dim=dataset]" << std::endl;
      std::cout << std::endl;
      std::cout << "Arguments:" << std::endl;
      std::cout << "   date         Pull data from this date (YYYYMMDD)" << std::endl;
      std::cout << "   startDate    Starting date of data retrival (YYYYMMDD)" << std::endl;
      std::cout << "   endDate      Ending date of data retrival (YYYYMMDD)" << std::endl;
      std::cout << "   -in          Take data from this dataset" << std::endl;
      std::cout << "   -out         Write data to this dataset" << std::endl;
      std::cout << "   -dim         Limit data retrival to the dimensions (locations, offsets, variables) from this dataset" << std::endl;
      return 1;
   }

   std::string inTag;
   std::string outTag;
   std::string dimTag="";
   int dateStart;
   int dateEnd;
   commandLineOptions.getRequiredValue("in", inTag);
   commandLineOptions.getRequiredValue("out", outTag);
   commandLineOptions.getRequiredValue("dateStart", dateStart);
   commandLineOptions.getRequiredValue("dateEnd",   dateEnd);
   commandLineOptions.getValue("dim", dimTag);

   Global::setLogger(new LoggerDefault(Logger::message));

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
   InputContainer* inputContainer = new InputContainer(Options(""));
   Input* in  = inputContainer->getInput(inTag);
   Input* out = inputContainer->getInput(outTag);
   Input* dim = NULL;
   if(dimTag != "")
      dim = inputContainer->getInput(dimTag);

   for(int i = 0; i < (int) dates.size(); i++) {
      std::cout << "Date: " << dates[i] << std::endl;
      if(dimTag != "") {
         out->write(*in, *dim, dates[i]);
      }
      else {
         out->write(*in, dates[i]);
      }
   }

   Variable::destroy();

   endwin();
   return 0;
}

bool getCommandLineOptions(int argc, const char *argv[], Options& iOptions) {
   std::stringstream ss;
   std::string dateStart;
   std::string dateEnd;
   std::vector<std::string> options;
   bool foundDateStart = false;
   bool foundDateEnd   = false;

   // TODO: Deal with multiple threads
   int numThreads = 1;

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
   }

   if(!foundDateStart) {
      return false;
   }
   if(!foundDateEnd) {
      dateEnd = dateStart;
   }

   ss << " dateStart=" << dateStart;
   ss << " dateEnd="   << dateEnd;
   // Add boolean options
   for(int i = 0; i < (int) options.size(); i++) {
       ss << " " << options[i];
   }

   iOptions = Options(ss.str());
   return true;
}
