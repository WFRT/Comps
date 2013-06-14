import numpy as np
import datetime
from copy import deepcopy
from scipy.io import netcdf
import matplotlib.pyplot 
import os
import bchUtil

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
      self.shiftOffsets = 0

   def getOffsets(self):
      offsets = list(self.getOffsetsCore())
      if(not self.shiftOffsets == 0):
         for i in range(0,len(offsets)):
            offsets[i] = offsets[i] + self.shiftOffsets / 24.0
      return offsets

   def setShiftOffsets(self, shift):
      self.shiftOffsets = shift

class NetCdfFile(File):
   def __init__(self, filename, location):
      File.__init__(self)
      self.s = int(location)
      self.filename = filename
      self._parseFile(filename)

   def getObs(self):
      return {'values':self.obs[:,0,self.s], 'offsets':self.getOffsets()}

   def getEnsemble(self):
      return {'values':self.ens[:,:,0,self.s], 'offsets':self.getOffsets()}

   def getCdfs(self):
      return {'values':self.cdfInv[:,:,0,self.s], 'offsets':self.getOffsets(),
            'cdfs':self.cdfs}

   def getLowerDiscrete(self):
      return {'values':self.p0[:,0,self.s], 'offsets':self.getOffsets()}

   def getOffsetsCore(self):
      return self.o

   def getLocation(self):
      return {'id':self.locationIds[self.s]}

   def getVariable(self):
      return {'name':self.variableName, 'units':self.units}

   def getDeterministic(self):
      return {'values':self.dets[:,0,self.s], 'offsets':self.getOffsets()}

   def getDates(self):
      return {'values':self.selectorDates[:,:,0,self.s], 'offsets':self.getOffsets()}

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
      self.cdfX     = self.clean(f.variables['X'])
      self.cdfs     = self.clean(f.variables['Cdfs'])
      self.variableName = f.Variable
      if(f.Units == "%"):
         self.units = "%"
      else:
         self.units = "$" + f.Units + "$"
      self.selectorDates = convertDates(self.clean(f.variables['SelectorDate']))
      self.selectorOffsets = (f.variables['SelectorOffset']);
      self.selectorSkills = self.clean(f.variables['SelectorSkill']);
      self.startDate = f.Date
      self.o0       = self.clean(f.variables['Offset'])
      self._convertOffsets()
      self.locationIds = self.clean(f.variables['Location'])

   def _convertOffsets(self):
      date0 = self._getDatetime(self.startDate)
      date1 = datetime.datetime(2012,1,8,18)
      self.o = np.zeros(len(self.o0))
      for i in range(0,len(self.o0)):
         self.o[i] = matplotlib.dates.date2num(date0 + datetime.timedelta(hours=self.o0[i]) - datetime.timedelta(hours=8))

   def _getDatetime(self, dateYYYYMMDD):
      yyyy = int(dateYYYYMMDD/10000)
      mm   = int(np.mod(dateYYYYMMDD,10000)/100)
      dd   = np.mod(dateYYYYMMDD,100)
      return datetime.datetime(yyyy,mm,dd)

class ClimFile(File):
   def __init__(self, station, variable):
      File.__init__(self)
      self.startDate = 0
      self.endDate   = 0
      self.station   = station
      self.variable  = variable
      self.baseDir     = bchUtil.getClimLocation()
      self.stationCode = bchUtil.getStationCode(station)

   def setDateRange(self, startDate, endDate):
      self.startDate = startDate
      self.endDate   = endDate

   def getOffsetsCore(self):
      year     = int(np.floor(self.startDate / 10000))
      month    = int(np.mod(np.floor(self.startDate / 100),100))
      day      = int(np.mod(self.startDate, 100))
      currdate = datetime.datetime(year, month, day, 0)
      start        = currdate
      end          = currdate + datetime.timedelta(0, 86400*15);
      #print self.startDate
      #print matplotlib.dates.date2num(start)
      return [matplotlib.dates.date2num(start),matplotlib.dates.date2num(end)]

   def getCdfs(self):
      year     = int(np.floor(self.startDate / 10000))
      month    = int(np.mod(np.floor(self.startDate / 100),100))
      day      = int(np.mod(self.startDate, 100))
      currdate = datetime.datetime(year, month, day, 0)


      # Parse date
      start        = currdate;
      start_string = str(start.month) + "/" + str(start.day);
      end          = currdate + datetime.timedelta(0, 86400*15);
      end_string   = str(end.month) + "/" + str(end.day);

      # Read file
      dir_mean = self.baseDir + "data/Filtered_Climo/"
      #dir_std = self.baseDir + "data/Standard_Deviation/Filtered_sd_climo/"
      dir_std  = self.baseDir + "data/Percentiles/Filtered_Climo_Per/"
      
      cdfInvs = np.zeros([2,3], 'float')
      if(os.path.exists(dir_mean + self.stationCode + ".txt")):
         start_mean= os.popen("grep '^" + start_string + "	' " + dir_mean +self.stationCode + ".txt | awk '{print $2,$3,$4}'").read().split()
         end_mean  = os.popen("grep '^" + end_string   + "	' " + dir_mean +self.stationCode + ".txt | awk '{print $2,$3,$4}'").read().split()
         if(self.variable == "TMin24"):
            index = 0;
         elif(self.variable == "TMax24"):
            index = 1;
         elif(self.variable == "Precip24"):
            index = 2;
         clim_mean = [start_mean[index], end_mean[index]];

         if(os.path.exists(dir_std + self.stationCode + ".txt")):
            start_std = os.popen("grep ^'" + start_string + "	' " + dir_std +self.stationCode + ".txt | awk '{print $2,$3,$4,$5,$6,$7}'").read().split()
            end_std   = os.popen("grep ^'" + end_string   + "	' " + dir_std +self.stationCode + ".txt | awk '{print $2,$3,$4,$5,$6,$7}'").read().split()

            for i in range(0, len(start_std)):
               start_std[i] = float(start_std[i]);
               end_std[i]   = float(end_std[i]);
               if(start_std[i] < -900):
                  start_std[i] = np.nan;
               if(end_std[i] < -900):
                  end_std[i] = np.nan;
            clim_below = [float(start_std[index*2]), float( end_std[index*2])];
            clim_above = [float(start_std[index*2+1]), float(end_std[index*2+1])];
         else:
            clim_below = [np.nan,np.nan]
            clim_above = [np.nan,np.nan]

         cdfInvs[:,0] = clim_below
         cdfInvs[:,1] = clim_mean
         cdfInvs[:,2] = clim_above
      else:
         print "No climatology found for " + self.stationCode
         cdfInvs[:] = np.nan
      return {'values':cdfInvs, 'offsets':self.getOffsets(),
            'cdfs':[0.05,0.5,0.95]}

class TextFile(File):
   
   def __init__(self, filename):
      File.__init__(self)
      self._parseFile(filename)

   # Returns an ensemble of values
   def getEnsemble(self):
      return {'values':np.array([(1,2,3),(2,4,5)]), 'offsets':np.array([2,4])}

   # Returns an ensemble of values
   def getObs(self):
      return {'values':np.array([1,2,3]), 'offsets':np.array([2,4,6])}

   # Returns all cdfs
   def getCdfs(self):
      return {'values':np.array([(1,2,3),(2,3,3)]), 'offsets':np.array([2,4]),
            'cdfs':np.array([0.1,0.9])}

   def _parseFile(self, filename):
      
      # Open and read data file
      if(not os.path.isfile(filename)):
         return 1;
      '''

      dataFile = open(filename, "r");
      lines = dataFile.readlines();

      # Loop over all the records
      for line in lines:
         if(line[0] != '\n'):
            # Find current mode
            if(line[0:5] == "# CDF"):
               mode = "cdf"
               counter = 0;
            elif(line[0:4] == "# P0"):
               mode = "p0"
               counter = 0;
               counter2 = 0;
            elif(line[0:5] == "# ENS"):
               mode = "ens"
               counter = 0;
            elif(line[0:5] == "# Var"):
               mode = "var"
               counter = 0;
            elif(line[0:5] == "# Cal"):
               mode = "cal"
               counter = 0;
            elif(line[0:5] == "# Obs"):
               mode = "obs"
               counter = 0
            elif(line[0:5] == "# Ver"):
               mode = "ver"
               counter = 0;
            elif(line[0:5] == "# Sha"):
               mode = "sha"
               counter = 0;
            elif(line[0:5] == "# PIT"):
               mode = "pit"
               counter = 0;

         if(line[0] != '#' and line[0] != '\n'):
            if(mode == "cdf"):
               data = [float(s) for s in line.split()]
               cdfdata_y[counter][0:(len(data)-1)/2] = data[2:len(data):2];
               cdfdata_bins[0:len(data[1:len(data):2]),0] = data[1:len(data):2]
               cdfdata_x[counter] = data[0];
               counter += 1
            if(mode == "p0"):
               data = [float(s) for s in line.split()]
               if(data[0] != pdfdata_x[counter]):
                  counter += 1
                  counter2 = 0
               pdfdata[counter][counter2] = data[1]
               if(counter2 == 0):
                  pdfdata_x[counter] =  data[0]
               if(counter == 0):
                   pdfdata_y[counter2] = 0;
               counter2 += 1
            if(mode == "ens"):
               data = [float(s) for s in line.split()]
               # First set to None, and valid numbers will then overwrite the None values
               ensdata[counter, :] = None
               if(len(data) > 1):
                  ensdata[counter, 0:len(data)-1] = data[1:len(data)]
               ensdata_x[counter] = data[0]
               counter += 1
            if(mode == "obs"):
               data = [float(s) for s in line.split()]
               obsdata_x[counter] = data[0];
               obsdata_y[counter] = data[1];
               counter += 1
            if(mode == "ver"):
               data = [float(s) for s in line.split()]
               verdata_y[counter] = data[0];
               counter += 1
            if(mode == "cal"):
               data = [float(s) for s in line.split()]
               caldata_x[counter] = data[0];
               caldata_y[counter,0] = data[1];
               caldata_y[counter,1] = data[2];
               caldata_y[counter,2] = data[3];
               caldata_y[counter,3] = data[4];
               counter += 1
            if(mode == "sha"):
               data = [float(s) for s in line.split()]
               shadata_x[counter] = data[0];
               shadata_y[counter] = data[1];
               counter += 1

      # Change to dateformat
      for i in range(0, len(cdfdata_x)):
         if(medrange == 1):
            cdfdata_x[i] = matplotlib.dates.date2num(currdate + datetime.timedelta(0, 3600*(int(cdfdata_x[i]))))
         else:
            cdfdata_x[i] = matplotlib.dates.date2num(currdate + datetime.timedelta(0, 3600*(int(cdfdata_x[i])+timezone - PDT)))
      for i in range(0, len(obsdata_x)):
         if(medrange == 1):
            obsdata_x[i] = matplotlib.dates.date2num(currdate + datetime.timedelta(0, 3600*(int(obsdata_x[i]))))
         else:
            obsdata_x[i] = matplotlib.dates.date2num(currdate + datetime.timedelta(0, 3600*(int(obsdata_x[i])+timezone - PDT)))
      for i in range(0, len(ensdata_x)):
         if(medrange == 1):
            ensdata_x[i] = matplotlib.dates.date2num(currdate + datetime.timedelta(0, 3600*(int(ensdata_x[i]))))
         else:
            ensdata_x[i] = matplotlib.dates.date2num(currdate + datetime.timedelta(0, 3600*(int(ensdata_x[i])+timezone - PDT)))

      # Remove missing values
      for i in range(0, len(ensdata[:,0])):
         for j in range(0, len(ensdata[0,:])):
            if(ensdata[i,j] == -999):
               ensdata[i,j] = None; 
      # Close file
      dataFile.close();
      return 0;
      '''

