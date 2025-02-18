#ifndef SRC_INCLUDE_STATS_H_
#define SRC_INCLUDE_STATS_H_

class Stats {
  double cpu_;
  double memory_;
  double network_;
  double disk_;

 public:
  struct StatsConstructor {
    double cpu;
    double memory;
    double network;
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

#endif  // SRC_INCLUDE_STATS_H_
