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
   def __init__(self, metric):
      self._metric = metric
      self.lines = ['o-','-','.-','--']
      self.colors = ['r',  'b', 'g', [1,0.73,0.2], 'k']
   def getColor(self, i, total):
      return self.colors[i % len(self.colors)]
   def getStyle(self, i, total):
      return self.lines[(i / len(self.colors)) % len(self.lines)]

class PlotLine(Plot):
   def __init__(self, xaxis, saxis=None):
      # offsets, dates, location, locationElev, threshold
      self._xaxis = xaxis
      self._saxis = saxis
   def plot(self, data):
      F = data.getNumFiles()
      for f in range(0, len(F)):
         lineColor = self.getColor(f, F)
         lineStyle = self.getColor(f, F)

         data.setAxis(self._axis)
         x = data.getAxisValues()
         N = len(x)
         for i in range(0,N):
            data.setIndex(i)
            y[i] = self._metric.compute(data)
         mpl.plot(x, y, lineStyle, color=lineColor)

class PlotPit(Plot):
   def plot(self, data):
      F = data.getNumFiles()
      for f in range(0, len(F)):
         mpl.subplot(1,F,f)
         color = self.getColor(f, F)
         data.setAxis(self._axis)
         x = data.getAxisValues()
         N = len(x)
         data.setAxis(-1)
         data.setIndex(0)
         pits = data.getScore("pit")
         mpl.hist(pits)
      mpl.xlabel("Probability")
      mpl.ylabel("Observed frequency")
