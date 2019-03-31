# RansomRescue

## Code Directory
The directory *code/* contains the prototype implementation code.

```bash
.
├── code
│   ├── btrace.sh
│   ├── data.txt.enc
│   ├── diskSectorBackup.hpp
│   ├── encScript.py
│   ├── getFileExtents.py
│   ├── preprocessing.cpp
│   ├── psutil_usages.py
│   └── ransomRescue.cpp
```

* The *btrace.sh* file contains a modified script that initiates *blktrace*, *blkparse* and the *preprocessing* processes. It takes a block device file path as a parameter.
* The *data.txt.enc* file contains an encrypted encryption key that is used by the encryption script.
* The *diskSectorBackup.hpp* file contains a structure for the disk sector backup object together with methods for writing and reading to/from a file.
* The *encScript.py* file contains the code related to the representative sample. It takes the decryption key (Test1234) and an execution ID as parameters.
* The *getFileExtents.py* file contains the code that extract the extents occupied by files targeted by the encryption script.
* The preprocessing.cpp file contains code that analysis a blkparse input to extract read and write trace events and save these events for every time window. A blkparse stream of data is taken as a parameter.
* The *psutil_usages.py* file contains code that extracts CPU and memory usage of a system. It takes an execution ID, time and an optional comment string as parameters.
* The *ransomRescue.cpp* file contains the prototype implementation of RansomRescue, where it was said to take an execution ID as a parameter.

In order to compile the output files for the *.cpp* files, it is recommended to use the following command for *a.cpp*:
```bash
g++ --std=c++11 a.cpp -o a.out
```

## Sessions Directory
The directory *sessions/* contains the sessions used to evaluate *RansomRescue*. The two files are bash scripts that automatically execute consecutive sessions. The *run_ransomware.sh* file executes sessions for Ransomware Activity, whilst the *run_ransomwareLike.sh* executes sessions for Ransomware-Like Behaviour. These sessions are to be placed in the same folder as the files within the *code/* directory.

```bash
.
sessions
│   ├── run_ransomwareLike.sh
│   └── run_ransomware.sh
```

## Results Directory
The directory *results/* contains the results obtained from the evaluation of *RansomRescue*. Results are presented for the 2 experiments carried out within which one would find the results for every session. A folder named ransomware refers to Ransomware Activity Sessions, whilst a folder named *ransomwareLike* refers to Ransomware-Like Behaviour Sessions.

```bash
.
├── results
│   ├── experiment1
│   │   ├── configurationParameters
│   │   ├── ransomware
│   │   │   ├── averageLast2readsPTW.png
│   │   │   ├── averageLast2writesPTW.png
│   │   │   ├── backupAnalysis
│   │   │   ├── backupAnalysis2
│   │   │   ├── backupAnalysis.ipynb
│   │   │   ├── backUpTime
│   │   │   ├── cpu.png
│   │   │   ├── diskSectors
│   │   │   ├── diskSectors.py
│   │   │   ├── filesExtents
│   │   │   ├── memory.png
│   │   │   ├── responseTime.png
│   │   │   ├── results.ipynb
│   │   │   ├── timestamps
│   │   │   ├── TWinfo
│   │   │   ├── usages
│   │   │   └── wireshark
│   │   └── ransomwareLike
│   │       ├── averageLast2readsPTW.png
│   │       ├── averageLast2writesPTW.png
│   │       ├── backupAnalysis
│   │       ├── backupAnalysis.ipynb
│   │       ├── cpu.png
│   │       ├── diskSectors
│   │       ├── diskSectors.py
│   │       ├── memory.png
│   │       ├── responseTime.png
│   │       ├── results.ipynb
│   │       ├── timestamps
│   │       ├── TWinfo
│   │       ├── usages
│   │       └── wireshark
│   └── experiment2
│       ├── parameters
│       ├── ransomware
│       │   ├── averageLast2readsPTW.png
│       │   ├── averageLast2writesPTW.png
│       │   ├── backupAnalysis
│       │   ├── backupAnalysis2
│       │   ├── backupAnalysis.ipynb
│       │   ├── backUpTime
│       │   ├── cpu.png
│       │   ├── diskSectors
│       │   ├── diskSectors.py
│       │   ├── filesExtents
│       │   ├── memory.png
│       │   ├── responseTime.png
│       │   ├── results.ipynb
│       │   ├── timestamps
│       │   ├── TWinfo
│       │   ├── usages
│       │   └── wireshark
│       └── ransomwareLike
│           ├── averageLast2readsPTW.png
│           ├── averageLast2writesPTW.png
│           ├── backupAnalysis
│           ├── backupAnalysis.ipynb
│           ├── cpu.png
│           ├── diskSectors
│           ├── diskSectors.py
│           ├── memory.png
│           ├── responseTime.png
│           ├── results.ipynb
│           ├── timestamps
│           ├── TWinfo
│           ├── usages
│           └── wireshark
```

The *configurationParameters* file in each experiment directory contains the configuration parameters used within that experiment.

##### NOTE: Due to large file size, the *diskSectors* file for every session of every experiment is to be obtained from [*Google Drive*](https://drive.google.com/open?id=1g9kJvaBo2QslH3l29bfnhOs5O0Oj0hYw).


The following is a brief overview of what the files within every session contain:
* The *TWinfo*, *usages* and *wireshark* files contain output information to be able to calculate the average CPU and memory usage and response time.
* The *results.ipynb* file is a notebook that is used to calculate and present the previous generated metrics. It generates the following files: *averageLast2readsPTW.png*, *averageLast2writesPTW.png*, *cpu.png*, *memory.png* and *responseTime.png*.
* The *timestamps* file contains timestamp information related to every execution of a session.
* The *diskSectors.py* contains the code that calculates the effectiveness of *RansomRescue* for every execution of the session. It produces the *backupAnalysis* file that contains the related effectiveness/cost data. In some sessions, a backupAnalysis2 is present which has outling execution data eliminated.
* The *backupAnalysis.ipynb* is a notebook the averages the results in the *backupAnalysis2/backupAnalysis* file.

## Secondary Drive Contents Directory
The directory secondaryDriveContents/ contains the default files required to be present on the secondary drive. The secondary drive path used within the code is /media/user/VD/ and so if one's secondary drive path is different, the code has to be updated accordingly.

```bash
.
├── secondaryDriveContents
│   ├── data
│   ├── diskSectors
│   ├── filesExtents
│   ├── TWinfo
│   └── usages
```
