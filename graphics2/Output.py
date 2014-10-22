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
   _description  = ""
   _defaultAxis = "offset"
   _reqThreshold = False
   _supThreshold = True
   _supX = True
   _experimental = False

   def __init__(self):
      self._filename = None
      self._thresholds = [None]
      leg = None
      self.lines = ['-','-','-','--']
      self.markers = ['o', '', '.', '']
      self.colors = ['r',  'b', 'g', [1,0.73,0.2], 'k']
      self._ms = 8
      self._lw = 2
      self._labfs = 16
      self._tickfs = 16
      self._legfs = 16
      self._figsize = [5,8]
      self._showMargin = True
      self._xrot = 0
      self._minlth = None
      self._majlth = None
      self._majwid = None
      self._bot = None #######
      self._top = None #######
      self._left = None #######
      self._right = None #######
      #self._pad = pad ######
      self._xaxis = self.defaultAxis()
   @classmethod
   def defaultAxis(cls):
      return cls._defaultAxis

   @classmethod
   def requiresThresholds(cls):
      return cls._reqThreshold

   @classmethod
   def supportsX(cls):
      return cls._supX

   @classmethod
   def supportsThreshold(cls):
      return cls._supThreshold

   @classmethod
   def description(cls):
      extra = ""
      if(cls._experimental):
         extra = " " + Common.experimental()
      return cls._description + extra

   # Produce output independently for each value along this axis
   def setAxis(self, axis):
      if(axis != None):
         self._xaxis = axis

   def setThresholds(self, thresholds):
      if(thresholds == None):
         thresholds = [None]
      thresholds = np.array(thresholds)
      self._thresholds = thresholds
   def setFigsize(self, size):
      self._figsize = size
   def setFilename(self, filename):
      self._filename = filename
   def setLegend(self, legend):
      self._legNames = legend
   def setShowMargin(self, showMargin):
      self._showMargin = showMargin
   def setMarkerSize(self, ms):
      self._ms = ms
   def setLineWidth(self, lw):
      self._lw = lw
   def setTickFontSize(self, fs):
      self._tickfs = fs
   def setLabFontSize(self, fs):
      self._labfs = fs
   def setLegFontSize(self, fs):
      self._legfs = fs
   def setXRotation(self, xrot):  #########XRotation  (dsiuta)
      self._xrot = xrot
   def setMinorLength(self, minlth): ######Minor tick length (dsiuta)
      self._minlth = minlth
   def setMajorLength(self, majlth):  ######Major tick length (dsiuta)
      self._majlth = majlth
   def setMajorWidth(self, majwid):   ######Major tick width (dsiuta)
      self._majwid = majwid
   def setBottom(self, bot):          ######Bottom alignment (dsiuta)
      self._bot = bot
   def setTop(self, top):             ######Top alignment (dsiuta)
      self._top = top
   def setLeft(self, left):           ######Left alignment (dsiuta)
      self._left = left
   def setRight(self, right):         ######Right alignment (dsiuta)
      self._right = right
   #def setPad(self, pad):
   #   self._pad = pad

   # Public 
   # Call this to create a plot, saves to file
   def plot(self, data):
      self._plotCore(data)
      self._adjustAxes()
      self._legend(data, self._legNames)
      self._savePlot(data)
   # Call this to write text output
   def text(self, data):
      self._textCore(data)
   # Draws a map of the data
   def map(self, data):
      self._mapCore(data)
      #self._legend(data, self._legNames)
      self._savePlot(data)

   # Implement these methods
   def _plotCore(self, data):
      Common.error("This type does not plot")
   def _textCore(self, data):
      Common.error("This type does not output text")
   def _mapCore(self, data):
      Common.error("This type does not produce maps")

   # Helper functions
   def _getColor(self, i, total):
      return self.colors[i % len(self.colors)]
   def _getStyle(self, i, total, connectingLine=True):
      I = (i / len(self.colors)) % len(self.lines)
      line   = self.lines[I]
      marker = self.markers[I]
      if(connectingLine):
         return line + marker
      return marker
   # Saves to file, set figure size
   def _savePlot(self, data):
      if(self._figsize != None):
         mpl.gcf().set_size_inches(int(self._figsize[0]), int(self._figsize[1]))
      if(not self._showMargin):
         Common.removeMargin()
      if(self._filename != None):
         mpl.savefig(self._filename, bbox_inches='tight')
      else:
         fig = mpl.gcf()
         fig.canvas.set_window_title(data.getFilenames()[0])
         mpl.show()
   def _legend(self, data, names=None):
      if(names == None):
         mpl.legend(loc="best",prop={'size':self._legfs})
      else:
         mpl.legend(names, loc="best",prop={'size':self._legfs})

   def _setYAxisLimits(self, metric):
      currYlim = mpl.ylim()
      ylim = [metric.min(), metric.max()]
      if(ylim[0] == None):
         ylim[0] = currYlim[0]
      if(ylim[1] == None):
         ylim[1] = currYlim[1]
      mpl.ylim(ylim)

   def _adjustAxes(self):
      # Apply adjustements to all subplots
      for ax in mpl.gcf().get_axes():
         # Tick font sizes
         for tick in ax.xaxis.get_major_ticks():
            tick.label.set_fontsize(self._tickfs) 
         for tick in ax.yaxis.get_major_ticks():
            tick.label.set_fontsize(self._tickfs) 
         ax.set_xlabel(ax.get_xlabel(), fontsize=self._labfs)
         ax.set_ylabel(ax.get_ylabel(), fontsize=self._labfs)
         #mpl.rcParams['axes.labelsize'] = self._labfs

         # Tick lines
         if(len(mpl.yticks()[0]) >= 2 and len(mpl.xticks()[0]) >= 2):
            # matplotlib crashes if there are fewer than 2 tick lines
            # when determining where to put minor ticks
            mpl.minorticks_on()
         if(not self._minlth == None):
            mpl.tick_params('both', length=self._minlth, which='minor')
         if(not self._majlth == None):
            mpl.tick_params('both', length=self._majlth, width=self._majwid, which='major')
         for label in ax.get_xticklabels():
            label.set_rotation(self._xrot)

      # Margins
      mpl.gcf().subplots_adjust(bottom=self._bot, top=self._top, left=self._left, right=self._right)

   def _plotObs(self, x, y, isCont=True):
      if(isCont):
         mpl.plot(x, y,  ".-", color=[0.3,0.3,0.3], lw=5, label="obs")
      else:
         mpl.plot(x, y,  "o", color=[0.3,0.3,0.3], ms=self._ms, label="obs")

class LinePlot(Output):
   def __init__(self, metric, binned=False):
      Output.__init__(self)
      # offsets, dates, location, locationElev, threshold
      self._binned = binned
      self._metric = metric
      if(metric.defaultAxis() != None):
         self._xaxis = metric.defaultAxis()

   def getXY(self, data):
      thresholds = self._thresholds
      axis = data.getAxis()

      lowerT = [-np.inf for i in range(0, len(thresholds))]
      upperT = thresholds
      if(self._binned):
         lowerT = thresholds[0:-1]
         upperT = thresholds[1:]

      if(axis == "threshold"):
         xx = upperT
         if(self._binned):
            xx = [(lowerT[i] + upperT[i])/2 for i in range(0, len(lowerT))]
      else:
         xx = data.getAxisValues()

      labels = data.getFilenames()
      F = data.getNumFiles()
      y = None
      x = None
      for f in range(0, F):
         data.setFileIndex(f)
         yy = np.zeros(len(xx), 'float')
         if(axis == "threshold"):
            for i in range(0, len(lowerT)):
               yy[i] = self._metric.compute(data, [lowerT[i], upperT[i]])
         else:
            for i in range(0, len(lowerT)):
               yy = yy + self._metric.compute(data, [lowerT[i], upperT[i]])
            yy = yy / len(thresholds)

         if(sum(np.isnan(yy)) == len(yy)):
            Common.warning("No valid scores for " + labels[f])
         if(y == None):
            y = np.zeros([F, len(yy)],'float')
            x = np.zeros([F, len(xx)],'float')
         y[f,:] = yy
         x[f,:] = xx
      return [x,y]

   def _plotCore(self, data):
      data.setAxis(self._xaxis)
      labels = data.getFilenames()
      F = data.getNumFiles()
      [x,y] = self.getXY(data)
      for f in range(0, F):
         color = self._getColor(f, F)
         style = self._getStyle(f, F, data.isAxisContinuous())
         alpha = (1 if(data.isAxisContinuous()) else 0.55)
         mpl.plot(x[f], y[f], style, color=color, label=labels[f], lw=self._lw, ms=self._ms, alpha=alpha)

      mpl.ylabel(self._metric.label(data))
      mpl.xlabel(data.getAxisLabel())
      mpl.gca().xaxis.set_major_formatter(data.getAxisFormatter())
      mpl.grid()
      self._setYAxisLimits(self._metric)

   def _textCore(self, data):
      thresholds = self._thresholds

      data.setAxis(self._xaxis)

      # Set configuration names
      if(self._legNames != None):
         names = self._legNames
      else:
         names = data.getShortNames()

      F     = data.getNumFiles()
      [x,y] = self.getXY(data)

      if(self._filename != None):
         sys.stdout = open(self._filename, 'w')

      maxlength = 0
      for name in names:
         maxlength = max(maxlength, len(name))
      maxlength = str(maxlength)

      # Header line
      fmt = "%-"+maxlength+"s"
      lineDesc = data.getAxisDescriptionHeader()
      lineDescN = len(lineDesc) + 2
      lineDescFmt = "%-" + str(lineDescN) + "s |"
      print lineDescFmt % lineDesc,
      descs = data.getAxisDescriptions()
      for name in names:
         print fmt % name,
      print ""

      # Loop over rows
      for i in range(0, len(x[0])):
         print lineDescFmt % descs[i],
         self._printLine(y[:,i], maxlength, "float")

      # Print stats
      for func in [Common.nanmin, Common.nanmean, Common.nanmax, Common.nanstd]:
         name = func.__name__[3:]
         print lineDescFmt % name,
         values = np.zeros(F, 'float')
         for f in range(0,F):
            values[f] = func(y[f,:])
         self._printLine(values, maxlength, "float")

      # Print count stats
      for func in [Common.nanmin, Common.nanmax]:
         name = func.__name__[3:]
         print lineDescFmt % ("num " + name),
         values = np.zeros(F, 'float')
         for f in range(0,F):
            values[f] = np.sum(y[f,:] == func(y,axis=0))
         self._printLine(values, maxlength, "int")

   def _printLine(self, values , colWidth, type="float"):
      if(type == "int"):
         fmt  = "%-"+colWidth+"i"
      else:
         fmt     = "%-"+colWidth+".2f"
      missfmt = "%-"+colWidth+"s" 
      minI    = np.argmin(values)
      maxI    = np.argmax(values)
      for f in range(0, len(values)):
         value = values[f]
         if(np.isnan(value)):
            txt = missfmt % "--"
         else:
            txt = fmt % value
         if(minI == f):
            print Common.green(txt),
         elif(maxI == f):
            print Common.red(txt),
         else:
            print txt,
      print ""


   def _mapCore(self, data):
      from mpl_toolkits.basemap import Basemap
      data.setAxis("location")
      labels = data.getFilenames()
      F = data.getNumFiles()
      lats = data.getLats()
      lons = data.getLons()
      ids  = data.getLocationIds()
      dlat = (max(lats) - min(lats))
      dlon = (max(lons) - min(lons))
      llcrnrlat= max(-90, min(lats) - dlat/10)
      urcrnrlat= min(90, max(lats) + dlat/10)
      llcrnrlon= min(lons) - dlon/10
      urcrnrlon= max(lons) + dlon/10
      res = Common.getMapResolution(lats, lons)
      dx = pow(10,np.ceil(np.log10(max(lons) - min(lons))))/10
      dy = pow(10,np.ceil(np.log10(max(lats) - min(lats))))/10
      names = data.getFilenames()
      [x,y] = self.getXY(data)

      # Colorbar limits should be the same for all subplots
      clim = [np.nanmin(y), np.nanmax(y)]

      for f in range(0, F):
         Common.subplot(f,F)
         map = Basemap(llcrnrlon=llcrnrlon,llcrnrlat=llcrnrlat,urcrnrlon=urcrnrlon,urcrnrlat=urcrnrlat,projection='mill', resolution=res)
         map.drawcoastlines(linewidth=0.25)
         map.drawcountries(linewidth=0.25)
         map.drawmapboundary()
         #map.drawparallels(np.arange(-90.,120.,dy),labels=[1,0,0,0])
         #map.drawmeridians(np.arange(0.,420.,dx),labels=[0,0,0,1])
         map.fillcontinents(color='coral',lake_color='aqua', zorder=-1)

         x0, y0 = map(lons, lats)
         I = np.where(np.isnan(y[f,:]))[0]
         map.plot(x0[I], y0[I], 'kx')

         if(1):
            isMax = y[f,:] == np.amax(y,0)
            isMin = y[f,:] == np.amin(y,0)
            s = 40 + 40*isMax - 30*isMin
            map.scatter(x0, y0, c=y[f,:], s=s)#, linewidths = 1 + 2*isMax)
         else:
            Imax = np.where(y[f,:] == np.amax(y,0))
            Imin = np.where(y[f,:] == np.amin(y,0))
            Iother = np.where(np.amax(y,0) == np.amin(y,0))[0]
            map.scatter(x0[Imax], y0[Imax], c=y[f,Imax], s=40)#, linewidths = 1 + 2*isMax)
            map.scatter(x0[Imin], y0[Imin], c=y[f,Imin], s=10)#, linewidths = 1 + 2*isMax)
            if(len(Iother)>0):
               map.scatter(x0[Iother], y0[Iother], c=y[f,Iother], marker='x', s=10)#, linewidths = 1 + 2*isMax)
         if(len(x0) < 100):
            for i in range(0,len(x0)):
               #mpl.text(x0[i], y0[i], "(%d,%d)" % (i,locs[i]))
               value = y[f,i]
               #if(value == max(y[:,i])):
               #   mpl.plot(x0[i], y0[i], 'ko', mfc=None, mec="k", ms=10)
                  
               if(not np.isnan(value)):
                  #if(isMax[i]):
                  #   mpl.plot(x0[i], y0[i], 'w.', ms=30, alpha=0.2)
                  mpl.text(x0[i], y0[i], "%d %3.2f" % (ids[i],value))
         cb = map.colorbar()
         cb.set_label(data.getVariableAndUnits())
         mpl.title(names[f])
         cb.set_clim(clim)
         mpl.clim(clim)

      l1 = map.scatter(0,0, c="b", s=10)
      l2 = map.scatter(0,0, c="b", s=20)
      l3 = map.scatter(0,0, c="b", s=40)
      l4 = map.scatter(0,0, c='k', marker="x")
      lines = [l1,l2,l3,l4]
      names = ["min", "regular", "max", "missing"]
      mpl.figlegend(lines, names, "lower center", ncol=4)

class ObsFcst(Output):
   _supThreshold = False
   _description = "Plot observations and forecasts"
   def __init__(self):
      Output.__init__(self)
   def _plotCore(self, data):
      F = data.getNumFiles()
      data.setAxis(self._xaxis)
      x = data.getAxisValues()

      isCont = data.isAxisContinuous()

      # Obs line
      mObs  = Metric.Mean("obs")
      y = mObs.compute(data, None)
      self._plotObs(x, y, isCont)

      mFcst = Metric.Mean("fcst")
      labels = data.getFilenames()
      for f in range(0, F):
         data.setFileIndex(f)
         color = self._getColor(f, F)
         style = self._getStyle(f, F, isCont)

         y = mFcst.compute(data, None)
         mpl.plot(x, y, style, color=color, label=labels[f], lw=self._lw,
               ms=self._ms)
      mpl.ylabel(data.getVariableAndUnits())
      mpl.xlabel(data.getAxisLabel())
      mpl.grid()
      mpl.gca().xaxis.set_major_formatter(data.getAxisFormatter())

class QQ(Output):
   _supThreshold = False
   _supX = False
   _description = "Quantile-quantile plot of obs vs forecasts"
   def __init__(self):
      Output.__init__(self)
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
      data.setAxis("all")
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
      ylim = list(mpl.ylim())
      xlim = list(mpl.xlim())
      axismax = max(max(ylim),max(xlim))
      mpl.plot([0,axismax], [0,axismax], "--", color=[0.3,0.3,0.3], lw=3, zorder=-100)
      mpl.grid()
   def _textCore(self, data):
      data.setAxis("all")
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
            if(len(x[f]) < i):
               print " --  -- "
            else:
               print fmt % (x[f][i], y[f][i]),
         print "\n",

class Scatter(Output):
   _description = "Scatter plot of forecasts vs obs"
   _supThreshold = False
   _supX = False
   def __init__(self):
      Output.__init__(self)
   def _plotCore(self, data):
      data.setAxis("all")
      data.setIndex(0)
      labels = data.getFilenames()
      F = data.getNumFiles()
      for f in range(0, F):
         data.setFileIndex(f)
         color = self._getColor(f, F)
         style = self._getStyle(f, F, connectingLine=False)

         [x,y] = data.getScores(["obs","fcst"])
         mpl.plot(x,y, ".", color=color, label=labels[f], lw=self._lw,
               ms=self._ms, alpha=0.2)
      mpl.ylabel("Forecasts (" + data.getUnits() + ")")
      mpl.xlabel("Observations (" + data.getUnits() + ")")
      ylim = mpl.ylim()
      xlim = mpl.xlim()
      axismax = max(max(ylim),max(xlim))
      mpl.plot([0,axismax], [0,axismax], "--", color=[0.3,0.3,0.3], lw=3, zorder=-100)
      mpl.grid()

class Cond(Output):
   _description = "Plots forecasts as a function of obs (use -r to specify bin-edges)"
   _defaultAxis = "threshold"
   _reqThreshold = True
   _supThreshold = True
   _supX = False
   def __init__(self, binned):
      Output.__init__(self)
      self._binned = True#binned
   def supportsThreshold(self):
      return True
   def _plotCore(self, data):
      data.setAxis("all")
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

         of = np.zeros(len(x), 'float')
         fo = np.zeros(len(x), 'float')
         mof = Metric.Conditional("obs", "fcst") # F | O
         mfo = Metric.Conditional("fcst", "obs") # O | F
         for i in range(0, len(lowerT)):
            fo[i] = mfo.compute(data, [lowerT[i], upperT[i]])
            of[i] = mof.compute(data, [lowerT[i], upperT[i]])
         mpl.plot(x,of, style, color=color, label=labels[f] + " (F|O)", lw=self._lw, ms=self._ms)
         mpl.plot(fo, x, style, color=color, label=labels[f] + " (O|F)", lw=self._lw, ms=self._ms, alpha=0.5)
      mpl.ylabel("Forecasts (" + data.getUnits() + ")")
      mpl.xlabel("Observations (" + data.getUnits() + ")")
      ylim = mpl.ylim()
      xlim = mpl.xlim()
      axismax = max(max(ylim),max(xlim))
      mpl.plot([0,axismax], [0,axismax], "--", color=[0.3,0.3,0.3], lw=3, zorder=-100)
      mpl.grid()

class Count(Output):
   _description = "Counts number of forecasts above or within thresholds (use -r to specify bin-edges). Use -binned to count number in bins, instead of number above each threshold."
   _defaultAxis = "threshold"
   _reqThreshold = True
   _supThreshold = True
   _supX = False
   def __init__(self, binned):
      Output.__init__(self)
      self._binned = binned
   def _plotCore(self, data):
      data.setAxis("all")
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

         Nobs = np.zeros(len(x), 'float')
         Nfcst = np.zeros(len(x), 'float')
         obs  = Metric.Count("obs")
         fcst = Metric.Count("fcst")
         for i in range(0, len(lowerT)):
            Nobs[i] = obs.compute(data, [lowerT[i], upperT[i]])
            Nfcst[i] = fcst.compute(data, [lowerT[i], upperT[i]])
         if(f == 0):
            self._plotObs(x, Nobs)
         mpl.plot(x,Nfcst, style, color=color, label=labels[f], lw=self._lw, ms=self._ms)
      mpl.ylabel("Number")
      mpl.xlabel(data.getAxisLabel())
      mpl.grid()

class TimeSeries(Output):
   _description = "Plot observations and forecasts as a time series (i.e. by concatinating all offsets). '-x <dimension>' has no effect, as it is always shown by date."
   _supThreshold = False
   _supX = False
   def __init__(self):
      Output.__init__(self)
   def _plotCore(self, data):
      F = data.getNumFiles()
      data.setAxis("none")
      dates = data.getAxisValues("date")
      offsets = data.getAxisValues("offset")

      # Connect the last offset of a day with the first offset on the next day
      # This only makes sense if the obs/fcst don't span more than a day
      connect = max(offsets) < 24

      # Obs line
      obs = data.getScores("obs")[0]
      for d in range(0,obs.shape[0]):
         x = dates[d] + offsets/24.0
         y = Common.nanmean(obs[d,:,:], axis=1)
         if(connect and d < obs.shape[0]-1):
            x = np.insert(x,x.shape[0],dates[d+1])
            y = np.insert(y,y.shape[0],Common.nanmean(obs[d+1,0,:], axis=0))
         lab = "obs" if d == 0 else ""
         mpl.rcParams['ytick.major.pad']='20'    ######This changes the buffer zone between tick labels and the axis. (dsiuta)
         #mpl.rcParams['ytick.major.pad']='${self._pad}'
         #mpl.rcParams['xtick.major.pad']='${self._pad}'
         mpl.rcParams['xtick.major.pad']='20'    ######This changes the buffer zone between tick labels and the axis. (dsiuta)
         mpl.plot(x, y,  ".-", color=[0.3,0.3,0.3], lw=5, label=lab)

         # Forecast lines
         labels = data.getFilenames()
         for f in range(0, F):
            data.setFileIndex(f)
            color = self._getColor(f, F)
            style = self._getStyle(f, F)

            fcst = data.getScores("fcst")[0]
            x = dates[d] + offsets/24.0
            y = Common.nanmean(fcst[d,:,:], axis=1)
            if(connect and d < obs.shape[0]-1):
               x = np.insert(x,x.shape[0],dates[d+1])
               y = np.insert(y,y.shape[0],Common.nanmean(fcst[d+1,0,:]))
            lab = labels[f] if d == 0 else ""
            mpl.rcParams['ytick.major.pad']='20'  ######This changes the buffer zone between tick labels and the axis. (dsiuta)
            mpl.rcParams['xtick.major.pad']='20'    ######This changes the buffer zone between tick labels and the axis. (dsiuta)
            #mpl.rcParams['ytick.major.pad']='${self._pad}'
            #mpl.rcParams['xtick.major.pad']='${self._pad}'
            mpl.plot(x, y,  style, color=color, lw=self._lw, ms=self._ms, label=lab)

      mpl.ylabel(data.getVariableAndUnits())  # "Wind Speed (km/hr)") ###hard coded axis label (dsiuta)
      mpl.xlabel(data.getAxisLabel("date"))
      mpl.grid()
      mpl.gca().xaxis.set_major_formatter(data.getAxisFormatter("date"))
    
class PitHist(Output):
   _description = "Histogram of PIT values"
   _supThreshold = False
   _supX = False
   def __init__(self, metric):
      Output.__init__(self)
      self._numBins = 10
      self._metric = metric
   def _legend(self, data,names=None):
      pass
   def _plotCore(self, data):
      F = data.getNumFiles()
      labels = data.getFilenames()
      for f in range(0, F):
         Common.subplot(f,F)
         color = self._getColor(f, F)
         data.setAxis("all")
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
         mpl.title(labels[f]);
         ytop = 200.0/self._numBins
         mpl.gca().set_ylim([0,ytop])
         if(f == 0):
            mpl.ylabel("Frequency (%)")
         else:
            mpl.gca().set_yticks([])
         #self._setYAxisLimits(self._metric)

         mpl.xlabel("Cumulative probability")

class Reliability(Output):
   _description = "Reliability diagram for a certain threshold (-r)"
   _reqThreshold = True
   _supX = False
   def __init__(self):
      Output.__init__(self)
   def _plotCore(self, data):
      labels = data.getFilenames()

      threshold = self._thresholds[0]
      F = data.getNumFiles()
      ax  = mpl.gca()
      axi = mpl.axes([0.16,0.65,0.2,0.2])
      mpl.sca(ax)
      for f in range(0, F):
         color = self._getColor(f, F)
         style = self._getStyle(f, F)
         data.setAxis("all")
         data.setIndex(0)
         data.setFileIndex(f)
         var = data.getPvar(threshold)
         [obs, p] = data.getScores(["obs", var])

         # Determine the number of bins to use # (at least 11, at most 25)
         if(f == 0):
            N = min(25, max(11, int(len(obs)/1000)))
            edges = np.linspace(0,1,N+1)
            x  = np.linspace(0.5/N,1-0.5/N,N)

         p = 1 - p
         obs = obs > threshold

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

         #for i in range(0,len(edges)-1):
         #   ax.text(x[i], y[i], "%d" % n[i], horizontalalignment="center", verticalalignment="bottom")
         self.plotConfidence(x, y, n, color=color)
         axi.plot(x, n, style, color=color, lw=self._lw, ms=self._ms)
         axi.xaxis.set_major_locator(mpl.NullLocator())
      mpl.sca(ax)
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
      mpl.title("Threshold: " + str(threshold) + units)

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


# doClassic: Use the classic definition, by not varying the forecast threshold
#            i.e. using the same threshold for observation and forecast.
class DRoc(Output):
   _description = "Plots the receiver operating characteristics curve for the deterministic " \
         + "forecast for a single threshold. Uses different forecast thresholds to create points."
   _supX = False
   _reqThreshold = True
   def __init__(self, fthresholds=None, doNorm=False, doClassic=False):
      Output.__init__(self)
      self._doNorm = doNorm
      self._fthresholds = fthresholds
      self._doClassic = doClassic
   def _plotCore(self, data):
      threshold = self._thresholds[0]   # Observation threshold
      if(threshold == None):
         Common.error("DRoc plot needs a threshold (use -r)")

      if(self._doClassic):
         fthresholds = [threshold]
      else:
         if(self._fthresholds != None):
            fthresholds = self._fthresholds
         else:
            if(data.getVariable() == "Precip"):
               fthresholds = [0,1e-5,0.001,0.005,0.01,0.05,0.1,0.2,0.3,0.5,1,2,3,5,10,20,100]
            else:
               N = 31
               fthresholds = np.linspace(threshold-10, threshold+10, N)

      F = data.getNumFiles()
      labels = data.getFilenames()
      for f in range(0, F):
         color = self._getColor(f, F)
         style = self._getStyle(f, F)
         data.setAxis("all")
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
               if(not np.isnan(x[i]) and not np.isnan(y[i]) and f == 0):
                  mpl.text(x[i], y[i], "%2.1f" % fthreshold, color=color)
         #I = np.where(np.isnan(x)+np.isnan(y)==0)
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
      mpl.title("Threshold: " + str(threshold) + units)
      mpl.grid()

class DRocNorm(DRoc):
   _description = "Same as DRoc, except the hit and false alarm rates are transformed using the " \
            "inverse of the standard normal distribution in order to highlight the extreme " \
            "values." 
   def __init__(self):
      DRoc.__init__(self, doNorm=True)

class DRoc0(DRoc):
   _description = "Same as DRoc, except don't use different forecast thresholds: Use the "\
      "same\n threshold for forecast and obs."
   def __init__(self):
      DRoc.__init__(self, doNorm=False, doClassic=True)

class Against(Output):
   _description = "Plots the forecasts for each pair of configurations against each other. Colours indicate which "\
   "configuration had the best forecast (but only if the difference is more than 10% of the standard deviation of the"\
         "observation)."
   _experimental = True
   _defaultAxis = "all"
   _supThreshold = False
   _supX = False
   _minStdDiff = 0.1 # How big difference should colour kick in (in number of STDs)?
   def _plotCore(self, data):
      F = data.getNumFiles()
      if(F < 2):
         Common.error("Cannot use Against plot with less than 2 configurations")

      data.setAxis("all")
      data.setIndex(0)
      labels = data.getFilenames()
      for f0 in range(0,F):
         for f1 in range(0,F):
            if(f0 != f1 and (F != 2 or f0 == 0)):
               if(F > 2):
                  mpl.subplot(F,F,f0+f1*F+1)
               data.setFileIndex(f0)
               x = data.getScores("fcst")[0].flatten()
               data.setFileIndex(f1)
               y = data.getScores("fcst")[0].flatten()
               lower = min(min(x),min(y))
               upper = max(max(x),max(y))
               mpl.plot(x, y, "s", mec="k", ms=self._ms/2, mfc="w", zorder=-1000)

               # Show which forecast is better
               data.setFileIndex(f0)
               [obsx,x] = data.getScores(["obs","fcst"])
               data.setFileIndex(f1)
               [obsy,y] = data.getScores(["obs","fcst"])
               x = x.flatten()
               y = y.flatten()
               obs = obsx.flatten()

               minDiff = self._minStdDiff*np.std(obs)
               if(len(x) == len(y)):
                  Ix = abs(obs - y) > abs(obs - x) + minDiff
                  Iy = abs(obs - y) + minDiff < abs(obs - x) 
                  mpl.plot(x[Ix], y[Ix], "r.", ms=self._ms, alpha=0.5)
                  mpl.plot(x[Iy], y[Iy], "b.", ms=self._ms, alpha=0.5)

               # Contour of the frequency
               #q = np.histogram2d(x[1,:], x[0,:], [np.linspace(lower,upper,100), np.linspace(lower,upper,100)])
               #[X,Y] = np.meshgrid(q[1],q[2])
               #mpl.contour(X[1:,1:],Y[1:,1:],q[0],[1,100],zorder=90)

               mpl.xlabel(labels[f0], color="r")
               mpl.ylabel(labels[f1], color="b")
               mpl.grid()
               xlim = mpl.xlim()
               ylim = mpl.ylim()
               lower = min(xlim[0],ylim[0])
               upper = max(xlim[1],ylim[1])
               mpl.xlim([lower, upper])
               mpl.ylim([lower, upper])
               mpl.plot([lower,upper], [lower, upper], '--', color=[0.3,0.3,0.3], lw=3, zorder=100)
               if(F == 2):
                  break
   def _legend(self, data, names=None):
      pass
