#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/statvfs.h>
#include <thread>
#include <time.h>
#include <vector>
#include "diskSectorBackup.hpp"
using namespace std;

struct BlkTraceInfo
{
  int seqNo, processID, sectorNo, numberOfBlocks;
  float time;
  char RWBS;
};

vector<BlkTraceInfo> filteredBlkTraceData;
int vectorSizeN; // used in averageLastNIntegers & averageLastNFloats
unsigned long fileSystemBlockSize = 512;
string directory = "/media/user/VD/";
string sectorsLocation =  directory + "diskSectors";
string backupsLocation =  directory + "diskSectorBackups";
string dataDirectory = directory + "data/";

bool getNextBatchOfEvents();
float averageLastNIntegers(vector<int>* v);
float averageLastNFloats(vector<float>* v);
float averageFloats(vector<float>* v);
string getDiskSector(string blkDevName, int sectorNo);
float entropyCheck(string data);
float entropyCheck(string blkDevName, int diskSector);
void backup(string type, int runID, int timeWindowID, string blkDevName, vector<int>* v);
void dumpTMinfo(int runID, int timeWindowID, int reads, float averageLastNreadsPTW, int writes, float averageLastNwritesPTW, float averageEntropies2, float averageLastNentropies, bool backedTF, time_t t);

int main(int argc, char *argv[])
{
  int runID = -1;
  if (argc > 1) { runID = strtol(argv[1], NULL, 10); }
  string blkDevName = "sda", mode = "stock";
  float timeWindow = 0.5; // seconds
  float maxReadsPTW = 2500.0, maxWritesPTW = 2000.0, maxEntropy = 5.0;
  vectorSizeN = 2;

  vector<int> diskSectors, readsPTW, writesPTW;
  vector<float> writeEntropies, writeEntropies2;
  float averageLastNreadsPTW, averageLastNwritesPTW;
  float averageLastNentropies, averageEntropies2;

  BlkTraceInfo blkTraceInfo;
  int reads, writes;
  bool tf = true, backedTF = false;
  int timeWindowID = 0;
  time_t t, t2;

  readsPTW.clear();
  writesPTW.clear();
  writeEntropies.clear();

  while(tf)
  {
    t = time(0);
    if(getNextBatchOfEvents())
    {
      cout << "TimeWindow: " << timeWindowID << endl;
      reads = 0, writes = 0;
      writeEntropies2.clear();
      diskSectors.clear();
      for(size_t i = 0; i != filteredBlkTraceData.size(); i++)
      {
        blkTraceInfo = filteredBlkTraceData.at(i);
        if (blkTraceInfo.RWBS == 'R')
        {
          for (size_t j = 0; j < blkTraceInfo.numberOfBlocks; j++)
          {
            diskSectors.push_back(blkTraceInfo.sectorNo+j);
            reads++;
          }
        }
        else if (blkTraceInfo.RWBS == 'W')
        {
          //sampling
          writeEntropies2.push_back(entropyCheck(blkDevName, blkTraceInfo.sectorNo));
          writes += blkTraceInfo.numberOfBlocks;
        }
      }
      readsPTW.push_back(reads);
      writesPTW.push_back(writes);
      averageLastNreadsPTW = averageLastNIntegers(&readsPTW);
      averageLastNwritesPTW = averageLastNIntegers(&writesPTW);
      averageEntropies2 = averageFloats(&writeEntropies2);
      writeEntropies.push_back(averageEntropies2);
      averageLastNentropies = averageLastNFloats(&writeEntropies);

      backedTF = false;
      if ((averageLastNreadsPTW > maxReadsPTW) ||
      (averageLastNwritesPTW > maxWritesPTW) ||
      (averageLastNentropies > maxEntropy))
      {
        bool displayLocationOfBackedUpSectors = false;
        if (mode == "stock")
        {
          backup("sectorsInfo", runID, timeWindowID, blkDevName, &diskSectors);
          displayLocationOfBackedUpSectors = true;
        }
        if (displayLocationOfBackedUpSectors)
        {
          backedTF = true;
        }
      }

      t2 = difftime(time(0), t);
      dumpTMinfo(runID, timeWindowID, reads, averageLastNreadsPTW, writes, averageLastNwritesPTW, averageEntropies2, averageLastNentropies, backedTF, t2);
      timeWindowID++;
    }
  }
  return 0;
}

bool getNextBatchOfEvents()
{
  struct dirent **namelist;
  string blkTraceFile, file;
  int n = scandir(dataDirectory.c_str(), &namelist, 0, alphasort);
  if (n < 0)
    perror("scandir");
  else
  {
    for (int i=0; i<n; i++)
    {
      file = namelist[i]->d_name;
      if ((blkTraceFile.empty()) and (file != ".") and (file != ".."))
      {
        blkTraceFile = dataDirectory + file;
        // cout << blkTraceFile << endl;
      }
      free(namelist[i]);
    }
    free(namelist);
  }

  filteredBlkTraceData.clear();
  if (!blkTraceFile.empty())
  {
    ifstream infile(blkTraceFile);
    int seqNo, processID, sectorNo, numberOfBlocks;
    float _time;
    char RWBS;
    string line;
    while (getline(infile, line))
    {
      istringstream iss(line);
      if (iss >> seqNo >> _time >> processID >> RWBS >> sectorNo >> numberOfBlocks)
      {
        BlkTraceInfo blkTraceInfo;
        blkTraceInfo.seqNo = seqNo;
        blkTraceInfo.time = _time;
        blkTraceInfo.processID = processID;
        blkTraceInfo.RWBS = RWBS;
        blkTraceInfo.sectorNo = sectorNo;
        blkTraceInfo.numberOfBlocks = numberOfBlocks;
        filteredBlkTraceData.push_back(blkTraceInfo);
      }
    }
    // cout << "deleting file" << endl;
    string execute = "exec rm " + blkTraceFile;
    system(execute.c_str());
  }
  if (filteredBlkTraceData.size() == 0)
  {
    // cout << "sleeping" << endl;
    this_thread::sleep_for(chrono::milliseconds(5000));
    return false;
  }

  return true;
}

float averageLastNIntegers(vector<int>* v)
{
  float total = 0.0;
  int i = 0;
  if (v->size() > vectorSizeN)
  {
    i = v->size() - vectorSizeN;
  }
  for (i; i<v->size(); i++)
  {
    total += v->at(i);
  }
  if (v->size() > vectorSizeN)
  {
    return total/vectorSizeN;
  }
  else
  {
    return total/v->size();
  }
}

float averageLastNFloats(vector<float>* v)
{
  float total = 0.0;
  int i = 0;
  if (v->size() > vectorSizeN)
  {
    i = v->size() - vectorSizeN;
  }
  for (i; i<v->size(); i++)
  {
    total += v->at(i);
  }
  if (total == 0.0)
    return total;
  if (v->size() > vectorSizeN)
  {
    return total/vectorSizeN;
  }
  else
  {
    return total/v->size();
  }
}

float averageFloats(vector<float>* v)
{
  float total = 0.0;
  for (int i=0; i<v->size(); i++)
  {
    total += v->at(i);
  }
  if (total == 0.0)
    return total;
  else
  {
    return total/v->size();
  }
}

string getDiskSector(string blkDevName, int sectorNo)
{
  string diskName = "/dev/" + blkDevName;
  string diskError = string() + diskName + ": ";

  // Open device file
  ifstream disk(diskName.c_str(), ios_base::binary);

  if(!disk)
  throw(runtime_error(diskError + strerror(errno)));

  // Seek to sectorNo
  disk.seekg(fileSystemBlockSize * sectorNo);
  if(!disk)
  throw(runtime_error(diskError + strerror(errno)));

  // Read in one sector
  vector<char> buffer(fileSystemBlockSize);
  disk.read(&buffer[0], fileSystemBlockSize);
  if(!disk)
  throw(runtime_error(diskError + strerror(errno)));

  string str(buffer.begin(),buffer.end());
  return str;
}

float entropyCheck(string data)
{
  map<char,int> frequencies;
  for(char c : data)
  {
    frequencies[c]++;
  }
  int numlen = data.length() ;
  float infocontent = 0 ;
  for(pair<char,int> p : frequencies)
  {
    float freq = static_cast<float>(p.second)/numlen;
    infocontent -= freq * (log(freq) / log(2));
  }
  // cout << "Entropy: " << infocontent << endl;
  return infocontent;
}

float entropyCheck(string blkDevName, int diskSector)
{
  string data = getDiskSector(blkDevName, diskSector);
  return entropyCheck(data);
}

void backup(string type, int runID, int timeWindowID, string blkDevName, vector<int>* v)
{
  if (type == "sectors")
  {
    int diskSector;
    string data;
    float entropy;
    ofstream writer;
    string fileName = backupsLocation + to_string(runID);
    writer.open(fileName, ios_base::app);
    if (!writer)
    {
      cout << "Error Opening File - " << backupsLocation << endl;
    }
    for(size_t i = 0; i != v->size(); i++)
    {
      diskSector = v->at(i);
      data = getDiskSector(blkDevName, diskSector);
      entropy = entropyCheck(data);

      DiskSectorBackup diskSectorBackup;
      diskSectorBackup.runID = runID;
      diskSectorBackup.timeWindowID = timeWindowID;
      diskSectorBackup.diskSector = diskSector;
      diskSectorBackup.backupTime = time(0);
      diskSectorBackup.entropy = entropy;
      diskSectorBackup.blkDevName = blkDevName;
      diskSectorBackup.data = data;
      writer << diskSectorBackup << endl;
    }
    writer.close();
  }
  else if (type == "sectorsInfo")
  {
    ofstream writer;
    writer.open(sectorsLocation, ios_base::app);
    if (!writer)
    {
      cout << "Error Opening File - " << sectorsLocation << endl;
    }
    for(size_t i = 0; i != v->size(); i++)
    {
      writer << runID << ',';
      writer << timeWindowID << ',';
      writer << v->at(i) << ',';
      writer << time(0) << endl;
    }
    writer.close();
  }
}

void readBackedUpSectors(string fileName)
{
  fstream binary_file(fileName, ios::binary|ios::in);
  DiskSectorBackup p_Data;
  while(binary_file >> p_Data)
  {
      cout << "Disk Sector: " << p_Data.diskSector << endl;
      cout << "BackupTime: " << p_Data.backupTime << endl;
      cout << "Data:" << endl << p_Data.data << endl << endl;
  }
  binary_file.close();
}

void dumpTMinfo(int runID, int timeWindowID, int reads, float averageLastNreadsPTW, int writes, float averageLastNwritesPTW, float averageEntropies2, float averageLastNentropies, bool backedTF, time_t t)
{
  string file = directory + "TWinfo";
  ofstream writer;
  writer.open(file, ios_base::app);
  if (!writer)
  {
    cout << "Error Opening File - " << file << endl;
  }
  string boolean = "false";
  if (backedTF)
    boolean = "true";
  writer << runID << ',' << timeWindowID << ',';
  writer << reads << ',' << averageLastNreadsPTW << ',';
  writer << writes << ',' << averageLastNwritesPTW << ',';
  writer << averageEntropies2 << ',' << averageLastNentropies << ',';
  writer << boolean << ',' << t << endl;
  writer.close();
}
