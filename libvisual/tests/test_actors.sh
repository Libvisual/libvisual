#!/bin/sh

ACTORS="/usr/local/lib/libvisual-0.5/actor/"
#RESOLUTIONS="20x20 32x32 128x128 320x200 640x400 800x600"
RESOLUTIONS="32x32 128x128 320x200 640x400 800x600"

n=0
for a in ${ACTORS}/libactor_* ; do 
    a=${a##*libactor_} 
    a=${a%%.so*} 
    
    res[${n}]=${res[${n}]}"${a}: "
    for r in ${RESOLUTIONS} ; do
	if $(lv-tool -a ${a} -D ${r} -F 1 >/dev/null 2>&1 3>&1) ; then 
	    res[${n}]=${res[${n}]}"${r} (OK) "
	else 
	    res[${n}]=${res[${n}]}"${r} (FAIL) "
	fi 
    done
    n=$[n+1]
done

echo "=============================="
echo "RESULTS:"
echo "=============================="
n=0
while test -n "${res[${n}]}" ; do
    echo "${res[${n}]}"
    n=$[n+1]
done
