#!/bin/csh

set basePath = data
#set month = 201010
#set day = 20101001
foreach month (`ls $basePath`)
   foreach day (`ls $basePath/$month`)
      foreach file (`ls $basePath/$month/${day}/*.grb2`)
#set y = `echo $file:t | cut -d'_' -f3 | cut -b1-4`
#set m = `echo $file:t | cut -d'_' -f3 | cut -b5-6`
#set d = `echo $file:t | cut -d'_' -f3 | cut -b7-8`
#set h = `echo $file:t | cut -d'_' -f4`
         set date = `echo $file:t | cut -d'_' -f3`
         set offset = `echo $file:t | cut -d'_' -f5 | cut -b1-3`
#set stamp = `/usr/local/ncl5.2/bin/ncl_filedump $file | grep initial | head -1 | awk '{print $3}'`
         set stamp = `/usr/local/grib_api-1.10.0/bin/grib_debug $file | grep -m 1 dataDate | awk '{print $5}'`
#echo "$y $m $d $h : $stamp"
         echo "$date $offset : $stamp"
         if("$stamp" == "$date") then
            echo "Matches";
         else
            echo "Doesnt match";
         endif
      end
   end
end
