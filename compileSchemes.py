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

def writeDescription(comp):
   descFile = "_includes/components/" + comp + ".html"
   if(os.path.isfile(descFile)):
      print "Found description file: " + descFile
      fo.write('            <div class="panel panel-info">\n')
      fo.write('               <div class="panel-heading">\n')
      fo.write('                  <h4>Description</h4>\n')
      fo.write('               </div>\n')
      fo.write('               <div class="panel-body">\n')
      fo.write('{% include ' + 'components/' + comp + '.html' + ' %}\n')
      fo.write('               </div>\n')
      fo.write('            </div>\n')
   else:
      print "Did not find: " + descFile

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

srcDir = "../compsAuto/src/"

components = [
"Inputs",
"Variables",
"Qcs",
"Downscalers",
"Spreaders",
"Poolers",
"ParameterIos",
"Selectors",
"Correctors",
"Continuous",
"Discretes",
"Calibrators",
"Outputs",
"Averagers",
"BaseDistributions",
"Interpolators",
"Neighbourhoods",
"Measures",
"Metrics",
"Transforms"
]
starts = [1,4,13]
ends   = [3,6,19]
types  = ["Data", "Parameters", "Helpers"]
#components = ["Inputs"]
defaultComponent = "Inputs"
colours = ["colRed", "red", "red", "red", "red", "red", "red", "red", "red", "red", "red", "red", "red", "red", "red", "red",
"red", "red", "red", "red",]

fileo = "components/index.html"
fo = open(fileo, "w")
fo.write("---\n")
fo.write("layout: components\n")
fo.write("---\n")
fo.write('<div class="tabbable">\n')
fo.write('   <ul class="nav nav-pills">\n')
counter = 0
catCounter = 0
inDropdown = False
for i in range(0,len(components)):
   comp   = components[i]
   colour = colours[i]
   extra  = ""
   if(catCounter < len(starts) and counter == starts[catCounter]):
      cat = types[catCounter]
      fo.write('      <li class="dropdown io">')
      fo.write('         <div class="btn-group">')
      fo.write('            <button href="#tab' + cat + '" data-toggle="tab" type="button" class="btn btn-default">' + types[catCounter] + '</button>')
      fo.write('            <button class="btn btn-default dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></button>')
      fo.write('            <ul class="dropdown-menu">')
      catCounter = catCounter + 1
      inDropdown = True
   classTag = 'class="'
   if(comp == defaultComponent):
      classTag = classTag + ' active'
   classTag = classTag + '"'
   fo.write('      <li ' + classTag + '>\n')
   if(inDropdown):
      fo.write('         <a href="#tab' + comp + '" data-toggle="tab">' + extra + comp + '</a> \n')
   else:
      fo.write('         <button href="#tab' + comp + '" data-toggle="tab" type="button" class="btn btn-default">' + extra + comp + '</button> \n')
   fo.write('      </li>\n')
   if(counter == ends[catCounter-1]):
      fo.write('   </ul>\n')
      fo.write('   </div></li>\n')
      inDropdown = False
   counter = counter + 1
fo.write('   </ul>\n')
fo.write('   </li>\n')
fo.write('   </ul>\n')
fo.write('   <div class="tab-content">\n')

counter = 0
for comp in types:
   fo.write('      <div class="tab-pane" id="tab' + comp + '">\n')
   fo.write('&nbsp;')
   fo.write('         <div class="row">\n')
   fo.write('            <div class="col-md-12">\n')
   writeDescription(comp)
   fo.write('            </div>\n')
   fo.write('         </div>\n')
   fo.write('      </div>\n')
for comp in components:
   files = [f for f in os.listdir(srcDir + comp) if re.match(r'.*\.h$', f)]
   #files = ["Logit.h"]
   removeh(files)
   sort(files, comp)

   classTag = ""
   if(comp == defaultComponent):
      classTag = ' active'
   fo.write('      <div class="tab-pane' + classTag + '" id="tab' + comp + '">\n')
   fo.write('&nbsp;')

   # Links to each scheme
   fo.write("<h4>List of schemes:")
   for f in range(0, len(files)):
      file = files[f]
      url = "#scheme" + file
      fo.write(" <a href='" + url + "'>" + file + "</a>")
      if( f < len(files)-1):
         fo.write(",")
   fo.write("</h4>")

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
      fo.write('         <a id="scheme' + file + '"></a>\n')
      fo.write('         <div class="row">\n')
      fo.write('            <div class="col-md-12">\n')
      writeDescription(file)
      fo.write('            </div>\n')
      fo.write('         </div>\n')
      fo.write('         <div class="row">\n')
      fo.write('            <div class="col-md-12">\n')
      fo.write(classFile.getHtml(linkToTop=True))
      fo.write('            </div>\n')
      fo.write('         </div>\n')
   fo.write('      </div>\n')
   counter = counter + 1

fo.write('   </div>\n')
fo.write('</div>\n')
fo.close()
