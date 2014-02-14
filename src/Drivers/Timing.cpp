#include "../Inputs/Input.h"
#include "../Location.h"
#include "../Member.h"
#include "../Data.h"
#include "../Loggers/Logger.h"
#include "../Loggers/Default.h"
int main(int argc, const char *argv[]) {

   if(argc != 3)
      abort();

   std::stringstream ss0;
   ss0 << argv[1];
   int date; 
   ss0 >> date;
   std::string tag = std::string(argv[2]);

   Global::setLogger(new LoggerDefault(Logger::message));

   Data data("test.run");
   Input* input = Input::getScheme(tag, data);

   const std::vector<Location>& locations = input->getLocations();
   std::vector<float> offsets = input->getOffsets();
   std::vector<Member> members = input->getMembers();
   int init = 0;
   std::string var = "T";

   int s = Global::clock();
   float total = 0;
   int counter = 0;
   for(int i = 0; i < offsets.size(); i++) {
      for(int j = 0; j < locations.size(); j++) {
         for(int k = 0; k < members.size(); k++) {
            float value = input->getValue(date, init, offsets[i], locations[j].getId(), members[k].getId(), var);
            total += value;
            counter++;
         }
      }
   }
   int e = Global::clock();
   std::stringstream ss;
   ss << "Read " << counter << " values. Total: " << total << std::endl;
   ss << "Total time : " << e - s;
   Global::logger->write(ss.str(), Logger::status);
}
