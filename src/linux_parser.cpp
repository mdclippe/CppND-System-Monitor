#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

/// @brief Get the value from a filename based on its key
/// @tparam T 
/// @param filename 
/// @param key : if key is empty, it will return the first value, otherwise the key is used as comparator
/// @return 
template<typename T>
T getValueFromFile(const std::string &filename, const std::string &key)
{
  T value = T();
  std::ifstream stream(filename);
  if (stream.is_open()) {
    std::string line;
    std::string readkey;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      // if the key is empty, return the first value
      if (key == "")
      {
        linestream >> value;
        break;
      }
      else
      {
        linestream >> readkey;
        if (readkey == key) {
          linestream >> value;
          break;
        }
      }
    }
  }
  return value;
}

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {  

  const float memtotal = getValueFromFile<float> (kProcDirectory + kMeminfoFilename, "MemTotal:");
  const float memfree = getValueFromFile<float> (kProcDirectory + kMeminfoFilename, "MemFree:");
  float value = 0.0;

  if (memtotal == 0 || memtotal < memfree)
    value = 0.0;
  else
    value = (memtotal - memfree) / memtotal;
  return value;
}

// Done: Read and return the system uptime
long LinuxParser::UpTime() { 
  return getValueFromFile<long>(kProcDirectory + kUptimeFilename, ""); 
}

// Done: Read and return the total number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cpu_jiffies = CpuUtilization();
  long total = 0;
  for (auto jiffie : cpu_jiffies)
  {
    total += std::stol(jiffie);
  }
  return total; 
}

// Done: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  long uptime = 0, stime = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::string line;
    std::string ignore;
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (auto i =0; i < 13; i++)
    {
          linestream >> ignore;
    }
    // do not include waiting for child processes here...
     linestream >> uptime >> stime;
    }
  return uptime+stime;
}

// Done: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_jiffies = CpuUtilization();

  return std::stol(cpu_jiffies[CPUStates::kUser_]) + std::stol(cpu_jiffies[CPUStates::kNice_]) 
  + std::stol(cpu_jiffies[CPUStates::kSystem_]) + std::stol(cpu_jiffies[CPUStates::kIRQ_]) +
  std::stol(cpu_jiffies[CPUStates::kSoftIRQ_]) + std::stol(cpu_jiffies[CPUStates::kSteal_]); 

}

// Done: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> cpu_jiffies = CpuUtilization();
  return std::stol(cpu_jiffies[CPUStates::kIdle_]) + std::stol(cpu_jiffies[CPUStates::kIOwait_]); 
}

// Done: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> numbers;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::string line;
    std::string value;
    std::string readkey;
    // while loop here to make sure we are looking at the correct line, other approach would be the only check the first line
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> readkey;
      if (readkey == "cpu")
      {
        for (auto i =0; i < CPUStates::kCount; i++)
        {
          linestream >> value;
          numbers.push_back(value);
        }
        break;
      }
    }
  }
  return numbers; 
}


// Done: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  // there was an error in the totalprocess explanation : on the udacity website it states that it resides in the /proc/meminfo file!
  return getValueFromFile<int>(kProcDirectory + kStatFilename,"processes"); 
}

// Done: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  return getValueFromFile<int>(kProcDirectory + kStatFilename,"procs_running"); ; 
}

// Done: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  return getValueFromFile<string>(kProcDirectory + std::to_string(pid) + kCmdlineFilename, ""); 
}

// Done: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  return getValueFromFile<string>(kProcDirectory + std::to_string(pid) + kStatusFilename, "VmSize:"); 
}

// Done: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  return getValueFromFile<std::string>(kProcDirectory + std::to_string(pid) + kStatusFilename,"Uid:");
}

// Done: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  std::string user;
  string uid = LinuxParser::Uid(pid);
  // parse the passwd file
   std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
      std::string line;
      std::string ignore;
      std::string read_uid;
      while (std::getline(stream, line)) {
        std::replace(line.begin(), line.end(), ':',' ');
        std::istringstream linestream(line);
        linestream >> user >> ignore >> read_uid;
        if (read_uid == uid)
        {
          break;
        }

      }
    }
    return user;
  }

// Done: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long start_time_ticks = 0;
 std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
      std::string line;
      std::string ignore;
      std::getline(stream, line);
      std::istringstream linestream(line);
      for (auto i = 1; i < 22; i++)
      {
        linestream >> ignore;
      }
      linestream >> start_time_ticks;
    }

  return LinuxParser::UpTime() - (start_time_ticks / sysconf(_SC_CLK_TCK));
}
