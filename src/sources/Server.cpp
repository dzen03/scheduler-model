#include "Server.h"

#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>

#include "nodes/Node.h"

namespace yql_model {
bool Server::EmplaceNode(const std::shared_ptr<Node>& node,
                         Node::NetworkMode need_network,
                         std::size_t server_id) {
  bool res = false;

  if (usages_ + node->GetUsage(need_network) <= limits_) {
    usages_ = usages_ + node->GetUsage(need_network);
    node->SetServer(server_id);
    emplaced_nodes_.emplace(node->GetNodeId(), node->GetUsage(need_network));
    res = true;
  }

  return res;
}

bool Server::EmplaceNodes(const std::vector<std::shared_ptr<Node>>& nodes,
                          const std::vector<Node::NetworkMode>& need_networks,
                          std::size_t server_id) {
  bool res = false;

  assert(nodes.size() == need_networks.size());

  Stats need_usage{};
  for (int i = 0; i < nodes.size(); ++i) {
    need_usage = need_usage + nodes[i]->GetUsage(need_networks[i]);
  }

  if (usages_ + need_usage <= limits_) {
    usages_ = usages_ + need_usage;
    for (int i = 0; i < nodes.size(); ++i) {
      nodes[i]->SetServer(server_id);
      emplaced_nodes_.emplace(nodes[i]->GetNodeId(),
                              nodes[i]->GetUsage(need_networks[i]));
    }
    res = true;
  }

  return res;
}

void Server::RemoveNode(std::size_t node_id) {
  assert(emplaced_nodes_.contains(node_id));
  usages_ = usages_ - emplaced_nodes_.at(node_id);
  emplaced_nodes_.erase(node_id);
}
}  // namespace yql_model
