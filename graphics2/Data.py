from scipy.io import netcdf
import numpy as np
import Common
import re
import sys
from matplotlib.dates  import *
from matplotlib.ticker import ScalarFormatter
class Data:
   def __init__(self, filenames, dates=None, offsets=None, locations=None, clim=None,
         climType="subtract", training=None):
      if(not isinstance(filenames, list)):
         filenames = [filenames]
      self._axis = "date"
      self._index = 0

      # Organize files
      self._files = list()
      self._cache  = list()
      self._clim = None
      for filename in filenames:
         file = netcdf.netcdf_file(filename, 'r')
         self._files.append(file)
         self._cache.append(dict())
      if(clim != None):
         self._clim = netcdf.netcdf_file(clim, 'r')
         self._cache.append(dict())
         if(not (climType == "subtract" or climType == "divide")):
            Common.error("Data: climType must be 'subtract' or 'divide")
         self._climType = climType

      # Climatology file
         self._files = self._files + [self._clim]

      # Find common indicies
      self._datesI     = Data._getCommonIndices(self._files, "Date", dates)
      self._offsetsI   = Data._getCommonIndices(self._files, "Offset", offsets)
      self._locationsI = Data._getCommonIndices(self._files, "Location", locations)

      # Training
      if(training != None):
         for f in range(0, len(self._datesI)):
            if(len(self._datesI[f]) <= training):
               Common.error("Training period too long for " + self.getFilenames()[f] + \
                     ". Max training period is " + str(len(self._datesI[f])-1) + ".")
            self._datesI[f] = self._datesI[f][training:]

      self._findex = 0

   # Find indicies of elements that are present in all files
   # Merge in values in 'aux' as well
   @staticmethod
   def _getCommonIndices(files, name, aux=None):
      values = aux
      for file in files:
         temp = Common.clean(file.variables[name])
         if(values == None):
            values = temp
         else:
            values = np.intersect1d(values, temp)
      indices = list()
      for file in files:
         temp = Common.clean(file.variables[name])
         I = np.where(np.in1d(temp, values))[0]
         indices.append(I)
      return indices

   def _getFiles(self):
      if(self._clim == None):
         return self._files
      else:
         return self._files[0:-1]

   def getMetrics(self):
      metrics = None
      for f in self._files:
         currMetrics = set()
         for (metric, v) in f.variables.iteritems():
            if(not metric in ["Date", "Offset", "Location", "Lat", "Lon", "Elev"]):
               currMetrics.add(metric)
         if(metrics == None):
            metrics = currMetrics
         else:
            metrics = metrics & set(currMetrics)

      return metrics

   def _getIndices(self, axis, findex=None):
      if(axis == "date"):
         I = self._getDateIndices(findex)
      elif(axis == "offset"):
         I = self._getOffsetIndices(findex)
      elif(axis == "location"):
         I = self._getLocationIndices(findex)
      else:
         Common.error(axis)
      return I
   def _getDateIndices(self, findex=None):
      if(findex == None):
         findex = self._findex
      return self._datesI[findex]

   def _getOffsetIndices(self, findex=None):
      if(findex == None):
         findex = self._findex
      return self._offsetsI[findex]

   def _getLocationIndices(self, findex=None):
      if(findex == None):
         findex = self._findex
      return self._locationsI[findex]

   def _getScore(self, metric, findex=None):
      if(findex == None):
         findex = self._findex

      if(not metric in self._cache[findex]):
         temp = self._files[findex].variables[metric]
         dims = temp.dimensions
         temp = Common.clean(temp)
         for i in range(0, len(dims)):
            I = self._getIndices(dims[i].lower(), findex)
            if(i == 0):
               temp = temp[I,Ellipsis]
            if(i == 1):
               temp = temp[:,I,Ellipsis]
            if(i == 2):
               temp = temp[:,:,I,Ellipsis]
         self._cache[findex][metric] = temp

      return self._cache[findex][metric]

   # Returns flattened arrays along the set axis/index
   def getScores(self, metrics):
      if(not isinstance(metrics, list)):
         metrics = [metrics]
      data = dict()
      valid = None
      axis = self._getAxisIndex(self._axis)
      
      # Compute climatology, if needed
      doClim = self._clim != None and ("obs" in metrics or "fcst" in metrics)
      if(doClim):
         temp = self._getScore("fcst", len(self._files)-1)
         if(axis == 0):
            clim = temp[self._index,:,:].flatten()
         elif(axis == 1):
            clim = temp[:,self._index,:].flatten()
         elif(axis == 2):
            clim = temp[:,:,self._index].flatten()
         else:
            clim = temp.flatten()
      else:
         clim = 0

      for i in range(0, len(metrics)):
         metric = metrics[i]
         temp = self._getScore(metric)

         if(axis == 0):
            data[metric] = temp[self._index,:,:].flatten()
         elif(axis == 1):
            data[metric] = temp[:,self._index,:].flatten()
         elif(axis == 2):
            data[metric] = temp[:,:,self._index].flatten()
         else:
            data[metric] = temp.flatten()

         # Subtract climatology
         if(doClim and metric == "fcst"):
            if(self._climType == "subtract"):
               data[metric] = data[metric] - clim
            else:
               data[metric] = data[metric] / clim

         # Remove missing values
         currValid = (np.isnan(data[metric]) == 0) & (np.isinf(data[metric]) == 0)
         if(valid == None):
            valid = currValid
         else:
            valid = (valid & currValid)
      I = np.where(valid)

      q = list()
      for i in range(0, len(metrics)):
         q.append(data[metrics[i]][I])
      return q
   def setAxis(self, axis):
      self._index = 0 # Reset index
      self._axis = axis
   def setIndex(self, index):
      self._index = index
   def setFileIndex(self, index):
      self._findex = index
   def getNumFiles(self):
      return len(self._files) - (self._clim != None)

   def getUnits(self):
      try:
         if(self._files[0].Units == "%"):
            return "%"
         else:
            return "$" + self._files[0].Units + "$"
      except:
         return "No units"

   def isLocationAxis(self, axis):
      if(axis == None):
         return False
      prog = re.compile("location.*")
      return prog.match(axis)

   # What values represent this axis?
   def getAxisValues(self, axis=None):
      if(axis == None):
         axis = self._axis
      if(axis == "date"):
         return Common.convertDates(self._getScore("Date").astype(int))
      elif(axis == "offset"):
         return self._getScore("Offset").astype(int)
      if(self.isLocationAxis(axis)):
         if(axis == "location"):
            data = range(0, len(self._getScore("Location")))
         elif(axis == "locationId"):
            data = self._getScore("Location").astype(int)
         elif(axis == "locationElev"):
            data = self._getScore("Elev")
         elif(axis == "locationLat"):
            data = self._getScore("Lat")
         elif(axis == "locationLon"):
            data = self._getScore("Lon")
         else:
            Common.error("Data.getAxisValues has a bad axis name: " + axis)
         return data
      else:
         return [0]

   def getAxisFormatter(self, axis=None):
      if(axis == None):
         axis = self._axis
      if(axis == "date"):
         return DateFormatter('\n%Y-%m-%d')
      else:
         return ScalarFormatter()


   # filename including path
   def getFullFilenames(self):
      names = list()
      files = self._getFiles()
      for i in range(0, len(files)):
         names.append(files[i].filename)
      return names
   def getFilename(self, findex=None):
      if(findex == None):
         findex = self._findex
      return getFilenames()[findex]

   # Do not include the path
   def getFilenames(self):
      names = self.getFullFilenames()
      for i in range(0, len(names)):
         I = names[i].rfind('/')
         names[i] = names[i][I+1:]
      return names
   def getAxis(self, axis=None):
      if(axis == None):
         axis = self._axis
      return axis

   def getVariable(self):
      return self._files[0].Variable

   def getVariableAndUnits(self):
      return self.getVariable() + " (" + self.getUnits() + ")"

   def getAxisLabel(self, axis=None):
      if(axis == None):
         axis = self._axis
      if(axis == "date"):
         return "Date"
      elif(axis == "offset"):
         return "Offset (h)"
      elif(axis == "locationElev"):
         return "Elevation (m)"
      elif(axis == "locationLat"):
         return "Latitude ($^o$)"
      elif(axis == "locationLon"):
         return "Longitude ($^o$)"


   def getAxisDescriptions(self, axis=None):
      if(axis == None):
         axis = self._axis
      prog = re.compile("location.*")
      if(prog.match(axis)):
         descs = list()
         ids = self._getScore("Location")
         lats = self._getScore("Lat")
         lons = self._getScore("Lon")
         elevs = self._getScore("Elev")
         for i in range(0, len(ids)):
            string = "%6d %5.2f %5.2f %5.0f" % (ids[i],lats[i], lons[i], elevs[i])
            descs.append(string)
         return descs
      else:
         return self.getAxisValues(axis)

   def getAxisDescriptionHeader(self, axis=None):
      if(axis == None):
         axis = self._axis
      prog = re.compile("location.*")
      if(prog.match(axis)):
         return "%6s %5s %5s %5s" % ("id", "lat", "lon", "elev")
      else:
         return axis

   def _getAxisIndex(self, axis):
      if(axis == "date"):
         return 0
      elif(axis == "offset"):
         return 1
      elif(axis == "location" or axis == "locationId" or axis == "locationElev" or axis == "locationLat" or axis == "locationLon"):
         return 2
      else:
         return None
