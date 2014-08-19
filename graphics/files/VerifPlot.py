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
      return [AnalogPlot, BrierPlot, BiasFreqPlot, CmaePlot, CorrPlot, DmbPlot,
            DRocNormPlot, DRocPlot, DRoc0Plot, EconPlot, ErrorPlot, EtsPlot, FalseAlarmPlot,
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
      ax.plot(x, y,  ".-", color=[0.3,0.3,0.3], lw=5, label="obs")

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
      elif(scale > 50):
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
      llcrnrlat= max(-90, min(lats) - dlat/10)
      urcrnrlat= min(90, max(lats) + dlat/10)
      llcrnrlon= min(lons) - dlon/10
      urcrnrlon= max(lons) + dlon/10
      res = Common.getMapResolution(lats, lons)
      dx = pow(10,np.ceil(np.log10(max(lons) - min(lons))))/10
      dy = pow(10,np.ceil(np.log10(max(lats) - min(lats))))/10
      my = np.ma.masked_array(y[:], np.isnan(y[:]))
      clim = [np.ma.min(my), np.ma.max(my)]
      for nf in range(0,NF):
         Common.subplot(nf,NF)
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
         locs = file.getLocations()
         x0, y0 = map(lons, lats)
         map.scatter(x0, y0, c=y[:,nf], s=40)
         if(len(x0)) < 100:
            for i in range(0,len(x0)):
               mpl.text(x0[i], y0[i], "(%d,%d)" % (i,locs[i]))
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

class DefaultPlot(Plot):
   def __init__(self, metric):
      Plot.__init__(self)
      self.metric = metric
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         offsets = file.getOffsets()
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         if(0):
            scores = file.getScores(self.metric)
            values  = np.zeros([len(scores[0,:]),1], 'float')
            for i in range(0,len(scores[0,:])):
               temp = scores[:,i]
               mask = np.where(temp > -999)
               values[i] = np.mean(temp[mask])
            ax.plot(offsets, values, lineStyle, color=lineColor)

         x = file.getX()
         y = file.getY(self.metric)
         ax.plot(x, y, lineStyle, color=lineColor)

         #locations = file.getLocations()
         #I = np.argsort(y)
         #print I[0:5]
         #print locations[I[0:5]]
         #print np.sort(y)[0:5]
      
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter(self.metric))
         ax.set_xlabel(file.getXLabel())
         if(self.metric.find(".") == -1):
            ax.set_ylabel(self.metric.capitalize() + " " + file.getUnitsString())
         else:
            ax.set_ylabel(self.metric[self.metric.index(".")+1:].capitalize() + " " + file.getUnitsString())

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

class StdErrorPlot(Plot):
   @staticmethod
   def description():
      return "Plots the standard error of the forecasts"
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         offsets = file.getOffsets()
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         bias = file.getScores("bias")
         values  = np.zeros([len(bias[0,:]),1], 'float')
         mbias = np.ma.masked_array(bias,np.isnan(bias))

         dim = file.getByAxis()
         if(dim == 0):
            N = len(bias[:,0,0]) 
            y = np.zeros(N, 'float')
            for i in range(0, N):
               y[i] = np.std(mbias[i,:,:].flatten())
         elif(dim == 1):
            N = len(bias[0,:,0]) 
            y = np.zeros(N, 'float')
            for i in range(0, N):
               y[i] = np.std(mbias[:,i,:].flatten())
         elif(dim == 2):
            N = len(bias[0,0,:]) 
            y = np.zeros(N, 'float')
            for i in range(0, N):
               y[i] = np.std(mbias[:,:,i].flatten())

         x = file.getX()

         ax.plot(x, y, lineStyle, color=lineColor)
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel("Standr error " + file.getUnitsString())
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter("bias"))

         temp = bias.flatten()
         mask = np.where(temp > -999)
         print np.std(temp[mask])

class NumPlot(Plot):
   @staticmethod
   def description():
      return "Plots the number of valid observations and forecasts"
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         offsets = file.getOffsets()
         obs  = file.getScores('obs')
         fcst = file.getScores('fcst')

         x = file.getX()
         mobs = np.ma.masked_array(obs,np.isnan(obs))
         mfcst = np.ma.masked_array(fcst,np.isnan(fcst))

         dim = file.getByAxis()
         if(dim == 0):
            N = len(obs[:,0,0]) 
            yobs  = np.zeros(N, 'float')
            yfcst = np.zeros(N, 'float')
            for i in range(0, N):
               yobs[i]  = mobs[i,:,:].count()
               yfcst[i] = mfcst[i,:,:].count()
         elif(dim == 1):
            N = len(obs[0,:,0]) 
            yobs  = np.zeros(N, 'float')
            yfcst = np.zeros(N, 'float')
            for i in range(0, N):
               yobs[i]  = mobs[:,i,:].count()
               yfcst[i] = mfcst[:,i,:].count()
         elif(dim == 2):
            N = len(obs[0,0,:]) 
            yobs  = np.zeros(N, 'float')
            yfcst = np.zeros(N, 'float')
            for i in range(0, N):
               yobs[i]  = mobs[:,:,i].count()
               yfcst[i] = mfcst[:,:,i].count()
         if(nf == 0):
            mpl.plot(x, yobs, 'o-r', color="yellow", label="obs", ms=12)
         mpl.plot(x, yfcst, lineStyle, color=lineColor, label=file.getFilename())
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel("Number of valid data")
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter('fcst'))
   def legend(self, ax, names=None):
      mpl.legend()

class RmsePlot(Plot):
   @staticmethod
   def description():
      return "Plots the root mean squared error of the forecasts"
   def __init__(self, metric=None):
      Plot.__init__(self)
      self.metric = metric
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         if(self.metric != None):
            metric = self.metric
         elif(file.hasScore("mae")):
            metric = "mae"
         elif(file.hasScore("bias")):
            metric = "bias"
         offsets = file.getOffsets()
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)

         bias = file.getScores("bias")
         values  = np.zeros([len(bias[0,:]),1], 'float')
         mbias = np.ma.masked_array(bias,np.isnan(bias))

         dim = file.getByAxis()
         if(dim == 0):
            N = len(bias[:,0,0]) 
            y = np.zeros(N, 'float')
            for i in range(0, N):
               y[i] = np.sqrt(np.mean((mbias[i,:,:]**2).flatten()))
         elif(dim == 1):
            N = len(bias[0,:,0]) 
            y = np.zeros(N, 'float')
            for i in range(0, N):
               y[i] = np.sqrt(np.mean((mbias[:,i,:]**2).flatten()))
         elif(dim == 2):
            N = len(bias[0,0,:]) 
            y = np.zeros(N, 'float')
            for i in range(0, N):
               y[i] = np.sqrt(np.mean((mbias[:,:,i]**2).flatten()))

         x = file.getX()
         ax.plot(x, y, lineStyle, color=lineColor)
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel("RMSE " + file.getUnitsString())
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter('fcst'))

class PitPlot(Plot):
   @staticmethod
   def description():
      return "Plots a PIT histogram of the forecasts (analogous to a rank histogram, "\
             + "but for probabilities)"
   def __init__(self, threshold=None, numBins=10):
      Plot.__init__(self)
      self.threshold = threshold
      self.numBins = numBins
   def plotCore(self, ax):
      NF = len(self.files)
      width = 1.0 / self.numBins / NF
      width = 1.0 / self.numBins
      for nf in range(0,NF):
         Common.subplot(nf,NF)
         file = self.files[nf]
         if(self.threshold == None):
            pits = file.getFlatScores("pit")
         else:
            [p,obs] = file.getFlatScores(["p11", "obs"])
            pits = np.array(p)
            I0 = np.where(obs <= 11)[0]
            I1 = np.where(obs > 11)[0]
            pits[I1] = 1-pits[I1]
            pits[I0] = pits[I0] * np.random.random(len(I0))
            pits[I1] = 1 - pits[I1] * np.random.random(len(I1))

         x = np.linspace(0,1,self.numBins+1)
         n = np.histogram(pits.flatten(), x)[0]
         n = n * 1.0 / sum(n)
         color = "gray" #self.getColor(nf, NF)
         xx = x[range(0,len(x)-1)]
         mpl.bar(xx, n, width=width, color=color)
         mpl.plot([0,1],[1.0/self.numBins, 1.0/self.numBins], 'k--')

         # X-axis
         mpl.gca().set_xlim([0,1])
         mpl.gca().set_xlabel("Cumulative probability")

         # Y-axis
         ytop = 2.0/self.numBins
         mpl.gca().set_ylim([0,ytop])
         if(nf == 0):
            mpl.gca().set_ylabel("Observed frequency")
         else:
            mpl.gca().set_yticks([])

         # Compute calibration deviation
         D  = np.sqrt(1.0 / self.numBins * np.sum((n - 1.0 / self.numBins)**2))
         D0 = np.sqrt((1.0 - 1.0 / self.numBins) / (len(pits) * self.numBins))
         mpl.text(0, ytop, "Dev: %2.4f\nExp: %2.4f" % (D,D0), verticalalignment="top")

   def legend(self, ax, names=None):
      if(names == None):
         names = list()
         for i in range(0, len(self.files)):
            names.append(self.files[i].getFilename())
      NF = len(self.files)
      for nf in range(0,NF):
         Common.subplot(nf,NF)
         mpl.title(names[nf])

class ReliabilityPlot(Plot):
   @staticmethod
   def description():
      return "Plots a reliability diagram for a certain threshold (-r)"
   def __init__(self, threshold=None):
      Plot.__init__(self)
      if(threshold == None):
         self.error("Reliability plot needs a threshold (use -r)")
      if(len(threshold) > 1):
         self.error("Reliability plot cannot take multiple thresholds")
      self.threshold = threshold[0]
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]

         N = 6
         edges = np.linspace(0,1,N+1)
         bins  = np.linspace(0.5/N,1-0.5/N,N)
         var = file.getPvar(self.threshold)
            
         p   = 1-file.getScores(var).flatten()
         obs = file.getScores('obs').flatten() > self.threshold
         # Remove points with missing forecasts and/or observations
         I   = np.where(np.logical_not(np.isnan(p)) & np.logical_not(np.isnan(obs)))
         p   = p[I]
         obs = obs[I]

         clim = np.mean(obs)
         # Compute frequencies
         y = np.nan*np.zeros([len(edges)-1,1],'float')
         n = np.zeros([len(edges)-1,1],'float')
         for i in range(0,len(edges)-1):
            q = (p >= edges[i])& (p < edges[i+1])
            I = np.where(q)
            n[i] = len(obs[I])
            # Need at least 10 data points to be valid
            if(n[i] >= 10):
               y[i] = np.mean(obs[I])
            bins[i] = np.mean(p[I])

         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         ax.plot(bins, y, lineStyle, color=lineColor)
         self.plotConfidence(ax, bins, y, n, color=lineColor)

         ax.plot([0,1], [0,1], color="k")
         #mpl.gca().yaxis.grid(False)
         
         ax.plot([0,1], [clim,clim], ":", color=lineColor)
         ax.plot([clim,clim], [0,1], ":", color=lineColor)
         ax.plot([0,1], [clim/2,1-(1-clim)/2], "--", color=lineColor)
         ax.axis([0,1,0,1])
         ax.set_xlabel("Cumulative probability")
         ax.set_ylabel("Observed frequency")
         units = " " + file.getUnits()
         ax.set_title("Threshold: " + str(self.threshold) + units)

         #ax2 = mpl.gcf().add_axes([0.2,0.7,0.15,0.15])
         #ax2.get_xaxis().set_visible(False)
         #ax2.set_xlim(1,N)
         #mpl.bar(range(1,N+1), n, width=1, log=True)

   def plotConfidence(self, ax, bins, y, n, color):
      z = 1.96 # 95% confidence interval
      type = "normal"
      style = "--"
      if type == "normal":
         mean = y
         lower = mean - z*np.sqrt(y*(1-y)/n)
         upper = mean + z*np.sqrt(y*(1-y)/n)
      elif type == "wilson":
         mean =  1/(1+1.0/n*z**2) * ( y + 0.5*z**2/n)
         upper = mean + 1/(1+1.0/n*z**2)*z*np.sqrt(y*(1-y)/n + 0.25*z**2/n**2)
         lower = mean - 1/(1+1.0/n*z**2)*z*np.sqrt(y*(1-y)/n + 0.25*z**2/n**2)
      ax.plot(bins, upper, style, color=color)
      ax.plot(bins, lower, style, color=color)
      self._fill(ax, bins, lower, upper, color, alpha=0.3)

class SpreadSkillPlot(Plot):
   @staticmethod
   def description():
      return "Plots forecast error (RMSE) as a function of forecast spread (STD)"
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         N = 10
         edges = np.linspace(0,10,N+1)
         bins  = np.linspace(0.5/N,1-0.5/N,N)
         [spread, skill] = file.getFlatScores(['ensSpread', 'mae'])
         style  = self.getStyle(nf, NF)
         color  = self.getColor(nf, NF)

         # Compute frequencies
         y = np.nan*np.zeros([len(edges)-1,1],'float')
         n = np.zeros([len(edges)-1,1],'float')
         for i in range(0,len(edges)-1):
            q = (spread >= edges[i])& (spread < edges[i+1])
            I = np.where(q)
            n[i] = len(skill[I])
            # Need at least 10 data points to be valid
            if(n[i] >= 5):
               y[i] = np.mean(skill[I])
            bins[i] = np.mean(spread[I])
         ax.plot(spread, skill, ".", color=color, alpha=0.3)
         ax.plot(bins, y, style, color=color)
         ax.set_xlabel("Ensemble spread")
         ax.set_ylabel("Ensemble mean skill (MAE)")

# Abstract class for plotting scores based on the 2x2 contingency table
# Derived classes must implement getY and getYlabel
class ContingencyPlot(Plot):
   def __init__(self, thresholds=None):
      Plot.__init__(self)
      self.thresholds = thresholds

   def plotCore(self, ax):
      NF = len(self.files)
      units = ""

      if(self.thresholds == None):
         N = 20
         obs  = self.files[0].getScores('obs')
         self.thresholds = np.linspace(min(obs.flatten()), max(obs.flatten()),N+1)
      else:
         self.thresholds = self.thresholds

      for nf in range(0,NF):
         file = self.files[nf]
         style = self.getStyle(nf, NF)
         color = self.getColor(nf, NF)

         N = 20
         obs  = file.getScores('obs')
         fcst  = file.getScores('fcst')

         # Remove points with missing forecasts and/or observations
         I    = np.where(np.logical_not(np.isnan(fcst)) & np.logical_not(np.isnan(obs)))
         fcst = np.array(fcst[I])
         obs  = np.array(obs[I])

         if(len(fcst) > 0):
            # Compute frequencies
            y = np.nan*np.zeros([len(self.thresholds),1],'float')
            for i in range(0,len(self.thresholds)):
               threshold = self.thresholds[i]
               a    = np.ma.sum((fcst >= threshold) & (obs >= threshold))
               b    = np.ma.sum((fcst >= threshold) & (obs <  threshold))
               c    = np.ma.sum((fcst <  threshold) & (obs >= threshold))
               d    = np.ma.sum((fcst <  threshold) & (obs <  threshold))

               y[i] = self.getY(a, b, c, d)

            ax.plot(self.thresholds, y, style, color=color)
            units = " (" + file.getUnits() + ")"
         else:
            self.warning(file.getFilename() + " does not have any valid forecasts")

      ylim = self.getYlim()
      if(ylim != None):
         ax.set_ylim(ylim)
      ax.set_xlabel("Threshold" + units)
      ax.set_ylabel(self.getYlabel())

   # Default for most contingency plots is [0,1]
   def getYlim(self):
      return [0,1]

class HitRatePlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the hit rate for one or more thresholds (-r). Accepts -c."
   def getY(self, a, b, c, d):
      return a / 1.0 / (a + c)
   def getYlabel(self):
      return "Hit rate"

class FalseAlarmPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the false alarm rate for one or more thresholds (-r). Accepts -c."
   def getY(self, a, b, c, d):
      return b / 1.0 / (b + d)
   def getYlabel(self):
      return "False alarm rate"

class EtsPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the Equitable Threat Score for one or more thresholds (-r). Accepts -c."
   def getY(self, a, b, c, d):
      # Divide by length(I) early on so we don't get integer overflow:
      N = a + b + c + d
      ar   = (a + b) / 1.0 / N * (a + c)
      if(a+b+c-ar > 0):
         return (a - ar) / 1.0 / (a + b + c - ar)
      else:
         return 0
   def getYlabel(self):
      return "Equitable Threat Score"

class ThreatPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the Threat Score for one or more thresholds (-r). Accepts -c."
   def getY(self, a, b, c, d):
      if(a+b+c > 0):
         return a / 1.0 / (a + b + c)
      else:
         return 0
   def getYlabel(self):
      return "Threat Score"

class BiasFreqPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the bias frequency (fcst >= threshold) / (obs >= threshold) for one or more " + \
            "thresholds (-r). Accepts -c."
   def getY(self, a, b, c, d):
      if(a+c > 0):
         return 1.0 * (a + b) / (a + c)
      else:
         return 0
   def getYlabel(self):
      return "Bias frequency (fcst / obs)"
   def getYlim(self):
      return None

class RocPlot(Plot):
   @staticmethod
   def description():
      return "Plots the receiver operating characteristics curve for an ensemble " \
         + "forecast for a single threshold (-r)"
   def __init__(self, threshold):
      self.error("RocPlot currently not implemented")
      Plot.__init__(self)
      self.thresholds = threshold
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         style = self.getStyle(nf, NF)
         color = self.getColor(nf, NF)

class DRocPlot(Plot):
   @staticmethod
   def description():
      return "Plots the receiver operating characteristics curve for the deterministic " \
         + "forecast for a single threshold (-r). Accepts -c."
   def __init__(self, threshold=None):
      Plot.__init__(self)
      if(threshold == None):
         self.error("DRoc plot needs a threshold (use -r)")
      if(len(threshold) > 1):
         self.error("Deterministic ROC plot cannot take multiple thresholds")
      self.threshold = threshold[0]
   def plotCore(self, ax):
      N = 21
      #fthresholds = np.power(10, np.linspace(-5,1,N+1))
      fthresholds = np.linspace(self.threshold-10, self.threshold+10, N)

      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         style = self.getStyle(nf, NF)
         color = self.getColor(nf, NF)

         obs  = file.getScores('obs')
         fcst = file.getScores('fcst')
         # Remove points with missing forecasts and/or observations
         I    = np.where(np.logical_not(np.isnan(fcst)) & np.logical_not(np.isnan(obs)))
         fcst = np.array(fcst[I])
         obs  = np.array(obs[I])

         # Compute frequencies
         #fthresholds = np.linspace(min(obs), max(obs), N+1)
         y = np.nan*np.zeros([len(fthresholds),1],'float')
         x = np.nan*np.zeros([len(fthresholds),1],'float')
         for i in range(0,len(fthresholds)):
            fthreshold = fthresholds[i]
            a    = np.ma.sum((fcst >= fthreshold) & (obs >= self.threshold)) # Hit
            b    = np.ma.sum((fcst >= fthreshold) & (obs <  self.threshold)) # FA
            c    = np.ma.sum((fcst <  fthreshold) & (obs >= self.threshold)) # Miss
            d    = np.ma.sum((fcst <  fthreshold) & (obs <  self.threshold)) # Correct rejection
            if(a + c > 0 and b + d > 0):
               y[i] = a / 1.0 / (a + c) 
               x[i] = b / 1.0 / (b + d) 
         ax.plot(x, y, style, color=color)
         ax.set_xlim([0,1])
         ax.set_ylim([0,1])
         ax.set_xlabel("False alarm rate")
         ax.set_ylabel("Hit rate")
         units = " " + file.getUnits()
         ax.set_title("Threshold: " + str(self.threshold) + units)

class CorrPlot(Plot):
   @staticmethod
   def description():
      return "Plots the correlation between observations and forecasts. Accept -c."
   def plot(self, ax):
      NF = len(self.files)
      corr = np.zeros(NF, 'float')
      names = list()
      for nf in range(0,NF):
         file = self.files[nf]
         names.append(file.getFilename())
         [obs,fcst] = file.getFlatScores(['obs', 'fcst'])
         corr[nf] = np.corrcoef(obs, fcst)[1,0]
      ax.bar(np.linspace(0.5,NF-0.5,NF),corr,color="gray", width=1)
      ax.set_xticks(range(1,NF+1))
      ax.set_xticklabels(names)
      ax.set_ylabel("Correlation")
      mpl.gca().set_xlim([0.5,NF+0.5])
      mpl.gca().set_ylim([0,1])
      mpl.gca().grid()
   def legend(self, ax, names=None):
      pass

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

class AnalogPlot(Plot):
   @staticmethod
   def description():
      return "Plots the forecast skill (RMSE) as a function of daily bias-change"
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
