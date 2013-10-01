#!/bin/csh
set total = 0
cd ../
set folders = (Calibrators Averagers Continuous Correctors Discretes Downscalers Selectors)
#foreach folder (`ls -F | grep "/"`)
foreach folder ($folders)
   if(!(-e $folder/notSchemeDirectory)) then
      set num = `ls $folder | grep '.h$' | wc -l`
      #ls $folder | grep '.h$'
      @ num = $num - 1
      @ total = $total + $num
      echo "${folder}: $num"
   endif
end
echo "Number of schemes: $total"
