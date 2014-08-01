import re
import os
import numpy as np
from Option import *
class ClassFile:
   def __init__(self, name, fileh, filec):
      self.name = name
      self.fileh = fileh
      self.filec = filec

      # Scheme information
      self.underDevelopment = False
      self.className = ""
      self.author = ""
      self.date = ""
      self._isAbstract = False

      description = ""
      f = open(self.fileh, "r")
      lastComment = ""
      while(True):
         line = f.readline()
         if(line == ""):
            break;

         m0 = re.search('^[\w]*class\s+([^\s]+) ', line)
         if(m0 != None):
            self.className= m0.group(1)

         m0 = re.search('//!\s*@author\s*(.+)', line)
         if(m0 != None):
            self.author = m0.group(1)

         m1 = re.search('//!\s*@date\s*(\w+)', line)
         if(m1 != None):
            self.date = m1.group(1)

         if(m0 == None and m1 == None):
            m = re.search('//!(.*)', line)
            if(m != None):
               comment = m.group(1)
               lastComment = lastComment + comment
            else:
               m = re.search('class .*' + self.getName(), line)
               if(m != None):
                  description = lastComment
      f.close()
      self.description = description
      self.options     = self.loadOptions()

   @staticmethod
   def removeh(file):
      return file[:-2]

   def getOptions(self):
      return self.options

   def merge(self, classFile):
      opt = classFile.getOptions()
      self.options = self.options + opt

   def getName(self):
      return self.name

   def getClassName(self):
      return self.className
      # I = self.fileh.rfind('/') + 1
      # return self.fileh[I:-2]

   def isUnderDevelopment(self):
      return self.underDevelopment

   def isAbstract(self):
      return self._isAbstract

   def setIsAbstract(self, isAbstract):
      self._isAbstract = isAbstract

   def getDescription(self):
      return self.description

   def addOption(self, opt):
      self.options.append(opt)

   def loadOptions(self):
      options = list()

      # Open a file
      f = open(self.filec, "r")

      names = []     # Attribute in namelist
      vars = []      # m<Variable>
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

      # Loop over lines and find all indications of options
      while(True):
         line = f.readline()
         if(line == ""):
            break;

         # Check for starting comment
         m = re.search('//!(.*)', line)
         if(m != None):
            start = 1
            comment = m.group(1)
            lastComment = lastComment + comment
         else:
            # Look for each type of options
            for i in range(0,len(cases)):
               c = cases[i]
               m = re.search('Options.get' + c + '\("(\w+)",\s*(\w+)\)', line)
               if(m != None):
                  name = m.group(1)
                  if name != "tag":
                     var  = m.group(2)
                     desc = lastComment
                     req  = creq[i]
                     vec  = cvec[i]
                     opt = Option(name, var, req, vec, description=desc)
                     options.append(opt)
            start = 0
            lastComment = ""
      f.close()

      # Second pass finding types
      # Check both header and implementation file. Type is normally in header file, but might be
      # in implementation file if it is a local temporary variable
      for f0 in [self.fileh, self.filec]:
         f = open(f0, "r")
         while(True):
            line = f.readline()
            if(line == ""):
               break;
            # Check if the scheme is under development
            m = re.search('underDevelopment()', line)
            if(m != None):
               self.underDevelopment = True

            # Check if variable is initialized in constructor mValue(iValue)
            m = re.findall('([a-zA-Z0-9]+)\(([a-zA-Z0-9\.:\*\+\/\-"]+)\)', line)
            if(len(m) > 0):
               for n in range(0, len(m)):
                  try:
                     var = m[n][0]
                     default = m[n][1]
                     for opt in options:
                        if(opt.getVar() == var):
                           opt.setDefault(default)
                  except IndexError:
                     pass

            # Find a declaration of the variable, e.g.:
            #    std::vector<float> mVariable;
            #    bool mVariable;
            m = re.search('\s*([\w:<>]+)\s+(\w+);', line)
            if(m != None):
               try:
                  type = m.group(1)
                  var  = m.group(2)
                  for opt in options:
                     if(opt.getType() == "" and opt.getVar() == var):
                        opt.setType(type)
               except IndexError:
                  pass
         f.close()
      return options
   def getHtmlTable(self, extraOptions=None):
      options = self.getOptions()
      if(extraOptions != None):
         options = concatenate(options, extraOptions)
      html = ""
      if(len(options)>0):
         if(self.isAbstract()):
            html = html + '               The following attributes are inherited by all ' + self.getName() + ' schemes:\n'
         html = html + '               <table class="table table-striped table-condensed">\n'\
               + "                  <thead>\n"\
               + '                     <tr><th width="20%">Attribute</th><th width="12%">Type</th><th width="12%">Default</th><th>Description</th></tr>\n'\
               + "                  </thead>\n"

         # Write options
         for opt in options:
            html = html + opt.getHtml()
         html = html + "               </table>\n"
      return html
   def getHtml(self, linkToTop):
      html = ""
      if(self.isAbstract()):
         heading = "Common options"
      else:
         devel = ""
         if(self.isUnderDevelopment()):
            devel = " (under development)"

         heading = self.getName() + devel + " (<code>class=" + self.getClassName()+ '</code>)'

      html = '            <div class="panel panel-info">\n'\
            + '               <div class="panel-heading">\n'\
            + '                  <h4>' + heading + '\n'
      if(linkToTop):
         html = html + '                   <span class="pull-right topLink"><a href="#top"><i class="fa fa-arrow-circle-up"> Top</i></a></span>\n'
      html = html + '                  </h4>\n'\
            + '               </div>\n'\
            + '               <div class="panel-body">\n'
      if(not self.isAbstract()):
         html = html + "                  <p>" + self.getDescription() + "</p>\n"
      html = html + self.getHtmlTable()
      html = html + "               </div>\n"
      html = html + "            </div>\n"

      return html
