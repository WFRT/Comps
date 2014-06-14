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

def red(text):
   return "\033[31m"+text+"\033[0m"

def green(text):
   return "\033[32m"+text+"\033[0m"

def error(message):
   print "\033[1;31mError: " + message + "\033[0m"
   sys.exit(1)

def warning(message):
   print "\033[1;33mWarning: " + message + "\033[0m"

def getSubplotSize(N):
   x = np.ceil(np.sqrt(N))
   y = np.ceil(float(N) / x)
   return [int(x),int(y)]

# allowable formats:
# num
# num1,num2,num3
# start:end
# start:step:end
def parseNumbers(numbers):
   values = list()
   commaLists = numbers.split(',')
   for commaList in commaLists:
      colonList = commaList.split(':')
      if(len(colonList) == 1):
         values.append(float(colonList[0]))
      elif(len(colonList) <= 3):
         start = float(colonList[0])
         step  = 1
         if(len(colonList) == 3):
            step = float(colonList[1])
         end   = float(colonList[-1]) + 0.0001 # arange does not include the end point
         values = values + list(np.arange(start, end, step))
      else:
         error("Could not parse '" + colonList + "'")
   return values

def testParseNumbers():
   print parseNumbers("1,2,3:5,6,7:2:20")
   print parseNumbers("1")
