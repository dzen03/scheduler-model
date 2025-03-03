#ifndef SRC_INCLUDES_NODES_INODE_H_
#define SRC_INCLUDES_NODES_INODE_H_

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "Stats.h"

namespace yql_model {

class Node {
 public:
  enum Type : std::uint8_t { SOURCE, FILTER };
  enum NetworkMode : std::uint8_t { NONE, INPUT, OUTPUT, BOTH };

  Node();
  virtual ~Node() = default;

  Node(const Node& source) = default;
  Node(Node&& source) = default;

  Node& operator=(const Node& source) = default;
  Node& operator=(Node&& source) = default;

  [[nodiscard]] virtual double GetOutputVolume() = 0;
  [[nodiscard]] virtual Stats GetUsage() = 0;
  [[nodiscard]] virtual Stats GetUsage(NetworkMode need_network) = 0;

  void SetInputVolume(double value) { input_ = value; }
  void AddInputVolume(double value) { SetInputVolume(input_ + value); }
  void ResetInputVolume() { SetInputVolume(0); }
  [[nodiscard]] auto GetInputVolume() const { return input_; }

  void SetServer(std::size_t server_id) {
    assert(server_id == std::size_t(-1) || server_id_ == std::size_t(-1));
    server_id_ = server_id;
  }
  [[nodiscard]] auto GetServer() const { return server_id_; }

  [[nodiscard]] auto GetNodeId() const { return node_id_; }

 private:
  double input_ = 0;
  std::size_t server_id_ = -1;
  std::size_t node_id_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_NODES_INODE_H_
