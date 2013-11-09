from File import *
from Plot import *
import sys
import matplotlib.pyplot as mpl
ifile = sys.argv[1]
ofile = ""
location = 0
if(len(sys.argv) >= 3):
   location = sys.argv[2]
if(len(sys.argv) == 4):
   ofile = sys.argv[3]

file = NetCdfFile(ifile,location=location)
pl = CdfPlot(file)

pl.plot(mpl.gca())

if(ofile != ""):
   mpl.savefig(ofile)
else:
   mpl.show()
