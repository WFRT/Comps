#!/bin/csh
set total = 0
cd ../
foreach folder (`ls -F | grep "/"`)
   if(!(-e $folder/notSchemeDirectory)) then
      set num = `ls $folder | grep '.h$' | wc -l`
      #ls $folder | grep '.h$'
      @ num = $num - 1
      @ total = $total + $num
   endif
end
echo "Number of schemes: $total"
