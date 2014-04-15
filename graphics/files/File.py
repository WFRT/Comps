import numpy as np
import datetime
from copy import deepcopy
from scipy.io import netcdf
import matplotlib.pyplot 
import os
import Common
import sys

def convertDates(dates):
   for i in range(0,len(dates.flat)):
      #if(~np.isnan(dates.flat[i])):
      if(~np.isnan(dates.flat[i])):
         year  = int(dates.flat[i] / 10000)
         month = int(dates.flat[i] / 100 % 100)
         day   = int(dates.flat[i] % 100)
         dates.flat[i] = matplotlib.dates.date2num(datetime.datetime(year, month, day, 0))
   return dates


class File:
   def __init__(self):
      self.timeZone = 0

   def getOffsets(self):
      offsets = list(self.getOffsetsCore())
      if(not self.timeZone == 0):
         for i in range(0,len(offsets)):
            offsets[i] = offsets[i] + self.timeZone/24.0
      return offsets

   def error(self, message):
      Common.error(message)

   def warning(self, message):
      Common.warning(message)

   # Use positive for East timezones
   def setTimeZone(self, timeZone):
      self.timeZone = timeZone

   def getTimeZone(self):
      return self.timeZone
   
   # Check if file exists
   def checkFile(self, filename):
      if(not os.path.exists(filename)):
         self.error("File " + filename + " does not exist")

class NetCdfFile(File):
   @staticmethod
   def getIndex(filename, locationId):
      f = netcdf.netcdf_file(filename, 'r')
      locationIds = f.variables['Location']
      locationIds = locationIds[:].astype(int).tolist()
      return locationIds.index(locationId)

   def __init__(self, filename, location):
      File.__init__(self)
      self.checkFile(filename)
      self.s = int(location)
      self.filename = filename
      self.timeZone = 0
      self._parseFile(filename)

   def setTimeZone(self, timeZone):
      self.timeZone = timeZone

   def setTimeZone(self, timeZone):
      self.timeZone = timeZone

   def getObs(self):
      return {'values':self.obs[:,self.s], 'offsets':self.getOffsets()}

   def getEnsemble(self):
      return {'values':self.ens[:,:,self.s], 'offsets':self.getOffsets()}

   def getCdfs(self):
      return {'values':self.cdfInv[:,:,self.s], 'offsets':self.getOffsets(),
            'cdfs':self.probs}

   def getLowerDiscrete(self):
      return {'values':self.p0[:,self.s], 'offsets':self.getOffsets()}

   def getOffsetsCore(self):
      return self.o

   def getLocation(self):
      lat = self.clean(self.f.variables['Lat'])[self.s]
      lon = self.clean(self.f.variables['Lon'])[self.s]
      return {'id':self.locationIds[self.s], 'lat':lat, 'lon':lon}

   def getVariable(self):
      return {'name':self.variableName, 'units':self.units}

   def getDeterministic(self):
      return {'values':self.dets[:,self.s], 'offsets':self.getOffsets()}

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

   def _parseFile(self,filename):
      f = netcdf.netcdf_file(filename, 'r')
      self.f = f
      self.dets     = self.clean(f.variables['Det'])
      self.ens      = self.clean(f.variables['Ens'])
      self.obs      = self.clean(f.variables['Observations'])
      if(f.variables.has_key('P0')):
         self.p0       = self.clean(f.variables['P0'])
      self.cdfInv   = self.clean(f.variables['CdfInv'])
      if(f.variables.has_key('Threshold')):
         self.cdfX     = self.clean(f.variables['Threshold'])
      self.probs     = self.clean(f.variables['Prob'])
      self.variableName = f.Variable
      if(f.Units == "%"):
         self.units = "%"
      else:
         self.units = "$" + f.Units + "$"
      self.startDate = f.Date
      self.o0       = self.clean(f.variables['Offset'])
      self._convertOffsets()
      self.locationIds = self.clean(f.variables['Location'])

   def _convertOffsets(self):
      date0 = self._getDatetime(self.startDate)
      date1 = datetime.datetime(2012,1,8,18)
      self.o = np.zeros(len(self.o0))
      for i in range(0,len(self.o0)):
         # Adjust based on the initialization hour
         init    = 0
         if(hasattr(self.f, 'Init')):
            init = self.f.Init
         hour = self.o0[i] + init
         self.o[i] = matplotlib.dates.date2num(date0 + datetime.timedelta(hours=hour))

   def _getDatetime(self, dateYYYYMMDD):
      yyyy = int(dateYYYYMMDD/10000)
      mm   = int(np.mod(dateYYYYMMDD,10000)/100)
      dd   = np.mod(dateYYYYMMDD,100)
      return datetime.datetime(yyyy,mm,dd)
