#include "Flat.h"
#include "../Data.h"
#include "../Parameters.h"
#include "../Configurations/Configuration.h"

ParameterIoFlat::ParameterIoFlat(const Options& iOptions, const Data& iData) : ParameterIo(iOptions, iData) {

}
bool ParameterIoFlat::read(Component::Type iType,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string iVariable,
      const Configuration& iConfiguration,
      Parameters& iParameters) const {
   std::string filename = getFilename(iType, iDate, iInit, iOffset, iLocation, iVariable, iConfiguration);
   std::ifstream file(filename.c_str());
   if(file.is_open()) {
      std::vector<float> parameters;
      while(file.good()) {
         float value;
         file >> value;
         if(file.good()) {
            //std::cout << "Parameters " << value << std::endl;
            parameters.push_back(value);
         }
      }
      iParameters.setAllParameters(parameters);
      return true;
   }
   else {
      return false;
   }
}
void ParameterIoFlat::writeCore() {
   /*
   std::string filename = getFilename(iType, iDate, iInit, iOffset, iLocation, iVariable, iConfiguration);
   std::ofstream file(filename.c_str());
   if(file.is_open()) {
      std::vector<float> parameters = iParameters.getAllParameters();
      for(int i = 0; i < (int) parameters.size(); i++) {
         file << parameters[i] << " ";
      }
   }
   */
}

std::string ParameterIoFlat::getFilename(Component::Type iType,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string iVariable,
      const Configuration& iConfiguration) const {
   std::stringstream ss;
   ss << mBaseOutputDirectory << mRunDirectory << "/parameters/" << iDate << "_" << iOffset << "_" << iLocation.getId() << "_" << Component::getComponentName(iType) << "_" << iConfiguration.getName() << "_" << iVariable << ".dat";
   return ss.str();
}
