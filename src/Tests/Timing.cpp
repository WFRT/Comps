#include "../Key.h"
#include "../Global.h"
#include "../Cache.h"
#include <boost/unordered_map.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/any.hpp>
#include <iomanip>

   size_t hash_value(Key::Input const& p) {
      std::size_t seed = 0;
      boost::hash_combine(seed, p.date);
      boost::hash_combine(seed, p.init);
      boost::hash_combine(seed, p.offset);
      boost::hash_combine(seed, p.location);
      boost::hash_combine(seed, p.member);
      boost::hash_combine(seed, p.variable);
      return seed;
   }
void writeTiming(std::string name, float time0, float time1, float value) {
   std::cout << name << ": " << std::fixed << std::setprecision(5) << time0 << " " << time1 << " " << value << std::endl;
}

int main(int argc, char **argv) {
   //Cache<Key::Input, float> cache0;
   std::map<Key::Three<int,float,int>, std::vector<float> > cache1; // Date offset var
   std::vector<float> cache2;
   std::map<Key::Input, float> cache3;
   std::map<int, std::map<int, std::map<float, std::map<int, std::map<int, std::vector<float> > > > > > cache4;
   std::map<int, std::map<int, std::map<float, std::map<int, std::map<int, std::map<int, float> > > > > > cache5;
   std::map<int, float> cache6;

   //cache0.setName("cache0");

   int dateN = 100;
   int offsetN = 10;
   int varN = 6;
   int locN = 200;

   std::cout << "Total size: " << dateN * offsetN * varN * locN << std::endl;

   cache2.resize(dateN*offsetN*varN*locN);

   // Insert data
   int index = 0;
   for(int date = 0; date < dateN; date++) {
      int init = 0;
      for(int offset = 0; offset < offsetN; offset++) {
         for(int var = 0; var < varN; var++) {
            std::vector<float> data;
            data.resize(locN);
            int member  = 0;
            for(int loc = 0; loc < locN; loc++) {
               data[loc] = loc * loc - loc;
               cache2[index] = data[loc];
               cache6[index] = data[loc];
               index++;
               Key::Input key(date, init, offset, loc, member, var);
               //cache0.add(key, data[loc]);
               cache3[key] = data[loc];
               cache5[date][init][offset][loc][member][var] = data[loc];
            }
            //Key::Three<int,float,int> key(date, offset, var);
            //cache1.add(key, data);
            cache4[date][init][offset][member][var] = data;
         }
      }
   }
   double s0 = 0;
   double e0 = 0;
   if(1) {
      /*
      double s0 = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               int member  = 0;
               for(int loc = 0; loc < locN; loc++) {
                  Key::Input key(date, init, offset, loc, member, var);
                  cache0.add(key, loc*loc - loc);
               }
            }
         }
      }
      double e0 = Global::clock();

      float value = 0;
      double s = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               int member  = 0;
               for(int loc = 0; loc < locN; loc++) {
                  Key::Input key(date, init, offset, loc, member, var);
                  value += cache0.get(key);
               }
            }
         }
      }
      double e = Global::clock();
      writeTiming("CacheInput", e - s, e0 - s0, value);
  */
   }
   {
      double s0 = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               int member  = 0;
               Key::Three<int,float,int> key(date, offset, var);
               cache1[key].resize(locN);
               std::map<Key::Three<int,float,int>, std::vector<float> >::iterator it = cache1.find(key);
               for(int loc = 0; loc < locN; loc++) {
                  //cache1[key][loc]  = loc*loc - loc;
                  (it->second)[loc] = loc*loc - loc;
               }
            }
         }
      }
      double e0 = Global::clock();

      float value = 0;
      double s = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               int member  = 0;
               Key::Three<int,float,int> key(date, offset, var);
               const std::vector<float> values = cache1[key];
               for(int loc = 0; loc < locN; loc++) {
                  value += values[loc];
               }
            }
         }
      }
      double e = Global::clock();
      writeTiming("CacheThree", e - s, e0 - s0, value);
   }
   {
      float value = 0;
      double s = Global::clock();
      int init = 0;
      /*
      for(int i = 0; i < cache2.size(); i++) {
         value += cache2[i];
      }
  */
      for(int date = 0; date < dateN; date++) {
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               for(int loc = 0; loc < locN; loc++) {
                  int member  = 0;
                  int index = loc + var*locN + offset*varN*locN + date*offsetN*varN*locN;
                  value += cache2[index];
               }
            }
         }
      }
      double e = Global::clock();
      writeTiming("Vector    ", e - s, e0 - s0, value);
   }
   if(1) {
      float value = 0;
      double s = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               int member  = 0;
               for(int loc = 0; loc < locN; loc++) {
                  Key::Input key(date, init, offset, loc, member, var);
                  value += cache3[key];
               }
            }
         }
      }
      double e = Global::clock();
      writeTiming("MapInput  ", e - s, e0 - s0, value);
   }
   {
      double s0 = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               //std::vector<float> data;
               //data.resize(locN);
               int member  = 0;
               std::vector<float>* it = &cache4[date][init][offset][member][var];

               for(int loc = 0; loc < locN; loc++) {
                  //data[loc] = loc * loc - loc;
                  //cache4[date][init][offset][member][var][loc] = loc*loc - loc; //= data;
                  (*it)[loc] = loc*loc - loc; //= data;
               }
            }
         }
      }
      double e0 = Global::clock();
      float value = 0;
      double s = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               int member  = 0;
               /*
               for(int loc = 0; loc < locN; loc++) {
                  value += cache4[date][init][offset][loc][member][var];
               }  */
               std::vector<float> data = cache4[date][init][offset][member][var];
               for(int loc = 0; loc < locN; loc++) {
                  value += data[loc];
               }
            }
         }
      }
      double e = Global::clock();
      writeTiming("MapMapVec ", e - s, e0 - s0, value);
   }
   {
      float value = 0;
      double s = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               int member  = 0;
               for(int loc = 0; loc < locN; loc++) {
                  value += cache5[date][init][offset][loc][member][var];
               }
            }
         }
      }
      double e = Global::clock();
      writeTiming("MapMapMap ", e - s, e0 - s0, value);
   }
   {
      float value = 0;
      double s = Global::clock();
      for(int date = 0; date < dateN; date++) {
         int init = 0;
         for(int offset = 0; offset < offsetN; offset++) {
            for(int var = 0; var < varN; var++) {
               int member  = 0;
               for(int loc = 0; loc < locN; loc++) {
                  int index = loc + var*locN + offset*varN*locN + date*offsetN*varN*locN;
                  value += cache6[index];
               }
            }
         }
      }
      double e = Global::clock();
      writeTiming("MapInt    ", e - s, e0 - s0, value);
   }
}
