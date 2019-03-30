#include <algorithm>
#include <fstream>
#include <string>

#include <iostream>

struct DiskSectorBackup
{
  int runID, timeWindowID, diskSector;
  time_t backupTime;
  float entropy;
  std::string blkDevName, data;

  DiskSectorBackup()
    : runID(-1)
    , timeWindowID(-1)
    , diskSector(-1)
    , backupTime(0)
    , entropy(0.0)
    , blkDevName("")
    , data("")
  {}

  void swap(DiskSectorBackup& other)
  {
    std::swap(runID, other.runID);
    std::swap(timeWindowID, other.timeWindowID);
    std::swap(diskSector, other.diskSector);
    std::swap(backupTime, other.backupTime);
    std::swap(entropy, other.entropy);
    std::swap(data, other.data);
  }
};

std::ostream& operator<<(std::ostream& stream, DiskSectorBackup const& diskSectorBackup)
{
  stream << diskSectorBackup.runID << " "
         << diskSectorBackup.timeWindowID << " "
         << diskSectorBackup.diskSector << " "
         << diskSectorBackup.backupTime << " "
         << diskSectorBackup.entropy << " "
         << diskSectorBackup.data.size() << ":"
         << diskSectorBackup.data;
  return stream;
}

std::istream& operator>>(std::istream& stream, DiskSectorBackup& data)
{
  DiskSectorBackup tmp;
  std::size_t size;
  if (stream >> tmp.runID >> tmp.timeWindowID >> tmp.diskSector >> tmp.backupTime >> tmp.entropy >> size)
  {
    tmp.data.resize(size);
    stream.read(&tmp.data[0], 1);
    if (stream.read(&tmp.data[0], size))
    {
      data.swap(tmp);
    }
  }
  return stream;
}
