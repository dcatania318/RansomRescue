#!/usr/bin/python

import psutil
import sys
f = open("/media/user/VD/usages", "a")
f.write(sys.argv[1])  #id
f.write(',')
f.write(sys.argv[2]) #time
f.write(',')
f.write(str(psutil.cpu_percent())) #cpu_percent
f.write(',')
f.write(str(psutil.phymem_usage().percent)) #phymem_percent
f.write(',')
f.write(str(psutil.virtmem_usage().percent)) #virtmem_percent
f.write(',')
f.write(str(psutil.disk_usage('/media/user/VD').percent)) #disk_percent
f.write(',')
if len(sys.argv) > 3:
    f.write(sys.argv[3]) #comment
f.write('\n')
