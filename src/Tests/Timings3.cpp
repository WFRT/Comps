#include "../Global.h"
#include "../Loggers/Logger.h"
#include <iomanip>
void writeTiming(std::string name, float t0, float t1, float value) {
   std::cout << name << " " << std::fixed << std::setprecision(5) << t0 << " " << t1 << " " << value << std::endl;
}
class Key {
   public:
      Key(int X, int Y) : x(X), y(Y) {};
      bool operator<(const Key &right) const {
         if(x == right.x) {
            return y < right.y;
         }
         return x < right.x;
      };
      int x;
      int y;
};
// Is map<int, float> faster than map<tuple, float> 
int main(void) {
   std::map<Key, float> cache0; // x, y, value
   std::map<int, float> cache1; // id, value

   int xN = 1000;
   int yN = 1000;

   {
      float value = 0;
      double s = Global::clock();
      for(int x = 0; x < xN; x++) {
         for(int y = 0; y < yN; y++) {
            Key key(x, y);
            cache0[key] = x * x - y;
         }
      }
      double m = Global::clock();
      for(int x = 0; x < xN; x++) {
         for(int y = 0; y < yN; y++) {
            Key key(x, y);
            value += cache0[key];
         }
      }
      double e = Global::clock();
      writeTiming("Key", e-m, m-s, value);
   }
   {
      float value = 0;
      double s = Global::clock();
      for(int x = 0; x < xN; x++) {
         for(int y = 0; y < yN; y++) {
            int index = y + x*yN;
            cache1[index] = x*x - y;
         }
      }
      double m = Global::clock();
      for(int x = 0; x < xN; x++) {
         for(int y = 0; y < yN; y++) {
            int index = y + x*yN;
            value += cache1[index];
         }
      }
      double e = Global::clock();
      writeTiming("int", e-m, m-s, value);
   }
}
