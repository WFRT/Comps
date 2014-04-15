import datetime
import numpy as np
import sys
from matplotlib.dates import *
def convertDates(dates):
   numDates = len(dates)
   dates2 = np.zeros([numDates], 'float')   
   for i in range(0, numDates):
      year = int(dates[i] / 10000)
      month = int(dates[i] / 100 % 100)
      day = int(dates[i] % 100)
      dates2[i] = date2num(datetime.datetime(year, month, day, 0))
   return dates2

def error(message):
   print "\033[1;31mError: " + message + "\033[0m"
   sys.exit(1)

def warning(message):
   print "\033[1;33mWarning: " + message + "\033[0m"
