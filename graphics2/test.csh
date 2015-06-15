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
foreach metric (mae rmse minobs maxobs minfcst maxfcst stderror bias cmae dmb qq std num corr scatter obsfcst against timeseries)
   $verif $files -m $metric $f/${metric}.png
end

#############
# Threshold #
#############
set thresholds = "-r -10:2:10"
foreach metric (hit fa kss biasfreq ets baserate or lor yulesq threat)
   $verif $files -m $metric -r -10:2:10 -x threshold $f/${metric}.png
end
$verif $files -m cond  -r -10:2:10 $f/cond.png
$verif $files -m count -r -10:2:10 $f/count.png
$verif $files -m count -r -10:2:10 -b below $f/count_below.png

foreach metric (droc droc0 drocnorm)
   $verif $files -m $metric -r 5 $f/${metric}.png
end

$verif $files -m within -r 2 $f/within_2.png

#################
# Probabilistic #
#################
foreach metric (pithist reliability pitdev)
   $verif $files -m $metric -r 0 $f/${metric}.png
end

###########
# Options #
###########
set metric = mae

# Check axes
foreach axis (date offset location locationElev locationId locationLat locationLon none)
   $verif $files -m $metric -x $axis $f/${metric}_${axis}.png
end

# Check line options
set metric = "mae"
$verif $files -m $metric -ms 0 -lw 4 $f/ms0_lw4.png

# Check bin type
set metric = "ets"
$verif $files -m $metric -x threshold -r 0:10 -b below $f/ets_below.png
$verif $files -m $metric -x threshold -r 0:10 -b above $f/ets_above.png
$verif $files -m $metric -x threshold -r 0:10 -b within $f/ets_within.png

# Check font sizes
set metric = "mae"
$verif $files -m $metric -labfs 8 -tickfs 6 -legfs 12 $f/lab8_tick6_leg12.png

# Check margins
set metric = "mae"
$verif $files -m $metric -bot 0.3 $f/bot_03.png
$verif $files -m $metric -top 0.3 $f/top_03.png
$verif $files -m $metric -left 0.3 $f/left_03.png
$verif $files -m $metric -right 0.3 $f/right_03.png
# Check rotation
$verif $files -m $metric -xrot 45 -x date $f/rot_45.png

# Test climatology
set metric = "ets"
$verif $files -m $metric -x threshold -r 0:10 $clim $f/ets_addClim.png
$verif $files -m $metric -x threshold -r 0:10 $Clim $f/ets_multClim.png

$verif $files -m marginal -x threshold -r 0,11 $f/marginal.png
$verif $files -m marginalratio -x threshold -r 0,11 $f/marginalratio.png
foreach metric (bs bss bsrel bsres bsunc)
   $verif $files -m $metric -r 11 $f/$metric.png
end

# xlabel/ylabel/title
$verif $files -m mae -xlabel xlabel -ylabel ylabel -title Test $f/labels.png

# xticks/yticks
$verif $files -m mae -xticks 0,7,9 -yticks 0,4:10 $f/ticks.png

# legsort
$verif $files -m mae -legsort $f/legsort.png

# legloc
$verif $files -m mae -legloc right $f/legend_right.png

# lc
$verif $files -m mae -lc red,blue $f/redBlueLines.png

# ls
$verif $files -m mae -ls -,.,-- $f/lineStyles.png

# -tight
$verif $files -m mae -tight -,.,-- $f/tight.png
