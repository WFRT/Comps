import matplotlib.pyplot as mpl
import datetime
import numpy as np
import sys
from matplotlib.dates import *
import os
from matplotlib.dates import YearLocator, MonthLocator, DateFormatter, DayLocator, HourLocator, WeekdayLocator
from matplotlib.ticker import ScalarFormatter
class Plot:
   def __init__(self):
      self.files = list()
      self.lines = ['o-','-','.-','--']
      self.colors = ['r',  'b', 'g', 'k', [1,0.73,0.2]]
      #self.colors = [[1,0,0],  [0,0,1], [0,0,1], [0,0,0], [1,0.73,0.2]]
   @staticmethod
   def getAllTypes():
      return [CorrelationPlot, DRocPlot, EtsPlot, NumPlot, ObsFcstPlot, PitPlot,
            ReliabilityPlot, RmsePlot, RocPlot, SpreadSkillPlot, StdErrorPlot, TracePlot]
   @staticmethod
   def getName(cls):
      name = cls.__name__
      name = name[0:-4]
      return name
   @staticmethod
   def description():
      return ""
   def add(self, data):
      self.files.append(data)
   def plot(self, ax):
      self.plotCore(ax)
      # Do generic things like add grid
      ax.grid('on')
   # Get the line style for line 'i' when 'total' lines are used
   def getColor(self, i, total):
      return self.colors[i % len(self.colors)]

   def error(self, message):
      print "Error: " + message
      sys.exit(1)

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
            ax.plot(x, yobs,  "-", color=[0.3,0.3,0.3], lw=5, label="obs")
         ax.plot(x, yfcst, lineStyle, color=lineColor, label=file.getFilename())
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel(file.getUnitsString())
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter('fcst'))
         ax.set_xlabel("Offset (h)")

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
      for nf in range(0,min(1,NF)):
         file = self.files[nf]
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
         mpl.plot(x, yobs, 'o-r', color="yellow", label="obs")
         mpl.plot(x, yfcst, 'o-b', color="red", label="fcst")
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

         x = file.getX()
         y = file.getY(metric)

         ax.plot(x, y, lineStyle, color=lineColor)
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel("RMSE " + file.getUnitsString())
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter('fcst'))

class PitPlot(Plot):
   @staticmethod
   def description():
      return "Plots a PIT histogram of the forecasts (analogous to a rank histogram, "\
             + "but for probabilitiesi)"
   def __init__(self, numBins=10):
      Plot.__init__(self)
      self.numBins = numBins
   def plotCore(self, ax):
      NF = len(self.files)
      width = 1.0 / self.numBins / NF
      width = 1.0 / self.numBins
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf)
         file = self.files[nf]
         pits = file.getFlatScores("pit")
         x = np.linspace(0,1,self.numBins+1)
         n = np.histogram(pits.flatten(), x)[0]
         n = n * 1.0 / sum(n)
         color = self.getColor(nf, NF)
         xx = x[range(0,len(x)-1)]+nf*width
         xx = x[range(0,len(x)-1)]
         mpl.bar(xx, n, width=width, color=color)
         mpl.plot([0,1],[1.0/self.numBins, 1.0/self.numBins], 'k--')
         mpl.gca().set_xlim([0,1])
         mpl.gca().set_ylim([0,1])
         mpl.gca().set_xlabel("Cumulative probability")
         mpl.gca().set_ylabel("Observed frequency")
   def legend(self, ax, names=None):
      if(names == None):
         names = list()
         for i in range(0, len(self.files)):
            names.append(self.files[i].getFilename())
      NF = len(self.files)
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf)
         mpl.title(names[nf])

class ReliabilityPlot(Plot):
   @staticmethod
   def description():
      return "Plots a reliability diagram for a certain threshold (-r)"
   def __init__(self, threshold):
      Plot.__init__(self)
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
         minus = ""
         if(self.threshold < 0):
            # Negative thresholds
            minus = "m"
         if(abs(self.threshold - int(self.threshold)) > 0.01):
            var = "p" + minus + str(abs(self.threshold)).replace(".", "")
         else:
            var   = "p" + minus + str(int(abs(self.threshold)))
            
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
         ax.set_xlabel("Exceedance probability")
         ax.set_ylabel("Observed frequency")
         units = " " + file.getUnits()
         ax.set_title("Threshold: " + str(self.threshold) + units)

         #ax2 = mpl.gcf().add_axes([0.2,0.7,0.15,0.15])
         #ax2.get_xaxis().set_visible(False)
         #ax2.set_xlim(1,N)
         #mpl.bar(range(1,N+1), n, width=1, log=True)

   def plotConfidence(self, ax, bins, y, n, color):
      z = 2
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

class EtsPlot(Plot):
   @staticmethod
   def description():
      return "Plots the equitable threat score for one or more thresholds (-r)"
   def __init__(self, thresholds=None):
      Plot.__init__(self)
      if(thresholds == None):
         #thresholds = np.linspace(-20,20,N+1)
         self.thresholds = np.linspace(min(obs), max(obs),N+1)
         #thresholds = np.linspace(0, 3,N+1)
      else:
         self.thresholds = thresholds

   def plotCore(self, ax):
      NF = len(self.files)
      units = ""
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

         # Compute frequencies
         y = np.nan*np.zeros([len(self.thresholds),1],'float')
         for i in range(0,len(self.thresholds)):
            threshold = self.thresholds[i]
            a    = sum((fcst >= threshold) & (obs >= threshold))
            b    = sum((fcst >= threshold) & (obs <  threshold))
            c    = sum((fcst <  threshold) & (obs >= threshold))
            d    = sum((fcst <  threshold) & (obs <  threshold))

            # Divide by length(I) early on so we don't get integer overflow:
            ar   = (a + b) / 1.0 / len(fcst) * (a + c)
            if(a+b+c-ar > 0):
               y[i] = (a - ar) / 1.0 / (a + b + c - ar)

         ax.plot(self.thresholds, y, style, color=color)
         units = " (" + file.getUnits() + ")"

      ax.set_ylim([0,1])
      ax.set_xlabel("Threshold" + units)
      ax.set_ylabel("Equitable Threat score")

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
         + "forecast for a single threshold (-r)"
   def __init__(self, threshold):
      Plot.__init__(self)
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
            a    = sum((fcst >= fthreshold) & (obs >= self.threshold)) # Hit
            b    = sum((fcst >= fthreshold) & (obs <  self.threshold)) # FA
            c    = sum((fcst <  fthreshold) & (obs >= self.threshold)) # Miss
            d    = sum((fcst <  fthreshold) & (obs <  self.threshold)) # Correct rejection
            y[i] = a / 1.0 / (a + c) 
            x[i] = b / 1.0 / (b + d) 
         ax.plot(x, y, style, color=color)
         ax.set_xlim([0,1])
         ax.set_ylim([0,1])
         ax.set_xlabel("False alarm rate")
         ax.set_ylabel("Hit rate")
         units = " " + file.getUnits()
         ax.set_title("Threshold: " + str(self.threshold) + units)

class CorrelationPlot(Plot):
   @staticmethod
   def description():
      return "Plots the correlation between observations and forecasts"
   def plot(self, ax):
      NF = len(self.files)
      corr = np.zeros(NF, 'float')
      colors = list()
      for nf in range(0,NF):
         file = self.files[nf]
         [obs,fcst] = file.getFlatScores(['obs', 'fcst'])
         corr[nf] = np.corrcoef(obs, fcst)[1,0]
         colors.append(self.getColor(nf, NF))
         print str(corr[nf]) + " " + file.getFilename()
      ax.bar(range(0,NF),corr,color=colors)

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
         dates = file.getDates()
         fcst = file.getScores('fcst')
         obs  = file.getScores('fcst')
         mae  = file.getScores('mae')
         bias  = file.getScores('bias')
         NO = len(fcst[0,:,0])
         NL = len(fcst[0,0,:])
         #clim = np.zeros([NO, NL], 'float')
         diff = abs(fcst[range(1, len(bias[:,0,0])),:,:] - bias[range(0, len(bias[:,0,0])-1),:,:])
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

class MapPlot(Plot):
   @staticmethod
   def description():
      return "Plots observations and forecasts on a map"
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
         lats  = file.getLats()
         lons  = file.getLons()

         if(nf == 0):
            ax.plot(x, yobs,  "-", color=[0.3,0.3,0.3], lw=5, label="obs")
         ax.plot(x, yfcst, lineStyle, color=lineColor, label=file.getFilename())
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel(file.getUnitsString())
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter('fcst'))
         ax.set_xlabel("Offset (h)")

   def legend(self, ax, names=None):
      ax.legend()
