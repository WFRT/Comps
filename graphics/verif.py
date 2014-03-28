import sys
import os
sys.path.append(sys.path[0]+'/verif/')
from File import *
from Plot import *
from Data import *
import matplotlib.pyplot as mpl

def showDescription():
   print "Compute verification scores for COMPS verification files\n"
   print "usage: verif.py files -m metric [-x x-dim] [-l location] [-o offset] [-t training] [-r threshold] [-s subplot-dim]\n"
   print "Arguments:"
   print "   files:        One or more COMPS verification files in NetCDF format"
   print "   metric:       Verification score to use. Either a score found in the files, or a"
   print "                 derived score such as: ets, roc"
   print "   x-dim:        Plot this dimension on the x-axis: date, offset, location, file"
   print "   location:     Limit the verification to this location index"
   print "   offset:       Limit the verification to this offset index"
   print "   training:     Remove this many days from the verification"
   print "   threshold:    Compute scores for this threshold (only used by some metrics)"
   print "   subplot-dim:  Plot this dimension in separate subplots"

if(len(sys.argv) == 1):
   showDescription()
   sys.exit()

ifiles   = list()
ofile    = ""
metric   = ""
location = None
training = 0
i        = 1
threshold = 0
offset = None
xdim = None
sdim = None

# Read command line arguments
while(i < len(sys.argv)):
   arg = sys.argv[i]
   if(arg[0] == '-'):
      # Process option
      if(arg == "-f"):
         ofile = sys.argv[i+1]
      elif(arg == "-l"):
         location = int(sys.argv[i+1])
      elif(arg == "-t"):
         training = int(sys.argv[i+1])
      elif(arg == "-x"):
         xdim = sys.argv[i+1]
      elif(arg == "-o"):
         offset = int(sys.argv[i+1])
      elif(arg == "-s"):
         sdim = sys.argv[i+1]
      elif(arg == "-r"):
         threshold = int(sys.argv[i+1])
      elif(arg == "-m"):
         metric = sys.argv[i+1]
      i = i + 1
   else:
      ifiles.append(sys.argv[i])
   i = i + 1

metricOrig = metric
metric = metric.lower()

if(len(ifiles) == 0):
   error("Needs at least one input file")
if(metric == ""):
   print "No metric specified. Use one of:"
   file = File(ifiles[0])
   metrics = ['roc', 'spreadSkill', 'reliability', 'correlation', 'pit']
   metrics = metrics + file.getMetrics()
   for metric in metrics:
      print "   " + metric
   sys.exit(1)

# Determine which dates to use
files = list()
for ifile in ifiles:
   files.append(File(ifile))

dates = files[0].getDates()
for i in range(1, len(files)):
   dates = np.intersect1d(dates, files[i].getDates())

dates = dates[range(training, len(dates))]

if(metric == "reliability"):
   pl = ReliabilityPlot(threshold)
elif(metric == "pit"):
   pl = PitPlot()
elif(metric == "spreadskill"):
   pl = SpreadSkillPlot()
elif(metric == "ets"):
   pl = EtsPlot()
elif(metric == "roc"):
   pl = RocPlot(threshold)
elif(metric == "correlation"):
   pl = CorrelationPlot()
else:
   pl = DefaultPlot(metricOrig)
for file in files:
   data = Data(file, offset, location, dates)
   pl.add(data)
pl.plot(mpl.gca())
pl.legend(mpl.gca())
mpl.show()
