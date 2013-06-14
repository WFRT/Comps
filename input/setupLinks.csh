set dataDir = /Volumes/Scratch/Thomas/CompsData/input/
set folders = ('annstlf' 'bchClim' 'bchNaefs' 'bchNaefsVerif' 'bchNaefsVerifObs' 'gfsOp' 'rda336')

foreach folder ($folders)
   ln -s $dataDir/$folder $folder/data
end
