#include "processor.h"
#include "linux_parser.h"

// Done: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float total = static_cast<float>(LinuxParser::Jiffies());
    float active = static_cast<float>(LinuxParser::ActiveJiffies());
    return active/total; 
}