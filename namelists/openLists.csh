#!/bin/csh
if($1 == "") then
   set folder = "default"
else 
   set folder = $1
endif
vi -o $folder/runs.nl $folder/configurations.nl $folder/schemes.nl
