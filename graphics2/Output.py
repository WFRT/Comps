# -*- coding: ISO-8859-1 -*-
import matplotlib.pyplot as mpl
import re
import datetime
import Common
from Metric import *
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
      self._metric   = metric
      self._filename = filename
class Plot(Output):
   def __init__(self, metric, filename=None):
      Output.__init__(self, metric, filename)
      self._metric = metric
      self.lines = ['o-','-','.-','--']
      self.colors = ['r',  'b', 'g', [1,0.73,0.2], 'k']
   def getColor(self, i, total):
      return self.colors[i % len(self.colors)]
   def getStyle(self, i, total):
      return self.lines[(i / len(self.colors)) % len(self.lines)]
   def output(self, data):
      self.outputCore(data)
      self.save(data)
   def save(self, data):
      if(self._filename != None):
         mpl.savefig(self._filename, bbox_inches='tight')
      else:
         fig = mpl.gcf()
         fig.canvas.set_window_title(data.getFilenames()[0])
         mpl.show()

class LinePlot(Plot):
   def __init__(self, metric, xaxis, saxis=None, filename=None):
      Plot.__init__(self, metric, filename)
      # offsets, dates, location, locationElev, threshold
      self._xaxis = xaxis
      self._saxis = saxis
   def outputCore(self, data):
      F = data.getNumFiles()
      data.setAxis(self._xaxis)
      x = data.getAxisValues()
      N = len(x)
      for f in range(0, F):
         data.setFileIndex(f)
         lineColor = self.getColor(f, F)
         lineStyle = self.getStyle(f, F)

         #y = np.zeros(N, 'float')
         #for i in range(0,N):
         #   data.setIndex(i)
         y = self._metric.compute(data)
         mpl.plot(x, y, lineStyle, color=lineColor)
      mpl.ylabel(self._metric.ylabel(data))
      mpl.xlabel(self._metric.xlabel(data))

class PitPlot(Plot):
   def __init__(self, metric, filename=None):
      Plot.__init__(self, metric, filename)
      self._numBins = 10
   def outputCore(self, data):
      F = data.getNumFiles()
      width = 1.0 / self._numBins
      for f in range(0, F):
         mpl.subplot(1,F,f+1)
         color = self.getColor(f, F)
         data.setAxis("none")
         data.setIndex(0)
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

         mpl.xlabel("Probability")

class Text(Output):
   def __init__(self, metric, xaxis, filename):
      Output.__init__(self, metric, filename)
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
