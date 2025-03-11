#ifndef SRC_INCLUDE_STATS_H_
#define SRC_INCLUDE_STATS_H_

#include <sstream>
#include <string>

#include "Util.h"

namespace yql_model {

class Stats {
  double cpu_;
  double memory_;
  double network_;
  double disk_;

 public:
  struct StatsConstructor {
    double cpu = 0;
    double memory = 0;
    double network = 0;
    double disk = 0;
  };
  Stats() = default;
  explicit Stats(StatsConstructor stats)
      : cpu_(stats.cpu),
        memory_(stats.memory),
        network_(stats.network),
        disk_(stats.disk) {}

  [[nodiscard]] auto GetCpu() const { return cpu_; }
  [[nodiscard]] auto GetMemory() const { return memory_; }
  [[nodiscard]] auto GetNetwork() const { return network_; }
  [[nodiscard]] auto GetDisk() const { return disk_; }

  friend Stats operator+(Stats lhs, const Stats& rhs) {
    return Stats({.cpu = lhs.cpu_ + rhs.cpu_,
                  .memory = lhs.memory_ + rhs.memory_,
                  .network = lhs.network_ + rhs.network_,
                  .disk = lhs.disk_ + rhs.disk_});
  }

  friend Stats operator-(Stats lhs, const Stats& rhs) {
    return Stats({.cpu = lhs.cpu_ - rhs.cpu_,
                  .memory = lhs.memory_ - rhs.memory_,
                  .network = lhs.network_ - rhs.network_,
                  .disk = lhs.disk_ - rhs.disk_});
  }

  [[nodiscard]] std::string ToJson() const {
    std::ostringstream res;

    res << '{';
    for (const auto& stat :
         std::initializer_list<std::tuple<std::string, double>>{
             {"cpu", GetCpu()},
             {"memory", GetMemory()},
             {"network", GetNetwork()},
             {"disk", GetDisk()}}) {
      res << '"' << std::get<0>(stat) << R"(":)" << std::get<1>(stat) << ",";
    }
    res.seekp(-1, std::ios_base::cur);

    res << '}';

    return res.str();
  }

  static Stats Parse(const std::string& inp) {
    auto delim_pos1 = inp.find(Util::delimeter);
    auto delim_pos2 = inp.find(Util::delimeter, delim_pos1 + 1);
    auto delim_pos3 = inp.find(Util::delimeter, delim_pos2 + 1);
    return Stats(StatsConstructor{
        .cpu = Util::ParseValue<double>(inp.substr(0, delim_pos1)),
        .memory = Util::ParseValue<double>(
            inp.substr(delim_pos1 + 1, delim_pos2 - delim_pos1 - 1)),
        .network = Util::ParseValue<double>(
            inp.substr(delim_pos2 + 1, delim_pos3 - delim_pos2 - 1)),
        .disk = Util::ParseValue<double>(
            inp.substr(delim_pos3 + 1, std::string::npos))});
  }

  // comparing all stats
  bool operator<<=(const Stats& rhs) const {
    return cpu_ <= rhs.cpu_ && memory_ <= rhs.memory_ &&
           network_ <= rhs.network_ && disk_ <= rhs.disk_;
  }

  // comparing only memory (as per current yql)
  bool operator<=(const Stats& rhs) const { return memory_ <= rhs.memory_; }

  // bool operator==(const Stats& rhs) const {
  //   return cpu_ == rhs.cpu_ && memory_ == rhs.memory_ &&
  //          network_ == rhs.network_ && ;
  // }
};
}  // namespace yql_model

#endif  // SRC_INCLUDE_STATS_H_
