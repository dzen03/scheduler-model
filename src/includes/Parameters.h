#ifndef SRC_INCLUDES_PARAMETERS_H
#define SRC_INCLUDES_PARAMETERS_H

#include <functional>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Stats.h"
#include "Util.h"
#include "libs/includes/Util.h"

namespace yql_model {

template <typename T>
struct Range {
  T min;  // NOLINT
  T max;  // NOLINT

  [[nodiscard]] std::string ToJson() const {
    std::ostringstream res;
    res << R"({"min":)" << min << R"(,"max":)" << max << "}";
    return res.str();
  }

  static Range Parse(const std::string& inp) {
    auto delim_pos = inp.find(Util::delimeter);
    return {.min = Util::ParseValue<T>(inp.substr(0, delim_pos)),
            .max = Util::ParseValue<T>(
                inp.substr(delim_pos + 1, std::string::npos))};
  }
};

// NOLINTNEXTLINE
#define DEFINE_PARAM(type, name, ...)                                 \
 private:                                                             \
  type name##_ = __VA_ARGS__;                                         \
                                                                      \
 public:                                                              \
  [[nodiscard]] const char* get_##name##_name() { return #name; }     \
  [[nodiscard]] type get_##name() {                                   \
    const std::shared_lock lock(mutex_);                              \
    return name##_;                                                   \
  }                                                                   \
                                                                      \
 private:                                                             \
  [[maybe_unused]] const bool _temp_##name##_ = [this]() {            \
    jsonParams_.emplace_back(                                         \
        #name, [this]() { return Util::ToJsonValue(get_##name()); }); \
    return true;                                                      \
  }()

// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
class Parameters {
 private:
  mutable std::shared_mutex mutex_;
  std::vector<std::pair<std::string, std::function<std::string()>>> jsonParams_;
  // graph_generation
  DEFINE_PARAM(Range<int>, graph_size, {.min = 2, .max = 30});
  DEFINE_PARAM(Range<int>, source_volume, {.min = 100, .max = 500});
  DEFINE_PARAM(Range<double>, filter_volume, {.min = 0.6, .max = 1});

  // server
  DEFINE_PARAM(int, servers_count, 3);
  DEFINE_PARAM(
      Stats, servers_stat,
      Stats({.cpu = 34 * 100, .memory = 100 * 1000, .network = 10000}));

  DEFINE_PARAM(int, max_count_local, 5);

 public:
  std::string ToJson() const {
    const std::shared_lock lock(mutex_);
    std::ostringstream res;
    res << "{";
    bool first = true;
    for (const auto& entry : jsonParams_) {
      if (!first) {
        res << ",";
      }
      res << "\"" << entry.first << "\":" << entry.second();
      first = false;
    }
    res << "}";
    return res.str();
  }
  void Update(const ArgumentsMap& map) {
    const std::unique_lock lock(mutex_);
    for (const auto& [param, value_str] : map) {
      std::cout << param << " -- " << value_str << "\n";

      if (param == "graph_size") {
        Util::ParseValue(value_str, graph_size_);
      } else if (param == "filter_volume") {
        Util::ParseValue(value_str, filter_volume_);
      } else if (param == "source_volume") {
        Util::ParseValue(value_str, source_volume_);
      } else if (param == "servers_count") {
        Util::ParseValue(value_str, servers_count_);
      } else if (param == "servers_stat") {
        Util::ParseValue(value_str, servers_stat_);
        std::cout << servers_stat_.ToJson();
      } else if (param == "max_count_local") {
        Util::ParseValue(value_str, max_count_local_);
      } else {
        std::cerr << "unknown param: " << param << '\n';
        throw std::runtime_error("unknown param: " + param);
      }
    }
  }
};
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
// NOLINTEND(readability-magic-numbers)
};  // namespace yql_model

#endif  // SRC_INCLUDES_PARAMETERS_H
