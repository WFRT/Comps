#!/bin/csh
# Code to test the verification system
set verif = "python verif"
set files = "../results/tutorial/verif/T_*.nc"

mkdir testPlots
rm testPlots/*.png
set f = "-f testPlots"

set clim = "-c ../results/tutorial/verif/T_clim_0.nc"
set Clim = "-C ../results/tutorial/verif/T_clim_0.nc"

################
# Basic scores #
################
foreach metric (mae rmse minobs maxobs minfcst maxfcst stderror bias cmae dmb qq std num corr scatter obsfcst cond timeseries)
   $verif $files -m $metric $f/${metric}.png
end

#############
# Threshold #
#############
set thresholds = "-r -10:2:10"
foreach metric (hit falsealarm hanssenkuiper biasfreq ets baserate oddsratioss threat droc droc0 drocnorm)
   $verif $files -m $metric -r -10:2:10 -x threshold $f/${metric}.png
end

foreach metric (droc droc0 drocnorm)
   $verif $files -m $metric -r 5 $f/${metric}.png
end

$verif $files -m within -r 2 $f/within_2.png

#################
# Probabilistic #
#################
foreach metric (pithist reliability brier)
   $verif $files -m $metric -r 0 $f/${metric}.png
end

###########
# Options #
###########
set metric = mae

# Check axes
foreach axis (date offset location locationElev locationId locationLat locationLon)
   $verif $files -m $metric -x $axis $f/${metric}_${axis}.png
end

# Check line options
set metric = "mae"
$verif $files -m $metric -ms 0 -lw 4 $f/ms0_lw4.png

# Check binned
set metric = "ets"
$verif $files -m $metric -x threshold -r 0:10 -binned $f/ets_binned.png

# Check font sizes
set metric = "mae"
$verif $files -m $metric -labfs 8 -tickfs 6 -legfs 12 $f/lab8_tick6_leg12.png

# Test climatology
set metric = "ets"
$verif $files -m $metric -x threshold -r 0:10 $clim $f/ets_addClim.png
$verif $files -m $metric -x threshold -r 0:10 $Clim $f/ets_multClim.png