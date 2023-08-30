#include "format.h"

#include <string>
#include <iomanip>
using std::string;

// Done: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int left_seconds, minutes, hours;
    std::stringstream ss;
    
    hours = seconds / 3600;
    minutes = (seconds%3600) / 60;
    left_seconds = (seconds%3600) % 60;
    
    ss << hours << ":" << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << left_seconds;
    string formatted(ss.str());
    return formatted; 
}