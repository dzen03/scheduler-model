#include "scheduler/RoundRobin.h"

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Graph.h"
#include "Server.h"
#include "nodes/Node.h"

namespace yql_model {
bool RoundRobin::Schedule(const Graph& graph, std::vector<Server>& servers) {
  std::size_t started_trying_server = -1;
  std::size_t node_id = 0;

  for (const auto& node : graph.GetNodes()) {
    bool emplaced = false;
    while (!emplaced) {
      emplaced =
          servers[next_server_].EmplaceNode(node, Node::BOTH, next_server_);

      if (!emplaced) {
        if (started_trying_server != -1 &&
            started_trying_server == next_server_ - 1) {
          throw std::runtime_error("Cannot emplace node on any server");
        }
        if (started_trying_server == -1) {
          std::cout << "Tried placing Node #" << node_id << " onto Server #"
                    << next_server_ << "\n";
          started_trying_server = next_server_ - 1;
        }
      } else {
        std::cout << "Placed Node #" << node_id << " onto Server #"
                  << next_server_ << "\n";
        started_trying_server = -1;
      }
      next_server_ = (next_server_ + 1) % servers.size();
    }
    ++node_id;
  }

  return true;
}

bool RoundRobin::Remove(const Graph& graph, std::vector<Server>& servers) {
  for (const auto& node : graph.GetNodes()) {
    servers[node->GetServer()].RemoveNode(node->GetNodeId());
    node->SetServer(-1);
  }
  return true;
}

}  // namespace yql_model
