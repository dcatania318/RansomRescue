#include <chrono>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

string directory = "/media/user/VD/data/";
ofstream writer;
int fileNumber = 1000;

void openFile();
void saveLine(int seqNo, float _time, int processID, char RW, int sectorNo, int numberOfBlocks);
void closeFile();
void checkDirectory(int fileCount);

int main()
{
  float endTimeWindow = -1, timeWindow = 0.5;
  int major, minor, cpuID, seqNo, processID, sectorNo, numberOfBlocks, now;
  float _time;
  char action, RW;
  string RWBS, line, fileName, lastLine;
  while (getline(cin, line))
  {
    istringstream iss(line);
    if (iss >> major >> minor >> cpuID >> seqNo >> _time >> processID >> action >> RWBS >> sectorNo >> numberOfBlocks)
    {
      if (endTimeWindow == -1)
      {
        endTimeWindow = _time + timeWindow;
        openFile();
      }
      if (RWBS.find('R') != string::npos)
        RW = 'R';
      else if (RWBS.find('W') != string::npos)
        RW = 'W';
      else
        RW = 'Z';
      if (_time > endTimeWindow)
      {
        closeFile();
        checkDirectory(500);
        openFile();
        endTimeWindow = _time + timeWindow;
      }
      if ((action == 'Q') and (RW != 'Z'))
        saveLine(seqNo, _time, processID, RW, sectorNo, numberOfBlocks);
    }
  }
  return 0;
}

void openFile()
{
  string fileName = directory + to_string(fileNumber++);
  writer.open(fileName, ios_base::out);
  if (!writer)
  {
    cout << "Error Opening File - " << fileName << endl;
  }
}

void saveLine(int seqNo, float _time, int processID, char RW, int sectorNo, int numberOfBlocks)
{
  writer << to_string(seqNo) << '\t';
  writer << to_string(_time) << '\t';
  writer << to_string(processID) << '\t';
  writer << RW << '\t';
  writer << to_string(sectorNo) << '\t';
  writer << to_string(numberOfBlocks) << endl;
}

void closeFile()
{
  writer.close();
}

void checkDirectory(int fileCount)
{
  DIR *dp;
  int i = 0;
  struct dirent *ep;
  dp = opendir (directory.c_str());

  if (dp != NULL)
  {
    while (ep = readdir (dp))
    i++;

    (void) closedir (dp);
  }
  else
  perror ("Couldn't open the directory");

  i -= 2;

  if (i >= fileCount)
  {
    cout << "emptying directory" << endl;
    string execute = "exec rm -r " + directory + "*";
    system(execute.c_str());
  }
}
