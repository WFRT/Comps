# -*- coding: ISO-8859-1 -*-
import matplotlib.pyplot as mpl
import re
import datetime
import Common
import numpy as np
import sys
reload(sys)
sys.setdefaultencoding('ISO-8859-1')
from matplotlib.dates import *
import os
from matplotlib.dates import YearLocator, MonthLocator, DateFormatter, DayLocator, HourLocator, WeekdayLocator
from matplotlib.ticker import ScalarFormatter

class Plot:
   def __init__(self):
      self.files = list()
      self.lines = ['o-','-','.-','--']
      self.colors = ['r',  'b', 'g', [1,0.73,0.2], 'k']
      #self.colors = [[1,0,0],  [0,0,1], [0,0,1], [0,0,0], [1,0.73,0.2]]
   @staticmethod
   def getAllTypes():
      return [AnalogPlot, BrierPlot, BiasFreqPlot, CorrPlot, DmbPlot, DRocPlot, EconPlot, ErrorPlot, EtsPlot, FalseAlarmPlot,
            HanssenKuiperPlot, HitRatePlot, IgnDecompPlot, NumPlot, ObsFcstPlot, PitPlot,
            ReliabilityPlot, RmsePlot, RocPlot, SpreadSkillPlot, StdErrorPlot, TracePlot,
            VariabilityPlot,WithinPlot]
   @staticmethod
   def getStaticName(cls):
      name = cls.__name__
      name = name[0:-4]
      return name

   def getClassName(self):
      name = self.__class__.__name__
      return name

   @staticmethod
   def supportsCompute():
      return False

   @staticmethod
   def supportsThreshold():
      return False

   @staticmethod
   def description():
      return ""
   def add(self, data):
      self.files.append(data)

   def compute(self, files):
      data =  self.computeCore(files)

      if(type(data) != np.ndarray):
         Common.error("Something is wrong with " + self.getClassName() + ".computeCore().  Does not return an array.")
      if(len(data.shape) < 2):
         Common.error("Something is wrong with " + self.getClassName() + ".computeCore().  Does not return an array of dimension 2.")
      expected = (len(self.files[0].getX()), len(self.files))
      if(data.shape != expected):
         Common.error("Something is wrong with " + self.getClassName() + ".computeCore().  Returns an incorrectly sized array. Expected size: " + str(expected) + ". Got: " + str(data.shape))
      return data

   def computeCore(self, files):
      self.error("Cannot create this type of info for this metric")

   def getXLim(self):
      return None
   def getYLim(self):
      return None

   @staticmethod
   def plotObs(ax, x, y):
      ax.plot(x, y,  "-", color=[0.3,0.3,0.3], lw=5, label="obs")

   def plot(self, ax):
      self.plotCore(ax)
      # Do generic things like add grid
      ax.grid('on')

   def map(self, ax):
      self.mapCore(ax)

   def getMetric(self):
      return "Undefined metric"

   def getYLabel(self, file):
      metric = self.getMetric()
      if(metric.find(".") == -1):
         return metric.capitalize() + " " + self.files[0].getUnitsString()
      else:
         return metric[metric.index(".")+1:].capitalize() + " " + self.files[0].getUnitsString()

   def plotCore(self, ax):
      y = self.compute(self.files)
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         x = file.getX()
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)

         ax.plot(x, y[:,nf], lineStyle, color=lineColor)

         mpl.gca().xaxis.set_major_formatter(file.getXFormatter())
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel(self.getYLabel(file))
      xlim = self.getXLim()
      ylim = self.getYLim()
      if(xlim != None):
         ax.set_xlim(xlim)
      if(ylim != None):
         ax.set_ylim(ylim)

   @staticmethod
   def getResolution(lats, lons):
      dlat = (max(lats) - min(lats))
      dlon = (max(lons) - min(lons))
      scale = max(dlat, dlon)
      if(np.isnan(scale)):
         res = "c"
      elif(scale > 10):
         res = "c"
      elif(scale > 1):
         res = "i"
      elif(scale > 0.1):
         res = "h"
      elif(scale > 0.01):
         res = "f"
      return res

   def mapCore(self, ax):
      from mpl_toolkits.basemap import Basemap
      y = self.compute(self.files)
      NF = len(self.files)
      lats = self.files[0].getLats()
      lons = self.files[0].getLons()

      dlat = (max(lats) - min(lats))
      dlon = (max(lons) - min(lons))
      llcrnrlat= min(lats) - dlat/10
      urcrnrlat= max(lats) + dlat/10
      llcrnrlon= min(lons) - dlon/10
      urcrnrlon= max(lons) + dlon/10
      res = self.getResolution(lats, lons)
      dx = pow(10,np.ceil(np.log10(max(lons) - min(lons))))/10
      dy = pow(10,np.ceil(np.log10(max(lats) - min(lats))))/10
      my = np.ma.masked_array(y[:], np.isnan(y[:]))
      clim = [np.ma.min(my), np.ma.max(my)]
      for nf in range(0,NF):
         [nx,ny] = Common.getSubplotSize(NF)
         mpl.subplot(ny,nx,nf+1)
         map = Basemap(llcrnrlon=llcrnrlon,llcrnrlat=llcrnrlat,urcrnrlon=urcrnrlon,urcrnrlat=urcrnrlat,projection='mill', resolution=res)
         map.drawcoastlines(linewidth=0.25)
         map.drawcountries(linewidth=0.25)
         map.drawmapboundary()
         map.drawparallels(np.arange(-90.,120.,dy),labels=[1,0,0,0])
         map.drawmeridians(np.arange(0.,420.,dx),labels=[0,0,0,1])
         map.fillcontinents(color='coral',lake_color='aqua', zorder=-1)

         file = self.files[nf]
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         lats = file.getLats()
         lons = file.getLons()
         x0, y0 = map(lons, lats)
         map.scatter(x0, y0, c=y[:,nf], s=40)
         cb = map.colorbar()
         cb.set_label(self.getYLabel(file))
         mpl.title(file.getFilename())
         cb.set_clim(clim)
         mpl.clim(clim)

   def text(self):
      file = self.files[0]
      x = file.getXHuman()
      y = self.compute(self.files)

      filenames = list()
      maxlength = 0
      for i in range(0, len(self.files)):
         name = self.files[i].getFilename()
         filenames.append(name)
         maxlength = max(maxlength, len(name))
      maxlength = str(maxlength)

      # Header
      fmt = "%-"+maxlength+"s"
      print "%-20s |" % file.getXHeader(),
      for i in range(0, len(self.files)):
         print fmt % self.files[i].getFilename(),
      print ""

      # Data
      fmt     = "%-"+maxlength+".2f"
      missfmt = "%-"+maxlength+"s"
      for j in range(0, len(x)):
         if(type(x[j]) == float):
            print "%-20d |" % x[j],
         else:
            print "%-20s |" % x[j],
         for i in range(0, len(y[0,:])):
            value = y[j,i]
            if(np.isnan(value)):
               print missfmt % "--",
            else:
               print fmt % value,
         print ""

   # Get the line style for line 'i' when 'total' lines are used
   def getColor(self, i, total):
      return self.colors[i % len(self.colors)]

   def error(self, message):
      Common.error(message)

   def warning(self, message):
      Common.warning(message)

   def getStyle(self, i, total):
      return self.lines[(i / len(self.colors)) % len(self.lines)]

   def legend(self, ax, names=None):
      hs     = list()
      if(names == None):
         names = list()
         for i in range(0, len(self.files)):
            names.append(self.files[i].getFilename())
      for i in range(0, len(self.files)):
         h, = ax.plot(None, None, self.getStyle(i, len(self.files)),
               color=self.getColor(i, len(self.files)))
         hs.append(h)
      ax.legend(hs, names)

   # Fill an area along x, between yLower and yUpper
   # Both yLower and yUpper most correspond to points in x (i.e. be in the same order)
   def _fill(self, ax, x, yLower, yUpper, col, alpha=1):
      # This approach doesn't work, because it doesn't remove points with missing x or y
      #X = np.hstack((x, x[::-1]))
      #Y = np.hstack((yLower, yUpper[::-1]))

      # Populate a list of non-missing points
      X = list()
      Y = list()
      for i in range(0,len(x)):
         if(not( np.isnan(x[i]) or np.isnan(yLower[i]))):
            X.append(x[i])
            Y.append(yLower[i])
      for i in range(len(x)-1, -1, -1):
         if(not (np.isnan(x[i]) or np.isnan(yUpper[i]))):
            X.append(x[i])
            Y.append(yUpper[i])
      ax.fill(X, Y, facecolor=col, alpha=alpha,linewidth=0)

class ObsFcstPlot(Plot):
   @staticmethod
   def description():
      return "Plots observations and forecasts"
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         offsets = file.getOffsets()
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)

         x     = file.getX()
         yobs  = file.getY('obs')
         yfcst = file.getY('fcst')

         if(nf == 0):
            Plot.plotObs(ax, x, yobs)
         ax.plot(x, yfcst, lineStyle, color=lineColor, label=file.getFilename())
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel(file.getUnitsString())
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter())

   def legend(self, ax, names=None):
      ax.legend()

class TracePlot(Plot):
   @staticmethod
   def description():
      return "Plots the forecast skill (RMSE) as a function of daily bias-change and distance "\
           + "from the climatological mean"
   def plot(self, ax):
      NF = len(self.files)
      clim = self.files[0].getScores('fcst')
      for nf in range(0,NF):
         file = self.files[nf]
         lineColor = self.getColor(nf, NF)
         dates = Common.convertDates(file.getDates())
         fcst = file.getScores('fcst')
         obs  = file.getScores('fcst')
         mae  = file.getScores('mae')
         bias  = file.getScores('bias')
         NO = len(fcst[0,:,0])
         NL = len(fcst[0,0,:])
         #clim = np.zeros([NO, NL], 'float')
         diff = abs(bias[range(1, len(bias[:,0,0])),:,:] - bias[range(0, len(bias[:,0,0])-1),:,:])
         mfcst = np.ma.masked_array(fcst,np.isnan(fcst))

         extr = abs(fcst - clim)
         extr = extr[range(1, len(extr[:,0,0])),:,:]

         mdiff = np.ma.masked_array(diff,np.isnan(diff))    
         mextr = np.ma.masked_array(extr,np.isnan(extr))    

         x = np.mean(np.mean(mdiff, axis=2), axis=1).flatten()
         y = np.mean(np.mean(mextr, axis=2), axis=1).flatten()

         # x = diff[:,0,0].flatten()
         # y = extr[range(1,len(extr[:,0,0])),0,0].flatten()
         size = mae[range(1,len(extr[:,0,0])),0,0]
         mpl.scatter(x, y, s=size*6, color=lineColor)
         ax.set_xlabel("Daily bias change " + file.getUnitsString())
         ax.set_ylabel("Distance from climatological mean " + file.getUnitsString())

      xlim = mpl.gca().get_xlim()
      ylim = mpl.gca().get_ylim()
      mpl.gca().set_xlim([0,xlim[1]])
      mpl.gca().set_ylim([0,ylim[1]])

class ErrorPlot(Plot):
   @staticmethod
   def description():
      return "Plots the contribution of systematic error to the total error (RMSE)"
   def __init__(self):
      Plot.__init__(self)
   def plotCore(self, ax):
      NF = len(self.files)
      names = list()
      colTotal = [1,0.2,0.2]
      colNoLoc = [0.75,0,0]
      colNoOff = [0.5,0,0]
      width = 1
      ymax = None
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf)
         file = self.files[nf]
         names.append(file.getFilename())
         bias  = file.getScores("bias")
         mbias = np.ma.masked_array(bias,np.isnan(bias))

         locBias0 = np.mean(np.mean(mbias, axis=1), axis=0)
         offBias0  = np.mean(np.mean(mbias, axis=2), axis=0)

         N0 = len(mbias[:,0,0])
         N1 = len(mbias[0,:,0])
         N2 = len(mbias[0,0,:])
         # Compute the biases along each dimension
         locBias = np.zeros([N0,N1,N2],'float')
         offBias = np.zeros([N0,N1,N2],'float')
         for i in range(0,N0):
            for j in range(0,N1):
               locBias[i,j,:] = locBias0
            for j in range(0,N2):
               offBias[i,:,j] = offBias0
         locBias = np.ma.masked_array(locBias, np.isnan(bias))
         offBias = np.ma.masked_array(offBias, np.isnan(bias))
         
         x = file.getOffsets()
         y0= np.zeros(N1, 'float')
         y1= np.zeros(N1, 'float')
         y2= np.zeros(N1, 'float')

         # Compute scores for each offset
         for i in range(0,N1):
            y0[i] = np.sqrt(np.mean(pow(mbias[:,i,:],2)[:]))
            y1[i] = np.sqrt(np.mean(pow(mbias[:,i,:]-locBias[:,i,:],2)[:]))
            y2[i] = np.sqrt(np.mean(pow(mbias[:,i,:]-locBias[:,i,:]-offBias[:,i,:],2)[:]))
         mpl.bar(x, y0, color=colTotal, width=width, label="Offset systematic error")
         mpl.bar(x, y1, color=colNoLoc, width=width, label="Location systematic error")
         mpl.bar(x, y2, color=colNoOff, width=width, label="Unsystematic error")

         mpl.gca().set_xlabel("Offset (h)")
         mpl.gca().set_ylabel("RMSE " + file.getUnitsString())
         ymax = max(ymax, mpl.gca().get_ylim()[1])

      # Make consistent y-limits
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf)
         mpl.gca().set_ylim([0,ymax])
         mpl.gca().grid()

   def legend(self, ax, names=None):
      if(names == None):
         names = list()
         for i in range(0, len(self.files)):
            names.append(self.files[i].getFilename())
      NF = len(self.files)
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf)
         mpl.title(names[nf])
         mpl.gca().legend(loc="lower center")

class AnalogPlot(Plot):
   @staticmethod
   def description():
      return "Plots the forecast skill (RMSE) as a function of daily bias-change." \
            " It uses the first file to compute the daily bias-change."
   def plot(self, ax):
      NF = len(self.files)
      edges = np.linspace(0, 10, 11)
      bins  = (edges[range(1,len(edges))] + edges[range(0,len(edges)-1)])/2

      # Compute the raw error
      rawfile = self.files[0]
      raw = rawfile.getScores('bias')
      diff = abs(raw[range(1, len(raw[:,0,0])),:,:] - raw[range(0, len(raw[:,0,0])-1),:,:])
      mdiff = np.ma.masked_array(diff,np.isnan(diff)).flatten()
      rawerr  = rawfile.getScores('mae')
      rawerr  = rawerr[range(1, len(rawerr[:,0,0])),:,:]
      mrawerr = np.ma.masked_array(rawerr,np.isnan(rawerr)).flatten()

      for nf in range(0,NF):
         file = self.files[nf]
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)

         fcst  = file.getScores('fcst')
         obs  = file.getScores('obs')
         mae  = file.getScores('mae')
         err  = mae[range(1, len(mae[:,0,0])),:,:]
         merr  = np.ma.masked_array(err,np.isnan(err)).flatten()

         y = np.nan * np.zeros(len(bins), 'float')
         x = np.nan * np.zeros(len(bins), 'float')
         n = np.nan * np.zeros(len(bins), 'float')
         for i in range(0,len(edges)-1):
            q = (mdiff >= edges[i]) & (mdiff < edges[i+1])
            I = np.where(q)[0]
            n[i] = len(I)
            # Need at least 10 data points to be valid
            if(n[i] >= 1):
               y[i] = (np.mean(mrawerr[I]) - np.mean(merr[I]))/np.mean(mrawerr[I])
            x[i] = np.mean(mdiff[I])

         mpl.plot(x, y, lineStyle, color=lineColor)
         ax.set_xlabel("Daily bias change " + file.getUnitsString())
         ax.set_ylabel("MAE " + file.getUnitsString())

      xlim = mpl.gca().get_xlim()
      ylim = mpl.gca().get_ylim()
      mpl.gca().set_xlim([0,xlim[1]])
from BasicPlot import *
from ProbPlot import *
from ThresholdPlot import *
