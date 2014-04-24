#!/usr/bin/python
import re
import os
import numpy as np
from ClassFile import *

def sort(files, comp):
   for i in range(0, len(files)):
      if(isAbstract(files[i], comp)):
         break
   temp = files[i]
   files.pop(i)
   files.insert(0, temp)

def removeh(files):
   for i in range(0, len(files)):
      files[i] = files[i][:-2]

def isAbstract(scheme, comp):
   return scheme == comp[:-1] or scheme == comp

def formatDate(date):
   year = date[0:4]
   month = date[4:6]
   day = date[6:8]
   return year + "/" + month + "/" + day

srcDir = "../comps/src/"

components = [
"Inputs",
"Selectors",
"Correctors",
"Continuous",
"Discretes",
"Calibrators",
"Outputs",
"Downscalers",
"Variables",
"Qcs",
"Poolers",
"Spreaders",
"ParameterIos",
"Averagers",
"BaseDistributions",
"Interpolators",
"Measures",
"Metrics",
"Transforms"
]
type = ["","","","","","","","io","io","io","helper","helper","helper","helper","helper","param","param","param"]
starts = [6,10,13]
types  = ["Data", "Parameters", "Helpers"]
#components = ["Inputs"]
defaultComponent = "Correctors"

fileo = "documentation/scheme.html"
fo = open(fileo, "w")
fo.write("---\n")
fo.write("layout: scheme\n")
fo.write("---\n")
fo.write('<div class="tabbable">\n')
fo.write('   <ul class="nav nav-pills">\n')
counter = 0
start   = 0
for comp in components:
   classTag = ""
   extra    = ""
   if(start < len(starts) and counter == starts[start]):
      if(start > 0):
         fo.write('   </ul>\n')
         fo.write('   </li>\n')
      fo.write('      <li class="dropdown io">')
      fo.write('         <a class="dropdown-toggle" data-toggle="dropdown" href="#">' + types[start])
      fo.write('         <b class="caret"></b></a>')
      fo.write('         <ul class="dropdown-menu">')
      start = start + 1
   classTag = ''
   if(comp == defaultComponent):
      classTag = ' class="active"'
   #if(type[counter] != "") :
   #   classTag = ' class="' + type[counter] + '"'
   fo.write('      <li' + classTag + '>\n')
   fo.write('         <a href="#tab' + comp + '" data-toggle="tab">' + extra + comp + '</a> \n')
   fo.write('      </li>\n')
   counter = counter + 1
fo.write('   </ul>\n')
fo.write('   </li>\n')
fo.write('   </ul>\n')
fo.write('   <div class="tab-content">\n')

counter = 0
for comp in components:
   files = [f for f in os.listdir(srcDir + comp) if re.match(r'.*\.h$', f)]
   #files = ["Logit.h"]
   removeh(files)
   sort(files, comp)

   classTag = ""
   if(comp == defaultComponent):
      classTag = ' active'
   fo.write('      <div class="tab-pane' + classTag + '" id="tab' + comp + '">\n')
   #fo.write('         <h2>' + comp + '</h2>\n')
   fo.write('&nbsp;')
   for file in files:
      # Open a file
      filec = srcDir + comp + "/" + file + ".cpp"
      fileh = srcDir + comp + "/" + file + ".h"

      classFile = ClassFile(file, fileh, filec)
      classFile.setIsAbstract(isAbstract(classFile.getClassName(), comp))

      devel = ""
      if(classFile.isUnderDevelopment()):
         devel = " (under development)"

      # Write to file
      fo.write('         <div class="row">\n')
      fo.write('            <div class="col-md-12">\n')
      fo.write(classFile.getHtml())
      fo.write('            </div>\n')
      fo.write('         </div>\n')
   fo.write('      </div>\n')
   counter = counter + 1

fo.write('   </div>\n')
fo.write('</div>\n')
fo.close()
