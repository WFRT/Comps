# Check regular files
cd ../
foreach file (`ls *.h`)
   set filename = $file:t
   set guardVariable = `echo $filename | tr 'a-z' 'A-Z' | tr '.' '_'`

   set guard1 = `head -1 $file`
   set expectedGuard1 = "#ifndef $guardVariable"
   set guard2 = `head -2 $file | tail -1`
   set expectedGuard2 = "#define $guardVariable"
   if(!("$guard1" == "$expectedGuard1" && "$guard2" == "$expectedGuard2")) then
      echo "FILE: $file"
      head -2 $file
      echo ""
   endif
end

# Check all models
foreach models (`ls`)
   if(-d $models) then
      # Determine model name
      # I.e the name without the ending s as in Selector(s)
      set model = ""
      cd $models
      foreach file (`ls *.h`)
         set filenoext = $file:r
         if("$filenoext" == "$models" || "${filenoext}s" == "$models") then
            set model = $filenoext
         endif
      end
      # Directories without this format are probable not model directories
      if($model != "") then
         # Loop over files
         foreach file (`ls *.h`)
            set filename = $file:t
            set filenoext = $file:r
            set MODEL = `echo $model | tr 'a-z' 'A-Z'`
            set guardVariable = `echo $filename | tr 'a-z' 'A-Z' | tr '.' '_'`
            if(!("$filenoext" == "$model")) then
               set guardVariable = "$MODEL$guardVariable"
            endif

            set guard1 = `head -1 $file`
            set expectedGuard1 = "#ifndef $guardVariable"
            set guard2 = `head -2 $file | tail -1`
            set expectedGuard2 = "#define $guardVariable"
            if(!("$guard1" == "$expectedGuard1" && "$guard2" == "$expectedGuard2")) then
               echo "FILE: $file"
               head -2 $file
               echo ""
            endif
         end
      endif
   cd ../
   endif
end
