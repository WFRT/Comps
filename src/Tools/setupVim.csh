cd ../
foreach dir (`ls`)
   if(-d $dir) then
      if($dir != "Tools") then
         rm $dir/.vimrc
         echo "set makeprg=make\ -C\ ../../\ obj/$dir/%:t:r.o" > $dir/.vimrc
#ln -s ../Tools/vimrc $dir/.vimrc
      endif
   endif
end
# Home directory
rm .vimrc
echo "set makeprg=make\ -C\ ../\ obj/%:t:r.o" > .vimrc
#ln -s Tools/vimrc .vimrc
