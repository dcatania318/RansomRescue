#!/usr/bin/python

import pandas as pd
import numpy as np

diskSectors = pd.read_csv("./diskSectors",delimiter=',')
timestamps = pd.read_csv("./timestamps",delimiter=',')
totalFileSectors = 12343808
with open("./backupAnalysis","w") as f:
    f.write("runID,cost,totalFileSectors\n")

for runID in range(20):
    filesExtents2 = filesExtents.loc[filesExtents["id"]==runID]
    diskSectors2 = diskSectors.loc[diskSectors["runID"]==runID]

    cost = len(diskSectors2['diskSector'].tolist())
    with open("./backupAnalysis","a+") as f:
        string = str(runID)+','+str(cost)+','+str(totalFileSectors)+'\n'
        f.write(string)
