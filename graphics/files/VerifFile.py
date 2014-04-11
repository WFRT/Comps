import numpy as np
import datetime
from copy import deepcopy
from scipy.io import netcdf
import matplotlib.pyplot 
import os
import sys
class File:
   def __init__(self, filename):
      self.checkFile(filename)
      self.filename = filename
      file = netcdf.netcdf_file(filename, 'r')
      self.file = file

   def getFilename(self):
      return self.filename

   # Check if file exists
   def checkFile(self, filename):
      if(not os.path.exists(filename)):
         print "ERROR: File " + filename + " does not exist"
         sys.exit(1)

   def getUnits(self):
      if(self.file.Units == "%"):
         return "%"
      else:
         return "$" + self.file.Units + "$"

   def clean(self, data):
      data = data[:].astype(float)
      q = deepcopy(data)
      mask = np.where(q == -999);
      q[mask] = None
      mask = np.where(q < -100000);
      q[mask] = None
      mask = np.where(q > 1e30);
      q[mask] = None
      return q

   def getDates(self):
      dates = self.clean(self.file.variables["Date"])
      return dates

   def hasScore(self, metric):
      return metric in self.file.variables

   def getScores(self, metric):
      if(not metric in self.file.variables):
         print "Error: Variable '" + metric + "' does not exist in " + self.filename
         sys.exit(1)
      data = self.file.variables[metric]
      data = self.clean(data)
      return data

   def getOffsets(self):
      offsets = self.file.variables["Offset"]
      offsets = self.clean(offsets)
      return offsets

   def getMetrics(self):
      metrics = list()
      for (metric, v) in self.file.variables.iteritems():
         if(metric != "Lat" and metric != "Lon" and metric != "Offset" and metric != "Location" and metric != "Date"):
            metrics.append(metric)
      return metrics
