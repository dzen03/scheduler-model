#ifndef SRC_INCLUDES_NODES_INODE_H_
#define SRC_INCLUDES_NODES_INODE_H_

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "Stats.h"

namespace yql_model {

class Node {
 public:
  enum NetworkMode : std::uint8_t { NONE, INPUT, OUTPUT, BOTH };
  struct Volume {
    double local;
    double remote;
  };

  Node();
  virtual ~Node() = default;

  Node(const Node& source) = delete;
  Node(Node&& source) = default;

  Node& operator=(const Node& source) = delete;
  Node& operator=(Node&& source) = default;

  [[nodiscard]] virtual double GetOutputVolume() = 0;
  [[nodiscard]] virtual Stats GetUsage() = 0;
  [[nodiscard]] virtual Stats GetUsage(NetworkMode need_network) = 0;
  [[nodiscard]] virtual std::shared_ptr<Node> GetCopy() const = 0;

  [[nodiscard]] virtual Stats GetTrueUsage() = 0;

  void SetOutputVolume(Volume value) { output_ = value; }
  void AddOutputVolume(Volume value) {
    SetOutputVolume({.local = output_.local + value.local,
                     .remote = output_.remote + value.remote});
  }
  void ResetOutputVolume() { SetOutputVolume({.local = 0, .remote = 0}); }

  void SetInputVolume(Volume value) { input_ = value; }
  void AddInputVolume(Volume value) {
    SetInputVolume({.local = input_.local + value.local,
                    .remote = input_.remote + value.remote});
  }
  void ResetInputVolume() { SetInputVolume({.local = 0, .remote = 0}); }
  [[nodiscard]] auto GetInputVolume() const {
    return input_.remote + input_.local;
  }

  void SetServer(std::size_t server_id) {
    assert(server_id == std::size_t(-1) || server_id_ == std::size_t(-1));
    server_id_ = server_id;
  }
  [[nodiscard]] auto GetServer() const { return server_id_; }

  [[nodiscard]] auto GetNodeId() const { return node_id_; }

  [[nodiscard]] auto GetInputVolume_() const { return input_; }
  [[nodiscard]] auto GetOutputVolume_() const { return output_; }

 private:
  Volume input_ = {.local = 0, .remote = 0};
  Volume output_ = {.local = 0, .remote = 0};
  std::size_t server_id_ = -1;
  std::size_t node_id_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_NODES_INODE_H_
