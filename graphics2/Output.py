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
from matplotlib.dates import *
import os
from matplotlib.dates import YearLocator, MonthLocator, DateFormatter, DayLocator, HourLocator, WeekdayLocator
from matplotlib.ticker import ScalarFormatter

class Output:
   def __init__(self, metric, filename=None):
      self._filename = filename
      self._metric = metric
class Plot(Output):
   def __init__(self, metric, filename=None):
      Output.__init__(self, metric, filename)
      self.lines = ['o-','-','.-','--']
      self.colors = ['r',  'b', 'g', [1,0.73,0.2], 'k']
   def getColor(self, i, total):
      return self.colors[i % len(self.colors)]
   def getStyle(self, i, total):
      return self.lines[(i / len(self.colors)) % len(self.lines)]
   def output(self, data):
      self.outputCore(data)
      self.legend(data)
      self.save(data)
   def save(self, data):
      if(self._filename != None):
         mpl.savefig(self._filename, bbox_inches='tight')
      else:
         fig = mpl.gcf()
         fig.canvas.set_window_title(data.getFilenames()[0])
         mpl.show()
   def _setAxisLimits(self):
      currYlim = mpl.ylim()
      ylim = self._metric.ylim()
      if(ylim[0] == None):
         ylim[0] = currYlim[0]
      if(ylim[1] != None):
         ylim[1] = currYlim[1]
      mpl.ylim(ylim)

      currXlim = mpl.xlim()
      xlim = self._metric.xlim()
      if(xlim[0] == None):
         xlim[0] = currXlim[0]
      if(xlim[1] != None):
         xlim[1] = currXlim[1]
      mpl.xlim(xlim)

   def legend(self, data, names=None):
      if(names == None):
         mpl.legend(loc="best")
      else:
         mpl.legend(names, loc="best")

class LinePlot(Plot):
   def __init__(self, metric, xaxis, saxis=None, filename=None):
      Plot.__init__(self, metric, filename)
      # offsets, dates, location, locationElev, threshold
      self._xaxis = xaxis
      self._saxis = saxis
   def outputCore(self, data):
      F = data.getNumFiles()
      data.setAxis(self._xaxis)
      x = self._metric.getX(data) #data.getAxisValues()
      N = len(x)
      labels = data.getFilenames()
      for f in range(0, F):
         data.setFileIndex(f)
         lineColor = self.getColor(f, F)
         lineStyle = self.getStyle(f, F)

         y = self._metric.compute(data)
         mpl.plot(x, y, lineStyle, color=lineColor, label=labels[f])

      mpl.ylabel(self._metric.ylabel(data))
      mpl.xlabel(self._metric.xlabel(data))
      mpl.gca().xaxis.set_major_formatter(self._metric.getAxisFormatter(data))
      mpl.grid()
      self._setAxisLimits()

class ThresholdPlot(Plot):
   def __init__(self, metric, thresholds, filename=None):
      Plot.__init__(self, metric, filename)
      self._metric = metric
      self._thresholds = thresholds
   def outputCore(self, data):
      F = data.getNumFiles()
      x = self._thresholds
      data.setAxis("none")
      data.setIndex(0)
      for f in range(0, F):
         color = self.getColor(f, F)
         style = self.getStyle(f, F)
         data.setFileIndex(f)
         y = self._metric.compute(data, self._thresholds)

         mpl.plot(x, y, style, color=color)
         mpl.ylabel(self._metric.ylabel(data))
         mpl.ylim([0, 1])

         mpl.xlabel(data.getVariableAndUnits())
      self._setAxisLimits()

class ObsFcstPlot(Plot):
   def __init__(self, metric, xaxis, filename=None):
      Plot.__init__(self, metric, filename)
      self._xaxis  = xaxis
      self._numBins = 10
   def outputCore(self, data):
      F = data.getNumFiles()
      data.setAxis(self._xaxis)
      x = data.getAxisValues()

      # Obs line
      mObs  = Metric.Mean("obs")
      y = mObs.compute(data)
      mpl.plot(x, y,  ".-", color=[0.3,0.3,0.3], lw=5, label="obs")

      mFcst = Metric.Mean("fcst")
      labels = data.getFilenames()
      for f in range(0, F):
         data.setFileIndex(f)
         lineColor = self.getColor(f, F)
         lineStyle = self.getStyle(f, F)

         y = mFcst.compute(data)
         mpl.plot(x, y, lineStyle, color=lineColor, label=labels[f])
      mpl.ylabel(self._metric.ylabel(data))
      mpl.xlabel(self._metric.xlabel(data))
      self._setAxisLimits()
      mpl.grid()
      mpl.gca().xaxis.set_major_formatter(data.getAxisFormatter())

class PitPlot(Plot):
   def __init__(self, metric, filename=None):
      Plot.__init__(self, metric, filename)
      self._numBins = 10
   def legend(self, data):
      pass
   def outputCore(self, data):
      F = data.getNumFiles()
      width = 1.0 / self._numBins
      for f in range(0, F):
         mpl.subplot(1,F,f+1)
         color = self.getColor(f, F)
         data.setAxis("none")
         data.setIndex(0)
         data.setFileIndex(f)
         pits = self._metric.compute(data)

         x = np.linspace(0,1,self._numBins+1)
         n = np.histogram(pits, x)[0]
         n = n * 1.0 / sum(n)
         color = "gray"
         xx = x[range(0,len(x)-1)]
         mpl.bar(xx, n, width=width, color=color)
         mpl.plot([0,1],[1.0/self._numBins, 1.0/self._numBins], 'k--')
         ytop = 2.0/self._numBins
         mpl.gca().set_ylim([0,ytop])
         if(f == 0):
            mpl.ylabel("Observed frequency")
         else:
            mpl.gca().set_yticks([])
         self._setAxisLimits()

         mpl.xlabel("Probability")

class ReliabilityPlot(Plot):
   def __init__(self, threshold, filename=None):
      Plot.__init__(self, None, filename)
      if(threshold == None):
         Common.error("Reliability plot needs a threshold (use -r)")
      self._threshold = threshold
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
   def outputCore(self, data):
      F = data.getNumFiles()
      N = 6
      edges = np.linspace(0,1,N+1)
      x  = np.linspace(0.5/N,1-0.5/N,N)
      labels = data.getFilenames()
      for f in range(0, F):
         color = self.getColor(f, F)
         style = self.getStyle(f, F)
         data.setAxis("none")
         data.setIndex(0)
         data.setFileIndex(f)
         var = self.getPvar(self._threshold)
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

         mpl.plot(x, y, style, color=color, label=labels[f])
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
      mpl.plot(x, upper, style, color=color)
      mpl.plot(x, lower, style, color=color)
      Common.fill(x, lower, upper, color, alpha=0.3)

class Text(Output):
   def __init__(self, metric, xaxis, filename):
      Output.__init__(self, filename)
      self._metric = metric
      self._xaxis = xaxis
   def output(self, data):
      if(self._filename != None):
         sys.stdout = open(self._filename, 'w')
      F = data.getNumFiles()
      data.setAxis(self._xaxis)
      y = list()
      for f in range(0, F):
         data.setFileIndex(f)

         x = data.getAxisDescriptions()
         y.append(self._metric.compute(data))

      maxlength = 0
      for name in data.getFilenames():
         maxlength = max(maxlength, len(name))
      maxlength = str(maxlength)

      # Header
      fmt = "%-"+maxlength+"s"
      print "%-20s |" % data.getAxisDescriptionHeader(), 
      for filename in data.getFilenames():
         print fmt % filename,
      print ""

      fmt     = "%-"+maxlength+".2f"
      missfmt = "%-"+maxlength+"s" 
      for i in range(0, len(x)):
         if(type(x[i]) == float):
            print "%-20d |" % x[i],
         else:
            print "%-20s |" % x[i],
         for j in range(0, len(y)):
            value = y[j][i]
            if(np.isnan(value)):
               print missfmt % "--",
            else:
               print fmt % value,
         print ""

   def save(self):
      pass
