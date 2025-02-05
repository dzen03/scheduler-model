#ifndef SRC_INCLUDE_STATS_H_
#define SRC_INCLUDE_STATS_H_

class Stats {
  double cpu_;
  double memory_;
  double network_;

 public:
  struct StatsConstructor {
    double cpu;
    double memory;
    double network;
  };
  Stats() = default;
  explicit Stats(StatsConstructor stats)
      : cpu_(stats.cpu), memory_(stats.memory), network_(stats.network) {}

  [[nodiscard]] auto GetCpu() const { return cpu_; }
  [[nodiscard]] auto GetMemory() const { return memory_; }
  [[nodiscard]] auto GetNetwork() const { return network_; }

  friend Stats operator+(Stats lhs, const Stats& rhs) {
    return Stats({.cpu = lhs.cpu_ + rhs.cpu_,
                  .memory = lhs.memory_ + rhs.memory_,
                  .network = lhs.network_ + rhs.network_});
  }

  bool operator<=(const Stats& rhs) const {
    return cpu_ <= rhs.cpu_ && memory_ <= rhs.memory_ &&
           network_ <= rhs.network_;
  }

  bool operator==(const Stats& rhs) const {
    return cpu_ == rhs.cpu_ && memory_ == rhs.memory_ &&
           network_ == rhs.network_;
  }
};

#endif  // SRC_INCLUDE_STATS_H_
