#include "Graph.h"

#include <cassert>
#include <cstddef>
#include <memory>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

#include "nodes/Node.h"

namespace yql_model {

Graph::Graph(std::size_t size) {
  adjacency_list_.resize(size);
  nodes_.resize(size);
}

Graph* Graph::AddNode(std::unique_ptr<Node> node,
                      const std::vector<std::size_t>& parents) {
  assert(size_ < nodes_.size());
  nodes_[size_] = std::move(node);

  if (parents.empty()) {
    sources_.emplace(size_);
  }

  for (const auto& parent : parents) {
    assert(parent < adjacency_list_.size());
    adjacency_list_[parent].emplace_back(size_);
  }

  ++size_;
  return this;
}

// TODO(dzen): add dynamic
void Graph::CalculateThroughput() {
  for (std::size_t node_id = 0; node_id < size_; ++node_id) {
    // preserve input volumes of sources
    if (!sources_.contains(node_id)) {
      nodes_[node_id]->ResetInputVolume();
    }
  }

  std::queue<std::size_t> queue;
  std::vector<bool> visited(size_);

  for (const auto& source : sources_) {
    queue.emplace(source);
  }

  while (!queue.empty()) {
    auto node_id = queue.front();
    queue.pop();

    if (visited[node_id]) {
      continue;
    }
    visited[node_id] = true;

    for (const auto& child_id : adjacency_list_[node_id]) {
      nodes_[child_id]->AddInputVolume(nodes_[node_id]->GetOutputVolume());
      queue.emplace(child_id);
    }

    total_usage_ = total_usage_ + nodes_[node_id]->GetUsage();
  }
}

std::unordered_set<std::size_t> Graph::GetSinks() const {
  std::unordered_set<std::size_t> res;

  std::size_t node_id = 0;
  for (const auto& node : adjacency_list_) {
    if (node.empty()) {
      res.emplace(node_id);
    }
    ++node_id;
  }

  return res;
}

std::vector<std::pair<std::size_t, std::size_t>> Graph::GetEdgeList() const {
  std::vector<std::pair<std::size_t, std::size_t>> res;
  int ind = 0;
  for (const auto& row : adjacency_list_) {
    for (const auto& val : row) {
      // std::cout << ind << ">" << val << "\n";
      res.emplace_back(nodes_[ind]->GetNodeId(), nodes_[val]->GetNodeId());
    }
    // std::cout << "\n";
    ++ind;
  }
  return res;
}

}  // namespace yql_model
