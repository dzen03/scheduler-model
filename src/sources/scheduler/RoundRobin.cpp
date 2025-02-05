#include "scheduler/RoundRobin.h"

#include <iostream>
#include <stdexcept>

namespace yql_model {
bool RoundRobin::Schedule(const Graph& graph, std::vector<Server>& servers) {
  std::size_t started_trying_server = -1;
  std::size_t node_id = 0;

  for (const auto& node : graph.GetNodes()) {
    bool emplaced = false;
    while (!emplaced) {
      emplaced = servers[next_server_].EmplaceNode(node, 2);

      if (!emplaced) {
        if (started_trying_server == next_server_ - 1) {
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
}  // namespace yql_model
