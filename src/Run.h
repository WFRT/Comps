#ifndef RUN_H
#define RUN_H

class Run {
   public:
      Run(std::string iName);
      int getDebugLevel() const;
   private:
      std::string mName;
};
#endif

