import subprocess
import sys

visualCflags, visualCflagse = subprocess.Popen(["pkg-config", "--cflags", "libvisual-0.5"],
    stderr=subprocess.PIPE, stdout=subprocess.PIPE).communicate()

output = visualCflags + " -I js/src/"

env = Environment(CCFLAGS = Split("-g -Wall -DXP_UNIX " + output))

visualLdflags, visualLdflagse = subprocess.Popen(["pkg-config", "--libs", "libvisual-0.4"],
    stderr=subprocess.PIPE, stdout=subprocess.PIPE).communicate()

output = visualLdflags + " js/src/Linux_All_DBG.OBJ/libjs.a"

libs = env.ParseFlags(Split(output));
env.MergeFlags(libs)

plugin = env.SharedLibrary(["script_visscript.c"]);

env.Install("/usr/lib/libvisual-0.5/script/", plugin);
env.Alias('install', '/usr/lib/libvisual-0.5/script')


