#!/bin/csh
set output = Schemes.inc
set outputHeader = SchemesHeader.inc
rm $output
rm $outputHeader

set model = Variable

# Write code portion
echo "${model}* ${model}::create(std::string iName) {" >> $output
echo "   if(0) {}" >> $output
set file = ${model}.h
set names = `ls *.h | grep -v ${model}.h | cut -d'.' -f1 | grep -v Bypass`
foreach name ($names)
   set fullname = $model$name
   echo '   else if(iName == "'$name'") {' >> $output
   echo "       return new $fullname();" >> $output
   echo "   }" >> $output
end
echo "   else {" >> $output
echo "       return new ${model}Bypass(iName);" >> $output
echo "   }" >> $output
echo "}" >> $output

# Write header
set names = `ls *.h | grep -v ${model}.h | cut -d'.' -f1`
foreach name ($names)
   set fullname = $model$name
   echo "#include " '"'"${name}.h"'"' >> $outputHeader
end
