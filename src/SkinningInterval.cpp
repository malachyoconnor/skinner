#include <chrono>
#include <fstream>
#include "SkinningInterval.h"
#include "utils.h"

using namespace std::chrono_literals;
using std::chrono::seconds, std::chrono::time_point, std::chrono::system_clock;
using std::optional;
using std::string;

void SkinningInterval::write_interval_to_file(std::ofstream &output_stream) const {
   // Pretty printing human-readable time into file
   const auto start_time_point = system_clock::from_time_t(start_time);

   string human_readable_time = get_human_readable_time(start_time_point);

   if (end_time != -1) {
      const auto end_time_point = system_clock::from_time_t(end_time);
      human_readable_time += "<-->";
      human_readable_time += get_human_readable_time(end_time_point);
   }

   output_stream << human_readable_time << " " << start_time << " " << end_time << " " << breaks_taken
         << std::endl;
}
