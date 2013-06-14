#include "../Global.h"
#include "../Loggers/Logger.h"
#include <iomanip>
void writeTiming(std::string name, float t0, float t1, float value) {
   std::cout << name << std::fixed << std::setprecision(5) << t0 << " " << t1 << " " << value << std::endl;
}
int main(void) {
   std::vector<std::map<int, float> > cache0; // LocationId, id, value
   std::map<int, float> cache1; // id value

   int dateN = 100000;
   int locN = 1;

   {
      float value = 0;
      double s = Global::clock();
      cache0.resize(locN);
      for(int date = 0; date < dateN; date++) {
         for(int loc = 0; loc < locN; loc++) {
            cache0[loc][date] = loc*loc - loc;
         }
      }
      double m = Global::clock();
      for(int date = 0; date < dateN; date++) {
         for(int loc = 0; loc < locN; loc++) {
            value += cache0[loc][date];
         }
      }
      double e = Global::clock();
      writeTiming("Vector", e-m, m-s, value);
   }
   {
      float value = 0;
      double s = Global::clock();
      for(int date = 0; date < dateN; date++) {
         for(int loc = 0; loc < locN; loc++) {
            int index = loc * dateN + date;
            cache1[index] = loc*loc - loc;
         }
      }
      double m = Global::clock();
      for(int date = 0; date < dateN; date++) {
         for(int loc = 0; loc < locN; loc++) {
            int index = loc * dateN + date;
            value += cache1[index];
         }
      }
      double e = Global::clock();
      writeTiming("Map   ", e-m, m-s, value);
   }
}
