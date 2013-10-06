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
};
#endif
