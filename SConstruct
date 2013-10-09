import os
defines  = ' -DWITH_GRIB -DWITH_NCURSES -DWITH_GTEST'
cflags   = ' -Wall -Wno-reorder -Wno-sign-compare'
opt      = Environment(CPPFLAGS = '-O3'    + defines + cflags)
dbg      = Environment(CPPFLAGS = '-g -pg' + defines + cflags)

exeDir = os.getcwd()

SConscript('main.scons', exports={'env': opt, 'exeDir':exeDir, 'debug':0}, variant_dir='build/opt/', duplicate=0)
SConscript('main.scons', exports={'env': dbg, 'exeDir':exeDir, 'debug':1}, variant_dir='build/debug/', duplicate=0)
