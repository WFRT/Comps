from File import *
from Plot import *
import sys
import matplotlib.pyplot as mpl
ifile = ""
location = ""
ofile = ""
#ofiledir = ""

i = 1
while(i < len(sys.argv)):
   arg = sys.argv[i]
   if(arg[0] == '-'):
      # Process option
      if(arg == "-o"):
         ofile = sys.argv[i+1]
      #elif(arg == "-O"):
      #   ofiledir = sys.argv[i+1]
      i = i + 1
   else:
      if(ifile == ""):
         ifile = sys.argv[i]
      elif(location == ""):
         location = int(sys.argv[i])
      elif(ofile == ""):
         ofile = sys.argv[i]
   i = i + 1
if(ifile == ""):
   error("Needs input file")
if(location == ""):
   error("Needs location")

file = NetCdfFile(ifile,location=location)
pl = CdfPlot(file)

pl.plot(mpl.gca())
#mpl.subplots_adjust(left=0, bottom=0, right=1, top=1, wspace=None, hspace=None)

if(ofile != ""):
   mpl.savefig(ofile, bbox_inches='tight')
else:
   mpl.show()
