#!/usr/bin/python

import pandas as pd
import numpy as np
#import matplotlib.pyplot as plt

diskSectors = pd.read_csv("./diskSectors",delimiter=',')
filesExtents = pd.read_csv("./filesExtents",delimiter=',')
timestamps = pd.read_csv("./timestamps",delimiter=',')
with open("backupAnalysis","w") as f:
    f.write("runID,totalBackedUpSectors,effective,cost,totalFileSectors\n")

for runID in range(20):
    filesExtents2 = filesExtents.loc[filesExtents["id"]==runID]
    diskSectors2 = diskSectors.loc[diskSectors["runID"]==runID]

    ransomwareStart = timestamps.loc[runID][3]
    techniqueStop = timestamps.loc[runID][5]

    diskSectors3 = diskSectors2[(diskSectors2['backupTime'] >= ransomwareStart) & (diskSectors2['backupTime'] < techniqueStop)]

    diskSectors4 = list(set(diskSectors3['diskSector'].tolist()))
    diskSectors4.sort()

    sectors = []
    for index, row in filesExtents2.iterrows():
        physicalOffsetStart = row['physicalOffsetStart']
        physicalOffsetEnd = row['physicalOffsetEnd']
        while (physicalOffsetStart <= physicalOffsetEnd):
            temp = 2048 + (8*physicalOffsetStart)
            for i in range(8):
                temp2 = temp + i
                sectors.append(temp2)
            physicalOffsetStart += 1
    sectors.sort()

    diskSectors5 = []
    for diskSector in diskSectors4:
        if (diskSector > sectors[0]) and (diskSector < sectors[len(sectors)-1]):
            diskSectors5.append(diskSector)

    start = sectors[0]
    end = start
    sectorList = []
    for sector in sectors:
        if sector != start:
            if sector == end+1:
                end = sector
            else:
                sectorList.append((start,end))
                start = sector
                end = start

    effective = 0
    for diskSector in diskSectors5:
        for sectorTuple in sectorList:
            if (diskSector >= sectorTuple[0]) and (diskSector <= sectorTuple[1]):
                effective += 1
                break

    totalBackedUpSectors = len(diskSectors2['diskSector'].tolist())
    cost = totalBackedUpSectors - effective
    totalFileSectors = len(sectors)
    with open("backupAnalysis","a+") as f:
        string = str(runID)+','+str(totalBackedUpSectors)+','+str(effective)+','+str(cost)+','+str(totalFileSectors)+'\n'
        f.write(string)
