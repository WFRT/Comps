#!/bin/csh

set fileh = Corrector.h
set filec = Corrector.cpp
set cases = ("RequiredValue" "RequiredValues" "Value" "Values")
foreach case ($cases)
   echo $case
   set lines = `grep "Options.get$case(" $filec`
   set IFS=$'\n'
   foreach line ($lines)
      set nameVar = `echo $line | cut -d '"' -f2`
      set codeVar = `echo $line | cut -d ',' -f2 | cut -d ')' -f1`
      echo $line 
      exit
      if($codeVar != "") then
         set type    = `grep $codeVar $fileh | awk '{print $2}' | cut -d ';' -f1`
         echo "$nameVar $type"
      endif
   end
end
