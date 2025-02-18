#ifndef SRC_INCLUDES_SERVER_H_
#define SRC_INCLUDES_SERVER_H_

#include <memory>
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

  bool EmplaceNode(std::shared_ptr<Node> node, Node::NetworkMode need_network);
  bool EmplaceNodes(std::vector<std::shared_ptr<Node>> nodes,
                    std::vector<Node::NetworkMode> need_networks);

 private:
  Stats limits_;
  Stats usages_ = Stats();
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_SERVER_H_
