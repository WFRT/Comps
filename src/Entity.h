#ifndef ENTITY_H
#define ENTITY_H
#include <string>
class Location;

// Abstract class containing date, init, offset, location, and variable
class Entity {
   public:
      Entity() {};
      virtual int  getDate() const = 0;
      virtual int  getInit() const = 0;
      virtual float getOffset() const = 0;
      virtual Location getLocation() const = 0;
      virtual std::string getVariable() const = 0;

      /*
   protected:
      void setDate(int iDate) const;
      void setInit(int iInit) const;
      void setOffset(float iOffset) const;
      void setLocation(Location iLocation) const;
      void setVariable(std::string iVariable) const;

   private:
      std::string mVariable;
      int mDate;
      float mOffset;
      int mInit;
      Location mLocation;
  */
};
#endif

