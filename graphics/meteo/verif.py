from File import *
from Plot import *
#from Metric import *
import sys
import matplotlib.pyplot as mpl
ifile    = ""
ofile    = ""
metric   = ""
location = np.nan
training = 0
i        = 1
threshold = 0
offset = np.nan
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
         threshold = int(sys.argv[i+1])
      elif(arg == "-o"):
         offset = int(sys.argv[i+1])
      elif(arg == "-m"):
         metric = sys.argv[i+1]
      i = i + 1
   else:
      ifile = sys.argv[i]
   i = i + 1
if(ifile == ""):
   error("Needs input file")
if(metric == ""):
   error("Needs metric (-m)")

file = VerifFile(ifile,training=training,location=location,offset=offset)
metric = metric.lower()
if(metric == "pit"):
   pl = PitPlot(file)
elif(metric == "reliability"):
   pl = ReliabilityPlot(file, threshold=threshold)
elif(metric == "obs"):
   pl = ObsPlot(file)
elif(metric == "ets"):
   pl = EtsPlot(file)
elif(metric == "spreadskill"):
   pl = SpreadSkillPlot(file)
elif(metric == "hitrate"):
   pl = HitRatePlot(file, threshold)
else:
   pl = VerifPlot(file, metric)

pl.plot(mpl.gca())

if(ofile != ""):
   mpl.savefig(ofile)
else:
   mpl.show()

