#include "scheduler/SingleHost.h"

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Graph.h"
#include "Server.h"
#include "nodes/Node.h"

namespace yql_model {
bool SingleHost::Schedule(const Graph& graph, std::vector<Server>& servers) {
  std::size_t started_trying_server = -1;
  // std::size_t node_id = 0;

  bool emplaced = false;
  while (!emplaced) {
    std::vector<Node::NetworkMode> networks(graph.GetSize(), Node::NONE);

    for (const auto& node_id : graph.GetSources()) {
      networks[node_id] = Node::INPUT;
    }

    for (const auto& node_id : graph.GetSinks()) {
      networks[node_id] = Node::OUTPUT;
    }

    emplaced = servers[next_server_].EmplaceNodes(graph.GetNodes(), networks,
                                                  next_server_);

    if (!emplaced) {
      if (started_trying_server == next_server_ - 1) {
        throw std::runtime_error("Cannot emplace node on any server");
      }
      if (started_trying_server == -1) {
        std::cout << "Tried placing Graph onto Server #" << next_server_
                  << "\n";
        started_trying_server = next_server_ - 1;
        // break;
      }
    } else {
      std::cout << "Placed Graph onto Server #" << next_server_ << "\n";
      started_trying_server = -1;
    }
    // ++node_id;
    next_server_ = (next_server_ + 1) % servers.size();
  }

  return true;
}
bool SingleHost::Remove(const Graph& graph, std::vector<Server>& servers) {
  for (const auto& node : graph.GetNodes()) {
    servers[node->GetServer()].RemoveNode(node->GetNodeId());
    node->SetServer(-1);
  }
  return true;
}
}  // namespace yql_model
