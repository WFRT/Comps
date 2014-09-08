# -*- coding: ISO-8859-1 -*-
import matplotlib.pyplot as mpl
import re
import datetime
import Common
import Metric
import numpy as np
import sys
reload(sys)
sys.setdefaultencoding('ISO-8859-1')
#from matplotlib.dates import *
import os
import inspect

def getAllOutputs():
   temp = inspect.getmembers(sys.modules[__name__], inspect.isclass)
   return temp

class Output:
   def __init__(self, thresholds=None, filename=None, leg=None):
      self._filename = filename
      self.lines = ['o-','-','.-','--']
      self.colors = ['r',  'b', 'g', [1,0.73,0.2], 'k']
      self._ms = 8
      self._lw = 2
      if(thresholds == None or len(thresholds) == 0):
         thresholds = [None]
      self._thresholds = thresholds
      self._leg = leg

   # Public 
   # Call this to create a plot, saves to file
   def plot(self, data):
      self._plotCore(data)
      self._legend(data, self._leg)
      self._savePlot(data)
   # Call this to write text output
   def text(self, data):
      self._textCore(data)
   # Does this output recognize the -x flag?
   def supportsX(self):
      return True
   def supportsThreshold(self):
      return True
   @staticmethod
   def description():
      return ""

   # Implement these methods
   def _plotCore(self, data):
      Common.error("This type does not plot")
   def _textCore(self, data):
      Common.error("This type does not output text")

   # Helper functions
   def _getColor(self, i, total):
      return self.colors[i % len(self.colors)]
   def _getStyle(self, i, total):
      return self.lines[(i / len(self.colors)) % len(self.lines)]
   def _savePlot(self, data):
      if(self._filename != None):
         mpl.savefig(self._filename, bbox_inches='tight')
      else:
         fig = mpl.gcf()
         fig.canvas.set_window_title(data.getFilenames()[0])
         mpl.show()
   def _legend(self, data, names=None):
      if(names == None):
         mpl.legend(loc="best")
      else:
         mpl.legend(names, loc="best")

   def _setAxisLimits(self):
      currYlim = mpl.ylim()
      ylim = [self._metric.min(), self._metric.max()]
      if(ylim[0] == None):
         ylim[0] = currYlim[0]
      if(ylim[1] == None):
         ylim[1] = currYlim[1]
      mpl.ylim(ylim)

class LinePlot(Output):
   def __init__(self, metric, xaxis, thresholds, binned=False, filename=None, leg=None):
      Output.__init__(self, thresholds, filename, leg)
      # offsets, dates, location, locationElev, threshold
      self._xaxis = xaxis
      self._binned = binned
      self._metric = metric

   def getXY(self, data, f):
      thresholds = self._thresholds

      data.setAxis(self._xaxis)
      lowerT = [-np.inf for i in range(0, len(thresholds))]
      upperT = thresholds
      if(self._binned):
         lowerT = thresholds[0:-1]
         upperT = thresholds[1:]

      if(self._xaxis == "threshold"):
         x = upperT
         if(self._binned):
            x = [(lowerT[i] + upperT[i])/2 for i in range(0, len(lowerT))]
      else:
         x = data.getAxisValues()

      labels = data.getFilenames()
      F = data.getNumFiles()
      y = np.zeros(len(x), 'float')
      data.setFileIndex(f)
      color = self._getColor(f, F)
      style = self._getStyle(f, F)

      if(self._xaxis == "threshold"):
         for i in range(0, len(lowerT)):
            y[i] = self._metric.compute(data, [lowerT[i], upperT[i]])
      else:
         for i in range(0, len(lowerT)):
            y = y + self._metric.compute(data, [lowerT[i], upperT[i]])
         y = y / len(thresholds)

      if(sum(np.isnan(y)) == len(y)):
         Common.warning("No valid scores for " + labels[f])
      return [x,y]

   def _plotCore(self, data):
      data.setAxis(self._xaxis)
      labels = data.getFilenames()
      F = data.getNumFiles()
      for f in range(0, F):
         [x,y] = self.getXY(data, f)
         color = self._getColor(f, F)
         style = self._getStyle(f, F)
         mpl.plot(x, y, style, color=color, label=labels[f], lw=self._lw, ms=self._ms)

      mpl.ylabel(self._metric.label(data))
      mpl.xlabel(data.getAxisLabel())
      mpl.gca().xaxis.set_major_formatter(data.getAxisFormatter())
      mpl.grid()
      self._setAxisLimits()

   def _textCore(self, data):
      thresholds = self._thresholds

      data.setAxis(self._xaxis)
      labels = data.getFilenames()
      F = data.getNumFiles()
      yy = list()
      for f in range(0, F):
         [x,y] = self.getXY(data, f)
         yy.append(y)

      if(self._filename != None):
         sys.stdout = open(self._filename, 'w')

      maxlength = 0
      for name in data.getFilenames():
         maxlength = max(maxlength, len(name))
      maxlength = str(maxlength)

      # Header
      fmt = "%-"+maxlength+"s"
      lineDesc = data.getAxisDescriptionHeader()
      lineDescN = len(lineDesc) + 2
      lineDescFmt = "%-" + str(lineDescN) + "s |"
      print lineDescFmt % lineDesc,
      for filename in data.getFilenames():
         print fmt % filename,
      print ""

      fmt     = "%-"+maxlength+".2f"
      missfmt = "%-"+maxlength+"s" 
      for i in range(0, len(x)):
         if(type(x[i]) == float):
            print lineDescFmt % x[i],
         else:
            print lineDescFmt % x[i],
         for j in range(0, len(yy)):
            value = yy[j][i]
            if(np.isnan(value)):
               print missfmt % "--",
            else:
               print fmt % value,
         print ""

class ObsFcst(Output):
   def __init__(self, xaxis, filename=None, leg=None):
      Output.__init__(self, None, filename, leg)
      self._xaxis  = xaxis
      self._numBins = 10
   def supportsThreshold(self):
      return False
   @staticmethod
   def description():
      return "Plot observations and forecasts"
   def _plotCore(self, data):
      F = data.getNumFiles()
      data.setAxis(self._xaxis)
      x = data.getAxisValues()

      # Obs line
      mObs  = Metric.Mean("obs")
      y = mObs.compute(data, None)
      mpl.plot(x, y,  ".-", color=[0.3,0.3,0.3], lw=5, label="obs")

      mFcst = Metric.Mean("fcst")
      labels = data.getFilenames()
      for f in range(0, F):
         data.setFileIndex(f)
         color = self._getColor(f, F)
         style = self._getStyle(f, F)

         y = mFcst.compute(data, None)
         mpl.plot(x, y, style, color=color, label=labels[f], lw=self._lw,
               ms=self._ms)
      mpl.ylabel(data.getVariableAndUnits())
      mpl.xlabel(data.getAxisLabel())
      mpl.grid()
      mpl.gca().xaxis.set_major_formatter(data.getAxisFormatter())

class QQ(Output):
   def __init__(self, filename=None, leg=None):
      Output.__init__(self, None, filename, leg)
   def supportsThreshold(self):
      return False
   def supportsX(self):
      return False
   @staticmethod
   def description():
      return "Quantile-quantile plot of obs vs forecasts"
   def getXY(self, data):
      x = list()
      y = list()
      F = len(data.getFilenames())
      for f in range(0, F):
         data.setFileIndex(f)
         [xx,yy] = data.getScores(["obs", "fcst"])
         x.append(np.sort(xx))
         y.append(np.sort(yy))
      return [x,y]

   def _plotCore(self, data):
      data.setAxis("none")
      data.setIndex(0)
      labels = data.getFilenames()
      F = data.getNumFiles()
      [x,y] = self.getXY(data)
      for f in range(0, F):
         color = self._getColor(f, F)
         style = self._getStyle(f, F)

         mpl.plot(x[f], y[f], style, color=color, label=labels[f], lw=self._lw,
               ms=self._ms)
      mpl.ylabel("Forecasts (" + data.getUnits() + ")")
      mpl.xlabel("Observations (" + data.getUnits() + ")")
      ylim = mpl.ylim()
      xlim = mpl.xlim()
      axismax = max(max(ylim),max(xlim))
      mpl.plot([0,axismax], [0,axismax], "--", color=[0.3,0.3,0.3], lw=3, zorder=-100)
      mpl.grid()
   def _textCore(self, data):
      data.setAxis("none")
      data.setIndex(0)
      labels = data.getFilenames()
      F = data.getNumFiles()

      # Header
      maxlength = 0
      for name in data.getFilenames():
         maxlength = max(maxlength, len(name))
      maxlength = int(np.ceil(maxlength/2)*2)
      fmt = "%"+str(maxlength)+"s"
      for filename in data.getFilenames():
         print fmt % filename,
      print ""
      fmt = "%" + str(int(np.ceil(maxlength/2))) + ".1f"
      fmt = fmt + fmt
      fmtS = "%" + str(int(np.ceil(maxlength/2))) + "s"
      fmtS = fmtS + fmtS
      for f in range(0, F):
         print fmtS % ("obs", "fcst"),
      print ""

      [x,y] = self.getXY(data)
      maxPairs = len(x[0])
      for f in range(1, F):
         maxPairs = max(maxPairs, len(x[f]))
      for i in range(0, maxPairs):
         for f in range(0, F):
            color = self._getColor(f, F)
            style = self._getStyle(f, F)
            if(len(x[f]) < i):
               print " --  -- "
            else:
               print fmt % (x[f][i], y[f][i]),
         print "\n",

class Scatter(Output):
   def __init__(self, filename=None, leg=None):
      Output.__init__(self, None, filename, leg)
   def supportsThreshold(self):
      return False
   def supportsX(self):
      return False
   @staticmethod
   def description():
      return "Scatter plot of forecasts vs obs"
   def _plotCore(self, data):
      data.setAxis("none")
      data.setIndex(0)
      labels = data.getFilenames()
      F = data.getNumFiles()
      for f in range(0, F):
         data.setFileIndex(f)
         color = self._getColor(f, F)
         style = self._getStyle(f, F)

         [x,y] = data.getScores(["obs","fcst"])
         mpl.plot(x,y, style, color=color, label=labels[f], lw=self._lw,
               ms=self._ms)
      mpl.ylabel("Forecasts (" + data.getUnits() + ")")
      mpl.xlabel("Observations (" + data.getUnits() + ")")
      ylim = mpl.ylim()
      xlim = mpl.xlim()
      axismax = max(max(ylim),max(xlim))
      mpl.plot([0,axismax], [0,axismax], "--", color=[0.3,0.3,0.3], lw=3, zorder=-100)
      mpl.grid()

class Cond(Output):
   def __init__(self, metric, thresholds, binned, filename=None, leg=None):
      Output.__init__(self, thresholds, filename, leg)
      print self._thresholds
      self._metric = metric
      self._binned = binned
   def supportsThreshold(self):
      return False
   def supportsX(self):
      return False
   @staticmethod
   def description():
      return "Plots forecasts as a function of obs (use -r to specify bin-edges)"
   def _plotCore(self, data):
      data.setAxis("none")
      data.setIndex(0)
      thresholds = self._thresholds

      lowerT = [-np.inf for i in range(0, len(thresholds))]
      upperT = thresholds
      if(self._binned):
         lowerT = thresholds[0:-1]
         upperT = thresholds[1:]
      x = upperT
      if(self._binned):
         x = [(lowerT[i] + upperT[i])/2 for i in range(0, len(lowerT))]

      labels = data.getFilenames()
      F = data.getNumFiles()
      for f in range(0, F):
         color = self._getColor(f, F)
         style = self._getStyle(f, F)
         data.setFileIndex(f)

         y = np.zeros(len(x), 'float')
         for i in range(0, len(lowerT)):
            y[i] = self._metric.compute(data, [lowerT[i], upperT[i]])
         mpl.plot(x,y, style, color=color, label=labels[f], lw=self._lw,
               ms=self._ms)
      mpl.ylabel("Forecasts (" + data.getUnits() + ")")
      mpl.xlabel("Observations (" + data.getUnits() + ")")
      ylim = mpl.ylim()
      xlim = mpl.xlim()
      axismax = max(max(ylim),max(xlim))
      mpl.plot([0,axismax], [0,axismax], "--", color=[0.3,0.3,0.3], lw=3, zorder=-100)
      mpl.grid()

class PitHist(Output):
   def __init__(self, metric, filename=None, leg=None):
      Output.__init__(self, None, filename, leg)
      self._numBins = 10
      self._metric = metric
   def supportsThreshold(self):
      return False
   def supportsX(self):
      return False
   @staticmethod
   def description():
      return "Histogram of PIT values"
   def legend(self, data):
      pass
   def _plotCore(self, data):
      F = data.getNumFiles()
      for f in range(0, F):
         mpl.subplot(1,F,f+1)
         color = self._getColor(f, F)
         data.setAxis("none")
         data.setIndex(0)
         data.setFileIndex(f)
         scores = self._metric.compute(data,None)

         smin = self._metric.min()
         smax = self._metric.max()
         width = (smax - smin) *1.0 / self._numBins
         x = np.linspace(smin,smax,self._numBins+1)
         n = np.histogram(scores, x)[0]
         n = n * 100.0 / sum(n)
         color = "gray"
         xx = x[range(0,len(x)-1)]
         mpl.bar(xx, n, width=width, color=color)
         mpl.plot([smin,smax],[100.0/self._numBins, 100.0/self._numBins], 'k--')
         ytop = 200.0/self._numBins
         mpl.gca().set_ylim([0,ytop])
         if(f == 0):
            mpl.ylabel("Frequency (%)")
         else:
            mpl.gca().set_yticks([])
         #self._setAxisLimits()

         mpl.xlabel(self._metric.label(data))

class Reliability(Output):
   def __init__(self, threshold, filename=None, leg=None):
      Output.__init__(self, None, filename, leg)
      if(threshold == None):
         Common.error("Reliability plot needs a threshold (use -r)")
      self._threshold = threshold
   def supportsX(self):
      return False
   def _plotCore(self, data):
      F = data.getNumFiles()
      N = 6
      edges = np.linspace(0,1,N+1)
      x  = np.linspace(0.5/N,1-0.5/N,N)
      labels = data.getFilenames()
      for f in range(0, F):
         color = self._getColor(f, F)
         style = self._getStyle(f, F)
         data.setAxis("none")
         data.setIndex(0)
         data.setFileIndex(f)
         var = data.getPvar(self._threshold)
         [obs, p] = data.getScores(["obs", var])
         p = 1 - p
         obs = obs > self._threshold

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
            x[i] = np.mean(p[I])

         mpl.plot(x, y, style, color=color, lw=self._lw, ms=self._ms, label=labels[f])
         self.plotConfidence(x, y, n, color=color)
      mpl.plot([0,1], [0,1], color="k")
      mpl.xlim([0,1])
      mpl.ylim([0,1])
      color = "gray"
      mpl.plot([0,1], [clim,clim], ":", color=color)
      mpl.plot([clim,clim], [0,1], ":", color=color)
      mpl.plot([0,1], [clim/2,1-(1-clim)/2], "--", color=color)
      mpl.axis([0,1,0,1])
      mpl.xlabel("Cumulative probability")
      mpl.ylabel("Observed frequency")
      units = " " + data.getUnits()
      mpl.title("Threshold: " + str(self._threshold) + units)
      #self._setAxisLimits()
   def plotConfidence(self, x, y, n, color):
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
      mpl.plot(x, upper, style, color=color, lw=self._lw, ms=self._ms)
      mpl.plot(x, lower, style, color=color, lw=self._lw, ms=self._ms)
      Common.fill(x, lower, upper, color, alpha=0.3)
   @staticmethod
   def description():
      return "Reliability diagram for a certain threshold (-r)"


class DRoc(Output):
   def __init__(self, threshold, filename=None, leg=None, fthresholds=None, doNorm=False):
      Output.__init__(self, None, filename, leg)
      if(threshold == None):
         Common.error("DRoc plot needs a threshold (use -r)")
      self._threshold = threshold
      self._doNorm = doNorm
      self._fthresholds = fthresholds
   def supportsX(self):
      return False
   def _plotCore(self, data):
      F = data.getNumFiles()
      if(self._fthresholds != None):
         fthresholds = self._fthresholds
      else:
         if(data.getVariable() == "Precip"):
            fthresholds = [0,1e-5,0.001,0.005,0.01,0.05,0.1,0.2,0.3,0.5,1,2,3,5,10,20,100]
         else:
            N = 31
            fthresholds = np.linspace(self._threshold-10, self._threshold+10, N)
      threshold = self._threshold

      labels = data.getFilenames()
      for f in range(0, F):
         color = self._getColor(f, F)
         style = self._getStyle(f, F)
         data.setAxis("none")
         data.setIndex(0)
         data.setFileIndex(f)
         [obs, fcst] = data.getScores(["obs", "fcst"])

         y = np.nan*np.zeros([len(fthresholds),1],'float')
         x = np.nan*np.zeros([len(fthresholds),1],'float')
         for i in range(0,len(fthresholds)):
            fthreshold = fthresholds[i]
            a    = np.ma.sum((fcst >= fthreshold) & (obs >= threshold)) # Hit
            b    = np.ma.sum((fcst >= fthreshold) & (obs <  threshold)) # FA
            c    = np.ma.sum((fcst <  fthreshold) & (obs >= threshold)) # Miss
            d    = np.ma.sum((fcst <  fthreshold) & (obs <  threshold)) # Correct rejection
            if(a + c > 0 and b + d > 0):
               y[i] = a / 1.0 / (a + c)
               x[i] = b / 1.0 / (b + d)
               if(self._doNorm):
                  from scipy.stats import norm
                  y[i] = norm.ppf(a / 1.0 / (a + c))
                  x[i] = norm.ppf(b / 1.0 / (b + d))
                  if(np.isinf(y[i])):
                     y[i] = np.nan
                  if(np.isinf(x[i])):
                     x[i] = np.nan
               if(not np.isnan(y[i]) and f == 0):
                  mpl.text(x[i], y[i], "%2.1f" % fthreshold, color=color)
         mpl.plot(x, y, style, color=color, label=labels[f], lw=self._lw, ms=self._ms)
         if(self._doNorm):
            xlim = mpl.xlim()
            ylim = mpl.ylim()
            q0 =  max(abs(xlim[0]), abs(ylim[0]))
            q1 =  max(abs(xlim[1]), abs(ylim[1]))
            mpl.plot([-q0,q1], [-q0,q1], 'k--')
            mpl.xlabel("Normalized false alarm rate")
            mpl.ylabel("Normalized hit rate")
         else:
            mpl.plot([0,1], [0,1], color="k")
            mpl.axis([0,1,0,1])
            mpl.xlabel("False alarm rate")
            mpl.ylabel("Hit rate")
      units = " " + data.getUnits()
      mpl.title("Threshold: " + str(self._threshold) + units)
      mpl.grid()
   @staticmethod
   def description():
      return "Plots the receiver operating characteristics curve for the deterministic " \
         + "forecast for a single threshold. Uses different forecast thresholds to create points."

class DRocNorm(DRoc):
   def __init__(self, threshold, filename=None, leg=None):
      DRoc.__init__(self, threshold, filename, leg, doNorm=True)
   @staticmethod
   def description():
      return "Same as DRoc, except the hit and false alarm rates are transformed using the " \
            "inverse of the standard normal distribution in order to highlight the extreme " \
            "values." 

class DRoc0(DRoc):
   def __init__(self, threshold, filename=None, leg=None):
      DRoc.__init__(self, threshold, filename, leg, fthresholds=[threshold], doNorm=False)
   @staticmethod
   def description():
      return "Same as DRoc, except don't use different forecast thresholds: Use the "\
      "same\n threshold for forecast and obs."
