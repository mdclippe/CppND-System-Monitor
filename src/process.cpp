#include "process.h"
#include "linux_parser.h"
#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

// Done: Return this process's ID
int Process::Pid() const { return pid_; }

// Done: Return this process's CPU utilization
float Process::CpuUtilization() const { 
  float total_time_ticks = static_cast<float>(LinuxParser::ActiveJiffies(Pid()));
  float uptime_seconds = static_cast<float>(LinuxParser::UpTime(Pid()));

  return 100* ((total_time_ticks / sysconf(_SC_CLK_TCK)) / uptime_seconds); 
}

// Done: Return the command that generated this process
string Process::Command() { 
  return command_; 
}

// Done: Return this process's memory utilization
string Process::Ram() const { 
  // convert kB to MB
  // use of atoi here because it will silenty fail if no conversion is possible
  int ram_kb = std::atoi(LinuxParser::Ram(Pid()).c_str());
  int ram_mb = ram_kb / 1000; 
  return std::to_string(ram_mb);
}

// Done: Return the user (name) that generated this process
string Process::User() { 
  return user_; 
}

void Process::SetUser(int pid) {
  user_ = LinuxParser::User(pid);
}

void Process::SetCommand(int pid) {
  command_ = LinuxParser::Command(pid);
}
// Done: Return the age of this process (in seconds)
long int Process::UpTime() { 
  return LinuxParser::UpTime(Pid()); 
}

// Done: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return CpuUtilization()< a.CpuUtilization() ;
  // return std::stol(Ram())< std::stol(a.Ram()) ;
}

