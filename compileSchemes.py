#!/usr/bin/python
import re
import os
import numpy as np

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

srcDir = "../compsDev/src/"

components = [
"Averagers",
"Calibrators",
"Continuous",
"Correctors",
"Discretes",
"Downscalers",
"Selectors",
"Inputs",
"Loggers",
"Outputs",
"Interpolators",
"Measures",
"Metrics",
"Qcs",
"Variables",
"Estimators",
"ParameterIos",
"Regions"
]
type = ["","","","","","","","io","io","io","helper","helper","helper","helper","helper","param","param","param"]
helperStartIndex = 9
#components = ["Inputs"]
defaultComponent = "Correctors"

fileo = "schemes/index.html"
fo = open(fileo, "w")
fo.write("---\n")
fo.write("layout: scheme\n")
fo.write("---\n")
fo.write('<div class="tabbable">\n')
fo.write('   <ul class="nav nav-pills">\n')
counter = 0
for comp in components:
   classTag = ""
   extra    = ""
   #if(counter == helperStartIndex):
   #   fo.write('      <li class="break">Helpers</li>')
   if(comp == defaultComponent):
      classTag = ' class="active"'
#   elif(comp == "Variables"):
#      extra = '<i class="icon-file"></i> '
   classTag = ''
   #if(counter == helperStartIndex):
   #  fo.write('      <li class="break">&nbsp;</li>\n')
   #  fo.write('      <li class="schemeCategory">Helpers</li>\n')
   if(type[counter] != "") :
      classTag = ' class="' + type[counter] + '"'
   fo.write('      <li' + classTag + '>\n')
   fo.write('         <a href="#tab' + comp + '" data-toggle="tab">' + extra + comp + '</a> \n')
   fo.write('      </li>\n')
   counter = counter + 1
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
   fo.write('         <h2>' + comp + '</h2>\n')
   for file in files:
      # Open a file
      filec = srcDir + comp + "/" + file + ".cpp"
      fileh = srcDir + comp + "/" + file + ".h"
      fileo = "auto/" + comp + "_" + file + ".tex"
      f = open(filec, "r")

      names = []     # Attribute in namelist
      vars = []      # mVariable
      desc = []      # Description
      types = []     # bool, int, ec
      req = []       # is required
      vec = []       # is vector?
      defs = []      # Default values (TODO)
      lastComment = ""
      start = 0
      cases = ["Value", "Values", "RequiredValue", "RequiredValues"]
      creq  = [0, 0, 1, 1]
      cvec  = [0, 1, 0, 1]
      while(True):
         line = f.readline()
         if(line == ""):
            break;
         m = re.search('//!(.*)', line)
         if(m != None):
            start = 1
            comment = m.group(1)
            lastComment = lastComment + comment
         else:
            for i in range(0,len(cases)):
               c = cases[i]
               m = re.search('Options.get' + c + '\("(\w+)",\s*(\w+)\)', line)
               if(m != None):
                  names.append(m.group(1))
                  vars.append(m.group(2))
                  desc.append(lastComment)
                  req.append(creq[i])
                  vec.append(cvec[i])
            start = 0
            lastComment = ""

      for var in vars:
         types.append("")
         desc.append("")

      f.close()

      # Second pass finding types
      # Check both header and implementation file. Type is normally in header file, but might be
      # in implementation file if it is a local temporary variable
      underDevelopment = False
      for f0 in [fileh, filec]:
         f = open(f0, "r")
         while(True):
            line = f.readline()
            if(line == ""):
               break;
            # Check if the scheme is under development
            m = re.search('underDevelopment()', line)
            if(m != None):
               underDevelopment = True

            # Find a declaration of the variable, e.g.:
            #    std::vector<float> mVariable;
            #    bool mVariable;
            m = re.search('^\s*([\w:<>]+)\s+(\w+);', line)
            if(m != None):
               try:
                  type = m.group(1)
                  var  = m.group(2)
                  for i in range(0,len(vars)):
                     if(types[i] == "" and vars[i] == var):
                        types[i] = type
               except IndexError:
                  print "nothing"
         f.close()

      # Third pass finding scheme description
      f = open(fileh, "r")
      classDesc = ""
      className = ""
      author = ""
      date = ""
      while(True):
         line = f.readline()
         if(line == ""):
            break;

         m0 = re.search('class\s+([^\s]+) ', line)
         if(m0 != None):
            className= m0.group(1)

         m0 = re.search('//!\s*@author\s*(.+)', line)
         if(m0 != None):
            author = m0.group(1)

         m1 = re.search('//!\s*@date\s*(\w+)', line)
         if(m1 != None):
            date = m1.group(1)

         if(m0 == None and m1 == None):
            m = re.search('//!(.*)', line)
            if(m != None):
               comment = m.group(1)
               classDesc = classDesc + comment
            else:
               m = re.search('class .*' + file, line)
               if(m != None):
                  break
               else:
                  classDesc = ""
      f.close()

      devel = ""
      if(underDevelopment):
         devel = " (under development)"

      # Write to file
      fo.write('         <div class="row">\n')
      fo.write('            <div class="span12">\n')
      if(not isAbstract(file, comp)):
         fo.write("               <h4>" + file + devel + " <code>" + className + "</code></h4>\n")
      #if(author != ""):
      #   fo.write("               <br><b>Author:</b> " + author + '\n')
      #if(date != ""):
      #   fo.write("               <b>Date:</b> " + formatDate(date)+ '\n')
      fo.write("               <p>" + classDesc + "</p>\n")
      fo.write("            </div>\n")
      fo.write('            <div class="span12">\n')
      if(len(names)>0):
         if(isAbstract(file, comp)):
            fo.write('               The following attributes are inherited by all ' + comp[:-1] + ' schemes:\n')
         fo.write('               <table class="table table-striped table-condensed">\n')
         fo.write("                  <thead>\n")
         fo.write('                     <tr><th width="20%">Attribute</th><th width="20%">Type</th><th>Description</th></tr>\n')
         fo.write("                  </thead>\n")
         for i in range(0, len(names)):
            if(vec[i] == 1):
               m = re.search("std::vector<([\w:]+)>", types[i])
               type = m.group(1) + "[]"
            else:
               type = types[i]
            if(req[i] == 1):
               #name = names[i] + "*"
               name = '<i class="icon-circle"></i> ' + names[i] + ''
            else:
               name = '<i class="icon-circle-blank"></i> ' + names[i] + ''
            fo.write('                  <tr><td width="25%">' + name + '</td><td width="15%">' + type + '</td><td>' + desc[i] + "</td></tr>\n")
         fo.write("               </table>\n")
      fo.write('            </div>\n')
      fo.write('         </div>\n')
   fo.write('      </div>\n')
   counter = counter + 1

fo.write('   </div>\n')
fo.write('</div>\n')
fo.close()
