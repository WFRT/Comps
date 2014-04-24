import re
import os
import numpy as np
class Option:
   def __init__(self, name, var, isRequired, isVector, type="", default="", description=""):
      self.name = name
      self.var = var
      self.isRequiredField = isRequired
      self.isVectorField = isVector
      self.type = type
      self.default = default
      self.description = description
   def getName(self):
      return self.name
   def getVar(self):
      return self.var
   def isRequired(self):
      return self.isRequiredField
   def isVector(self):
      return self.isVectorField
   def getType(self):
      type = self.type
      if(self.isVector()):
         m = re.search("std::vector<([\w:]+)>", self.type)
         if(m != None):
            type = m.group(1) + "[]"
      return type
   def getDefault(self):
      return self.default
   def getDescription(self):
      return self.description
   def setName(self, name):
      self.name = name
   def setVar(self, var):
      self.var = var
   def setIsRequired(self, isRequired):
      self.isRequired = isRequired
   def setIsVector(self, isVector):
      self.isVector = isVector
   def setType(self, type):
      self.type = type
   def setDefault(self, default):
      self.default = default
   def setDescription(self, description):
      self.description = description
   def getHtml(self):
      if(self.isRequired()):
         name = '<i class="fa fa-circle"></i> ' + self.getName() + ''
      else:
         name = '<i class="fa fa-circle-o"></i> ' + self.getName() + ''
      html = '                  <tr><td width="25%">' + name + '</td><td width="15%"><code>'\
            + self.getType() + '</code></td><td>' + self.getDefault() + '</td><td>' + self.getDescription() + "</td></tr>\n"
      return html
