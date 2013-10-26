import os
opt      = Environment(CPPFLAGS = '-O3 ')
dbg      = Environment(CPPFLAGS = '-g -pg ')
#SetOption('implicit_cache', 1)

exeDir = os.getcwd()

SConscript('main.scons', exports={'env': opt, 'exeDir':exeDir, 'debug':0}, variant_dir='build/opt/', duplicate=0)
SConscript('main.scons', exports={'env': dbg, 'exeDir':exeDir, 'debug':1}, variant_dir='build/debug/', duplicate=0)

#SConscript('doxygen.scons')
