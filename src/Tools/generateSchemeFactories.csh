#!/bin/csh
if($1 == "") then
   set modelDirs = (`ls -F ../ | grep "/"`)
else 
   set modelDirs = $1
endif
cd ../
foreach modelDir ($modelDirs)
   if(!(-e $modelDir/notSchemeDirectory)) then
      set model = `echo "$modelDir" | sed "s/\///g"`

      # Remove the last 's' if needed
      if(!(-e $modelDir/${model}.h)) then
         if($modelDir == "Uncertainties/") then
            set model = "Uncertainty"
         else
            set model = `echo "$modelDir" | sed "s/s\///"`
         endif
      endif
      cd $modelDir

      set output = Schemes.inc
      set outputHeader = SchemesHeader.inc
      rm $output
      rm $outputHeader

      set needsData = 1
      if(${model} == "Input" || $model == "Interpolator" || $model == "DetMetric" || $model == "Transform" || $model == "Downscaler") then
         set needsData = 0
      endif

      # Write code portion
      if($needsData) then
         echo "${model}* ${model}::getScheme(const std::string& iTag, const Data& iData) {" >> $output
      else
         echo "${model}* ${model}::getScheme(const std::string& iTag) {" >> $output
      endif
      echo '   Options opt;' >> $output
      if(${model} == "Configuration") then
         echo '   getOptions(iTag, opt);' >> $output
      else
         echo '   Scheme::getOptions(iTag, opt);' >> $output
      endif
      if($needsData) then
         echo '   return getScheme(opt, iData);' >> $output
      else
         echo '   return getScheme(opt);' >> $output
      endif

      echo '}' >> $output

      if($needsData) then
         echo "${model}* ${model}::getScheme(const Options& iOptions, const Data& iData) {" >> $output
      else
         echo "${model}* ${model}::getScheme(const Options& iOptions) {" >> $output
      endif
      echo '   std::string className;' >> $output
      echo '   iOptions.getRequiredValue("class", className);' >> $output
      echo "   if(0) {}" >> $output
      set file = ${model}.h
      set names = `ls *.h | grep -v ${model}.h | cut -d'.' -f1 | grep -v Bypass`
      foreach name ($names)
         set fullname = $model$name
         echo '   else if(className == "'$fullname'") {' >> $output
         if($needsData) then
            echo "       return new $fullname(iOptions, iData);" >> $output
         else
            echo "       return new $fullname(iOptions);" >> $output
         endif
         echo "   }" >> $output
      end
      echo "   else {" >> $output
      echo "      Component::doesNotExist(className);" >> $output
      echo "      return NULL;" >> $output
      echo "   }" >> $output
      echo "}" >> $output

      # Write header
      set names = `ls *.h | grep -v ${model}.h | cut -d'.' -f1`
      foreach name ($names)
         set fullname = $model$name
         echo "#include " '"'"${name}.h"'"' >> $outputHeader
      end
      echo '#include "../Scheme.h"' >> $outputHeader

      cd ../
   endif
end
