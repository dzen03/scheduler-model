#ifndef SRC_INCLUDES_SERVER_H_
#define SRC_INCLUDES_SERVER_H_

#include <cstddef>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Stats.h"
#include "nodes/Node.h"

namespace yql_model {

class Server {
 public:
  explicit Server(Stats stats) : limits_(stats) {}

  [[nodiscard]] auto GetLimits() const { return limits_; }
  [[nodiscard]] auto GetCpuLimit() const { return limits_.GetCpu(); }
  [[nodiscard]] auto GetNetworkLimit() const { return limits_.GetNetwork(); }
  [[nodiscard]] auto GetMemoryLimit() const { return limits_.GetMemory(); }

  [[nodiscard]] auto GetUsages() const { return usages_; }
  [[nodiscard]] auto GetCpuUsage() const { return usages_.GetCpu(); }
  [[nodiscard]] auto GetNetworkUsage() const { return usages_.GetNetwork(); }
  [[nodiscard]] auto GetMemoryUsage() const { return usages_.GetMemory(); }

  [[nodiscard]] auto GetNodes() const { return emplaced_nodes_; }

  bool EmplaceNode(const std::shared_ptr<Node>& node,
                   Node::NetworkMode need_network, std::size_t server_id);
  bool EmplaceNodes(const std::vector<std::shared_ptr<Node>>& nodes,
                    const std::vector<Node::NetworkMode>& need_networks,
                    std::size_t server_id);

  void RemoveNode(std::size_t node_id);

 private:
  Stats limits_;
  Stats usages_ = Stats();
  std::unordered_map<std::size_t, Stats> emplaced_nodes_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_SERVER_H_
