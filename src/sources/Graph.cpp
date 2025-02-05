#include "Graph.h"

#include <cassert>
#include <queue>
#include <utility>
#include <vector>

namespace yql_model {

Graph::Graph(std::size_t size) {
  adjacency_list_.resize(size);
  nodes_.resize(size);
  size_ = 0;
}

Graph* Graph::AddNode(std::unique_ptr<Node> node,
                      std::initializer_list<int> parents) {
  assert(size_ < nodes_.size());
  nodes_[size_] = std::move(node);

  if (parents.size() == 0) {
    sources_.emplace(size_);
  }

  for (auto& parent : parents) {
    assert(parent < adjacency_list_.size());
    adjacency_list_[parent].emplace_back(size_);
  }

  ++size_;
  return this;
}

void Graph::CalculateThroughput() {
  for (std::size_t node_id = 0; node_id < size_; ++node_id) {
    // preserve input volumes of sources
    if (!sources_.contains(node_id)) {
      nodes_[node_id]->ResetInputVolume();
    }
  }

  std::queue<std::size_t> queue;
  std::vector<bool> visited(size_);

  for (auto& source : sources_) {
    queue.emplace(source);
  }

  while (!queue.empty()) {
    auto node_id = queue.front();
    queue.pop();

    if (visited[node_id]) {
      return;
    }
    visited[node_id] = true;

    for (auto& child_id : adjacency_list_[node_id]) {
      nodes_[child_id]->AddInputVolume(nodes_[node_id]->GetOutputVolume());
      queue.emplace(child_id);
    }
  }
}

}  // namespace yql_model
