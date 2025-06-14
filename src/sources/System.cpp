#include "System.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "Graph.h"
#include "Server.h"

namespace yql_model {

System* System::AddServer(const Server& server) {
  servers_.emplace_back(server);

  return this;
}

bool System::ExecuteQuery(const Graph& graph) {
  if (scheduler_->Schedule(graph, servers_)) {
    graphs_.emplace_back(graph);
    return true;
  }
  return false;
}

bool System::RemoveQuery(std::size_t index) {
  assert(index < graphs_.size());
  if (scheduler_->Remove(graphs_[index], servers_)) {
    std::cerr << "erased size before: " << graphs_.size() << " ";
    graphs_.erase(graphs_.begin() + static_cast<std::int64_t>(index));
    std::cerr << "after" << graphs_.size() << "\n";
    return true;
  }
  return false;
}

void System::ReexecuteAll() {
  const auto graphs = std::vector<Graph>(graphs_.begin(), graphs_.end());
  for (std::size_t i = graphs_.size(); (i--) != 0;) {
    RemoveQuery(i);
  }
  for (const auto& graph : graphs) {
    ExecuteQuery(graph);
  }
}

void System::Tick() {
  for (auto& graph : graphs_) {
    // graph->MultiplyBy();
    // TODO(dzen): implement
    graph.CalculateThroughput(true);
  }
  for (auto& server : servers_) {
    server.SyncNodesUsage();
  }
}

}  // namespace yql_model
