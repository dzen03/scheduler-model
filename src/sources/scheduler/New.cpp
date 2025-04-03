#include "scheduler/New.h"

#include <cstddef>
#include <string>
#include <vector>

#include "Graph.h"
#include "Server.h"

namespace yql_model {
bool New::Schedule([[maybe_unused]] const Graph& graph,
                   [[maybe_unused]] std::vector<Server>& servers) {
  if (graph.GetSize() <= params_->get_max_count_local()) {
    return singlehost_.Schedule(graph, servers);
  }
  return roundrobin_.Schedule(graph, servers);
}
bool New::Remove(const Graph& graph, std::vector<Server>& servers) {
  for (const auto& node : graph.GetNodes()) {
    servers[node->GetServer()].RemoveNode(node->GetNodeId());
    node->SetServer(-1);
  }
  return true;
}

std::string New::GetName() const { return "New"; };

}  // namespace yql_model
