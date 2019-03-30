#!/usr/bin/python

import os
import re
import sys
filesExtents = open("/media/user/VD/filesExtents", "a")
for i in range(1,2+1):
  for j in range(1,1000+1):
    cmd = 'filefrag -v /var/www/html/upload/files/files'+str(i)+'/file'+str(j)+'.txt | grep "[0-9]:" > /media/user/VD/temp'
    os.system(cmd)
    with open("/media/user/VD/temp") as f:
      for line in f:
        line2 = line.lstrip().strip('\n')
        line2 = re.sub(' +', '\t', line2)
        items = line2.split('\t')
        string = sys.argv[1]+',files'+str(i)+',file'+str(j)+'.txt,'+items[3].strip('..')+','+items[4].strip(':')+','+items[5].strip(':')+'\n'
        filesExtents.write(string)
filesExtents.close()
