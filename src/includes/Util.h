#ifndef SRC_INCLUDES_UTIL_H_
#define SRC_INCLUDES_UTIL_H_

#include <cstdlib>
#include <iostream>
#include <string>

namespace yql_model {
class Util {
 public:
  static std::string ToJsonValue(const int& value) {
    return std::to_string(value);
  }

  static std::string ToJsonValue(const double& value) {
    return std::to_string(value);
  }

  template <typename T>
  static std::string ToJsonValue(const T& value) {
    return value.ToJson();
  }

  template <typename T>
  static T ParseValue(const std::string& value) {
    if constexpr (std::is_same_v<T, int>) {
      return std::stoi(value);
    } else if constexpr (std::is_same_v<T, double>) {
      std::cerr << "double: #" << value << "#\n";
      return std::stod(value);
    } else {
      return T::Parse(value);
    }
  }

  static void ParseValue(const std::string& value, int& ret) {
    ret = std::stoi(value);
  }

  static void ParseValue(const std::string& value, double& ret) {
    ret = std::stod(value);
  }

  template <typename T>
  static void ParseValue(const std::string& value, T& ret) {
    ret = T::Parse(value);
  }

  static constexpr std::string delimeter = ",";
};
}  // namespace yql_model

#endif  // SRC_INCLUDES_UTIL_H_
