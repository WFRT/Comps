#!/bin/csh
# Initializes by creating folders and automated code
set compsDir = `pwd`
set srcDir = $compsDir/src

# Create folders
mkdir -p $compsDir/results
mkdir -p $compsDir/obj
mkdir -p $compsDir/obj_debug

cd $srcDir
set components = `ls`
echo $components
cd $compsDir
foreach component ($components)
   if(-d $srcDir/$component) then
      mkdir -p $compsDir/obj_debug/$component
      mkdir -p $compsDir/obj/$component
   endif
end

# Create automated schemes code
cd $srcDir/Tools
csh generateSchemeFactories.csh
cd $srcDir/Variables
csh init.csh
