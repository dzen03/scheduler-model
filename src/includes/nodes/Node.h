#ifndef SRC_INCLUDES_NODES_INODE_H_
#define SRC_INCLUDES_NODES_INODE_H_

#include <cstdint>

#include "Stats.h"

namespace yql_model {

class Node {
 public:
  enum Type : std::uint8_t { SOURCE, FILTER };

  Node() = default;
  virtual ~Node() = default;

  Node(const Node& source) = default;
  Node(Node&& source) = default;

  Node& operator=(const Node& source) = default;
  Node& operator=(Node&& source) = default;

  [[nodiscard]] virtual double GetOutputVolume() = 0;
  [[nodiscard]] virtual Stats GetUsage() = 0;
  [[nodiscard]] virtual Stats GetUsage(bool need_network) = 0;

  void SetInputVolume(double value) { input_ = value; }
  void AddInputVolume(double value) { SetInputVolume(input_ + value); }
  void ResetInputVolume() { SetInputVolume(0); }
  [[nodiscard]] auto GetInputVolume() const { return input_; }

 private:
  double input_ = 0;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_NODES_INODE_H_
