from scipy.io import netcdf
import Common

class File:
   def __init__(self, filename):
      self._file = netcdf.netcdf_file(filename, 'r')
   def getDates(self):
      return getScore("Date")
   def getOffsets(self):
      return getScore("Offset")
   def getLocations(self):
      return getScore("Location")
   def getLat(self):
      return getScore("Lat")
   def getLon(self):
      return getScore("Lon")
   def getElev(self):
      return getScore("Elev")
   def getScore(self, metric):
      return Common.clean(self._file.variables[metric])

