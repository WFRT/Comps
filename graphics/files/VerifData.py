import numpy as np
from matplotlib.dates import YearLocator, MonthLocator, DateFormatter, DayLocator, HourLocator, WeekdayLocator
from matplotlib.ticker import ScalarFormatter
import matplotlib.pyplot as mpl
import sys
# Wrapper on file to only return a subset of the data
class Data:
   def __init__(self, file, offset=None, location=None, dates=None, by="offset"):
      self.file = file
      self.offset = offset
      self.location = location
      self.by = by
      if(self.by != "offset" and self.by != "date" and self.by != "location"):
         print "Invalid '-x' option"
         sys.exit(1)

      allDates = self.file.getDates()
      if(dates == None):
         self.dateIndices = range(0,len(allDates))
      else:
         self.dateIndices = np.in1d(allDates, dates)

   # Return the recommended x-values (if you want to abide by the user-requested dimension)
   def getX(self):
      if(self.by == "offset"):
         return self.getOffsets()
      elif(self.by == "date"):
         return self.getDates()
      elif(self.by == "location"):
         return np.array(range(0, len(self.getLocations())))
      else:
         print "Invalid 'by' option in Data"
         sys.exit(1)

   # Return the recommended y-values (if you want to abide by the user-requested dimension)
   def getY(self, metric):
      values = self.getScores(metric)
      mvalues = np.ma.masked_array(values,np.isnan(values))
      N = mvalues.count()
      if(self.by == "offset"):
         N = np.sum(mvalues.count(axis=2), axis=0)
         return np.sum(np.sum(mvalues,axis=2), axis=0)/N
      elif(self.by == "date"):
         N = np.sum(mvalues.count(axis=2), axis=1)
         return np.sum(np.sum(mvalues,axis=2), axis=1)/N
      elif(self.by == "location"):
         N = np.sum(mvalues.count(axis=1), axis=0)
         return np.sum(np.sum(mvalues,axis=1), axis=0)/N

   def getXFormatter(self, metric):
      if(self.by == "date"):
         return DateFormatter('\n%Y-%m-%d')
      else:
         return ScalarFormatter()

   def getXLabel(self):
      if(self.by == "offset"):
         return "Offset (h)"
      else:
         return self.by.capitalize()

   def getByAxis(self):
      if(self.by == "offset"):
         return 1
      elif(self.by == "date"):
         return 0
      elif(self.by == "location"):
         return 2

   def getDates(self):
      dates = self.file.getDates()
      return dates[self.dateIndices]

   def getLocations(self):
      locations = self.file.getLocations()
      return locations

   def getLats(self):
      lats = self.file.getLats()
      return lats

   def getLons(self):
      lons = self.file.getLons()
      return lons

   def hasScore(self, metric):
      return self.file.hasScore(metric)

   def getUnitsString(self):
      units = self.file.getUnits()
      if(units == ""):
         return ""
      return "(" + units + ")"

   def getScores(self, metrics):
      data = self.file.getScores(metrics)
      data = data[self.dateIndices,:,:]
      if(self.location is not None and self.offset is not None):
         data = data[:,self.offset,self.location, None]
      elif(self.location is not None):
         data = data[:,:,self.location, None]
      elif(self.offset is not None):
         data = data[:,self.offset,:, None]
      return data

   def getFlatScores(self, metrics):
      if(not isinstance(metrics, list)):
         data = self.getScores(metrics)
         data = data.flatten()
         mask = np.where(np.isnan(data) == 0)
         data = data[mask]
         return data
      else:
         data = list()
         curr = self.getScores(metrics[0]).flatten()
         data.append(curr)
         I = np.where(np.isnan(data[0]) == 0)[0]
         for i in range(1, len(metrics)):
            curr = self.getScores(metrics[i]).flatten()
            data.append(curr)
            Icurr = np.where(np.isnan(curr) == 0)[0]
            I    = np.intersect1d(I, Icurr)
         for i in range(0, len(metrics)):
            data[i] = data[i][I]
         return data

   def getOffsets(self):
      #if(self.offset is not None):
      return self.file.getOffsets()

   def getFilename(self):
      return self.file.getFilename()

   def getUnits(self):
      return self.file.getUnits()
