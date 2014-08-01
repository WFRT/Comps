#!/usr/bin/python
import re
import os
import numpy as np
from ClassFile import *

srcDir = "../compsAuto/src/"

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

# Add options to configuration
confdef.addOption(Option("downscaler", "", 0, 0, "std::string", description="Which downscaler should be used? If not specified, the downscaler from the run will be used, and if that is not speicified, the nearest neighbour approach is used."));

conf.merge(confdef)
write(conf)

varconf = getClassFile("VarConf")
write(varconf)

#run = getClassFile("Run")
#write(run)
print "Warning: It is too difficult to create options file for Run. Do manually."
