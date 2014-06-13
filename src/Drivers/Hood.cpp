#include <iostream>
#include <iomanip>
#include "../Global.h"
#include "../Options.h"
#include "../Data.h"
#include "../Scheme.h"
#include "../Inputs/Input.h"
#include "../Loggers/Logger.h"
#include "../Loggers/Default.h"
#include "../Location.h"
#include "../Member.h"
#include "../Neighbourhoods/Neighbourhood.h"
#ifdef WITH_PYTHON
#include <cstring>
#include <Python.h>

std::string getLocationString(const Location& iLocation);
float getFloat(std::string iString);
void plot(const std::vector<Location>& iLocations, std::string iOptions);
void plot(const std::vector<float>& x, const std::vector<float>& y, std::string options);
void plotElev(const std::vector<Location>& iLocations, std::string iOptions="");
std::string toPyArray(const std::vector<float>& iValues);
void draw(const Location& iLocation, std::string iInputTag, std::string iHoodTag);

int main(int argc, const char *argv[]) {
   Global::setLogger(new LoggerDefault(Logger::message));

   if(argc != 5 && argc != 6 && argc != 8) {
      std::cout << "Draws the neighbourhood around a point" << std::endl;
      std::cout << std::endl;
      std::cout << "usage: neighbourhood.exe <scheme> <input> <lat> <lon> [<elev> [<lonGrad> <latGrad>]]" << std::endl;
      std::cout << std::endl;
      std::cout << "Arguments:" << std::endl;
      std::cout << "   scheme         The neighbourhood scheme (from namelist) used to create the neighbourhood" << std::endl;
      std::cout << "   input          An input scheme (from namelist) to create neighbourhood from" << std::endl;
      std::cout << "   lat            Latitude (degrees) of the point" << std::endl;
      std::cout << "   lon            Longitude (degrees) ofthe  point" << std::endl;
      std::cout << "   elev           Elevation (m) at the point" << std::endl;
      std::cout << "   lonGrad        West to east height gradient (m/m) at the point" << std::endl;
      std::cout << "   latGrad        South to north heigh gradient (m/m) at the point" << std::endl;

      return 0;
   }

   std::string hoodTag(argv[1]);
   std::string inputTag(argv[2]);
   float lat  = getFloat(argv[3]);
   float lon  = getFloat(argv[4]);
   float elev = Global::MV;
   float gradX = Global::MV;
   float gradY = Global::MV;
   if(argc > 5) {
      elev = getFloat(argv[5]);
      if(argc > 6) {
         gradX = getFloat(argv[6]);
         gradY = getFloat(argv[7]);
      }
   }
   Location loc("", 0, lat, lon);
   loc.setElev(elev);
   loc.setGradientX(gradX);
   loc.setGradientY(gradY);

   // Start python
   Py_Initialize();
   PyRun_SimpleString("import pylab as mpl");
   PyRun_SimpleString("import numpy as np");
   PyRun_SimpleString("from scipy.io import netcdf");
   PyRun_SimpleString("from matplotlib.mlab import griddata");
   PyRun_SimpleString("from copy import deepcopy");
   PyRun_SimpleString("import scipy.ndimage");
   PyRun_SimpleString("from matplotlib.widgets import Button");

   std::cout << "Latitude: " << lat << " Longitude: " << lon;
   if(Global::isValid(elev))
      std::cout << " Elevation: " << elev;
   if(Global::isValid(gradX))
      std::cout << " W-E gradient: " << gradX;
   if(Global::isValid(gradY))
      std::cout << " S-N gradient: " << gradY;
   std::cout << std::endl;
   draw(loc, inputTag, hoodTag);
   while(0) {
      std::string line;
      std::string first;
      std::getline(std::cin, line);
      std::stringstream ss(line);
      ss >> first;
      if(first == "q") {
         break;
      }
      lat = getFloat(first);
      std::string lonString;
      std::string elevString;
      ss >> lonString >> elevString;
      lon = getFloat(lonString);
      elev = getFloat(elevString);

      loc.setLat(lat);
      loc.setLon(lon);
      loc.setElev(elev);
      draw(loc, inputTag, hoodTag);
   }
}

std::string getLocationString(const Location& iLocation) {
   std::stringstream ss;
   ss << iLocation.getLat() << " " << iLocation.getLon() << " " << iLocation.getElev(); 
   return ss.str();
}

float getFloat(std::string iString) {
   std::stringstream ss;
   ss << iString;
   float value;
   ss >> value;
   return value;
}

void plot(const std::vector<Location>& iLocations, std::string iOptions) {
   std::vector<float> x(iLocations.size(), 0);
   std::vector<float> y(iLocations.size(), 0);
   for(int i = 0; i < iLocations.size(); i++) {
      x[i] = iLocations[i].getLon();
      y[i] = iLocations[i].getLat();
   }
   plot(x, y, iOptions);
}
void plot(const std::vector<float>& x, const std::vector<float>& y, std::string options) {
   std::stringstream ss;
   ss << "mpl.plot(";
   ss << toPyArray(x);
   ss << ", ";
   ss << toPyArray(y);
   ss << ", " << options << ")" << std::endl;
   PyRun_SimpleString(ss.str().c_str());
}
void plotElev(const std::vector<Location>& iLocations, std::string iOptions) {
   std::vector<float> x(iLocations.size(), 0);
   std::vector<float> y(iLocations.size(), 0);
   std::vector<float> z(iLocations.size(), 0);
   for(int i = 0; i < iLocations.size(); i++) {
      x[i] = iLocations[i].getLon();
      y[i] = iLocations[i].getLat();
      z[i] = iLocations[i].getElev();
   }
   std::stringstream ss;
   ss << "lons = " << toPyArray(x) << std::endl;
   ss << "lats = " << toPyArray(y) << std::endl;
   ss << "elevs = " << toPyArray(z) << std::endl;

   ss << "N = np.ceil(np.sqrt(len(lons)))" << std::endl;
   ss << "xi = np.linspace(min(lons), max(lons),N*3)" << std::endl;
   ss << "yi = np.linspace(min(lats), max(lats),N*3)" << std::endl;
   ss << "elevsi = griddata(lons, lats, elevs, xi, yi)" << std::endl;
   ss << "mpl.contourf(xi, yi, elevsi, range(0,2000,100), cmap=mpl.cm.Greys)" << std::endl;
   ss << "mpl.contour(xi, yi, elevsi, range(0,2000,100), colors='black')" << std::endl;
   ss << "mpl.contour(xi, yi, elevsi, range(0,2000,500), linewidths=2, colors='black')" << std::endl;
   ss << "mpl.gca().grid(color='g', linestyle='-', linewidth=1)" << std::endl;
   ss << "mpl.subplots_adjust(bottom=0, left=0, right=1, top=1, hspace=0)" << std::endl;
   ss << "for tick in mpl.gca().yaxis.get_major_ticks():" << std::endl;
   ss << "   tick.set_pad(-30)" << std::endl;
   ss << "   tick.label2.set_horizontalalignment('right')" << std::endl;
   ss << "for tick in mpl.gca().xaxis.get_major_ticks(): " << std::endl;
   ss << "   tick.set_pad(-30)" << std::endl;
   ss << "   tick.label2.set_verticalalignment('top')" << std::endl;
   PyRun_SimpleString(ss.str().c_str());
}

std::string toPyArray(const std::vector<float>& iValues) {
   std::stringstream ss;
   ss << "[";
   for(int i = 0; i < iValues.size(); i++) {
      ss << iValues[i];
      if(i < iValues.size()-1)
         ss << ",";
   }
   ss << "]";
   return ss.str();

}

void draw(const Location& iLocation, std::string iInputTag, std::string iHoodTag) {
   Input* input        = Input::getScheme(iInputTag);
   Neighbourhood* hood = Neighbourhood::getScheme(iHoodTag);

   std::vector<Location> iLocationVector(1, iLocation);

   assert(input != NULL);
   assert(hood != NULL);

   // Retrieve neighbourhood
   std::vector<Location> locations    = hood->select(input, iLocation);
   std::vector<Location> allLocations = input->getLocations();

   plot(locations, "'ro', ms=10, alpha=0.5");
   PyRun_SimpleString("xlim = mpl.xlim()");
   PyRun_SimpleString("ylim = mpl.ylim()");
   plot(allLocations, "'kx', ms=8, alpha=0.5");
   plot(iLocationVector, "'go', ms=15");
   plotElev(allLocations);
   PyRun_SimpleString("mpl.xlim(xlim)");
   PyRun_SimpleString("mpl.ylim(ylim)");
   PyRun_SimpleString("mpl.show()");
   delete input;
   delete hood;
}
#else
#error Cannot compile hood.exe without libpython2.7
#endif
