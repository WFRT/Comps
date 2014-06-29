#!/usr/bin/python
import re
import os
import numpy as np
from ClassFile import *

srcDir = "../comps/src/"

def getClassFile(name):
   filec = srcDir + name + ".cpp"
   fileh = srcDir + name + ".h"
   return ClassFile(name.replace("/",""), fileh, filec)

def write(classFile):
   name = classFile.getName()
   fo = open("_includes/" + name.replace("/", "") + ".html", "w")
   fo.write(classFile.getHtmlTable())
   fo.close()

confdef = getClassFile("Configurations/Default")
conf    = getClassFile("Configurations/Configuration")

conf.merge(confdef)
write(conf)

varconf = getClassFile("VarConf")
write(varconf)

#run = getClassFile("Run")
#write(run)
print "Warning: It is too difficult to create options file for Run. Do manually."
