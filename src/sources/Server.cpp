#include "Server.h"

#include <cassert>
#include <memory>
#include <vector>

#include "nodes/Node.h"

namespace yql_model {
bool Server::EmplaceNode(const std::shared_ptr<Node> node,
                         Node::NetworkMode need_network) {
  bool res = false;

  if (usages_ + node->GetUsage(need_network) <= limits_) {
    usages_ = usages_ + node->GetUsage(need_network);
    res = true;
  }

  return res;
}

bool Server::EmplaceNodes(const std::vector<std::shared_ptr<Node>> nodes,
                          std::vector<Node::NetworkMode> need_networks) {
  bool res = false;

  assert(nodes.size() == need_networks.size());

  Stats need_usage{};
  for (int i = 0; i < nodes.size(); ++i) {
    need_usage = need_usage + nodes[i]->GetUsage(need_networks[i]);
  }

  if (usages_ + need_usage <= limits_) {
    usages_ = usages_ + need_usage;
    res = true;
  }

  return res;
}
}  // namespace yql_model
