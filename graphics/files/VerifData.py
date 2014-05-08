import numpy as np
from matplotlib.dates import YearLocator, MonthLocator, DateFormatter, DayLocator, HourLocator, WeekdayLocator
from matplotlib.ticker import ScalarFormatter
import matplotlib.pyplot as mpl
import sys
import Common
# Wrapper on file to only return a subset of the data
class Data:
   def __init__(self, file, offsets=None, locations=None, dates=None, clim=None, by="offset"):
      self.file = file
      self.offsets = offsets
      self.locations = locations
      self.clim = clim
      self.by = by
      if(self.by != "offset" and self.by != "date" and self.by != "location" and self.by != "threshold"):
         print "Invalid '-x' option"
         sys.exit(1)

      allDates = self.file.getDates()
      if(dates == None):
         self.dateIndices = range(0,len(allDates))
      else:
         self.dateIndices = np.in1d(allDates, dates)
      if(clim != None):
         climDates = self.clim.getDates()
         self.climIndices = np.in1d(climDates, dates);

   # Return the recommended x-values (if you want to abide by the user-requested dimension)
   def getX(self):
      if(self.by == "offset"):
         return self.getOffsets()
      elif(self.by == "date"):
         return Common.convertDates(self.getDates())
      elif(self.by == "location"):
         return self.getLocations()
         return np.array(range(0, len(self.getLocations())))
      else:
         print "Invalid 'by' option in Data"
         sys.exit(1)
   def getXHeader(self):
      if(self.by == "offset"):
         return "Offset (h)"
      elif(self.by == "date"):
         return "Date"
      elif(self.by == "location"):
         return "%6s %5s %5s" % ("id", "lat", "lon")
      else:
         print "Invalid 'by' option in Data"
         sys.exit(1)

   # Get human readable x-values
   def getXHuman(self):
      if(self.by == "offset"):
         return self.getOffsets()
      elif(self.by == "date"):
         return self.getDates()
      elif(self.by == "location"):
         lats = self.getLats()
         lons = self.getLons()
         ids  = self.getLocations()
         x = list()
         for i in range(0, len(ids)):
            x.append("%6d %5.2f %5.2f" % (ids[i], lats[i], lons[i]))
         return x
      else:
         print "Invalid 'by' option in Data"
         sys.exit(1)

   # Return the recommended y-values (if you want to abide by the user-requested dimension)
   def getY(self, metric):
      values = self.getScores(metric)
      mvalues = np.ma.masked_array(values,np.isnan(values))
      mvalues.set_fill_value(np.nan)
         
      N = mvalues.count()
      if(self.by == "offset"):
         N = np.ma.sum(mvalues.count(axis=2), axis=0)
         r = np.ma.sum(np.ma.sum(mvalues,axis=2), axis=0)/N
      elif(self.by == "date"):
         N = np.ma.sum(mvalues.count(axis=2), axis=1)
         r = np.ma.sum(np.ma.sum(mvalues,axis=2), axis=1)/N
      elif(self.by == "location"):
         N = np.ma.sum(mvalues.count(axis=1), axis=0)
         r = np.ma.sum(np.ma.sum(mvalues,axis=1), axis=0)/N

      return np.ma.filled(r, np.nan)


   def getXFormatter(self):
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

   def getLength(self):
      if(self.by == "offset"):
         return len(self.getOffsets())
      elif(self.by == "date"):
         return len(self.getDates())
      elif(self.by == "location"):
         return len(self.getLocations())
      else:
         return 1

   def getDates(self):
      dates = self.file.getDates()
      return dates[self.dateIndices]

   def getLocations(self):
      locations = self.file.getLocations()
      if(self.locations != None):
         locations = locations[self.locations]
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
      if(self.clim != None and (metrics == "fcst" or metrics == "obs")):
         clim = self.clim.getScores("fcst")
         data = data - clim[self.climIndices,:,:]
      if(self.locations is not None):
         data = data[:,:,self.locations]
      if(self.offsets is not None):
         data = data[:,self.offsets,:]

      return data

   def getClimScores(self, metrics):
      if(self.clim == None):
         Common.error("Climatology file not specified");
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
      if(self.offsets == None):
         return self.file.getOffsets()
      else:
         return self.offsets

   def getFilename(self):
      return self.file.getFilename()

   def getUnits(self):
      return self.file.getUnits()

   def getPvar(self, threshold):
      minus = ""
      if(threshold < 0):
         # Negative thresholds
         minus = "m"
      if(abs(threshold - int(threshold)) > 0.01):
         var = "p" + minus + str(abs(threshold)).replace(".", "")
      else:
         var   = "p" + minus + str(int(abs(threshold)))
      return var
