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
                      const std::vector<Link>& parents) {
  assert(size_ < nodes_.size());
  nodes_[size_] = std::move(node);

  if (parents.empty()) {
    sources_.emplace(size_);
  }

  for (const auto& parent : parents) {
    assert(parent.id < adjacency_list_.size());
    adjacency_list_[parent.id].emplace_back(
        Link{.id = size_, .volume = parent.volume});
  }

  ++size_;
  return this;
}

// TODO(dzen): add dynamic
void Graph::CalculateThroughput(bool dynamic) {
  for (std::size_t node_id = 0; node_id < size_; ++node_id) {
    // preserve input volumes of sources
    if (!sources_.contains(node_id)) {
      nodes_[node_id]->ResetInputVolume();
    }
    nodes_[node_id]->ResetOutputVolume();
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

    for (auto& [child_id, volume, size] : adjacency_list_[node_id]) {
      if (!dynamic ||
          nodes_[child_id]->GetServer() != nodes_[node_id]->GetServer()) {
        nodes_[child_id]->AddInputVolume(
            {.remote = nodes_[node_id]->GetOutputVolume() * volume});

        nodes_[node_id]->AddOutputVolume(
            {.remote = nodes_[node_id]->GetOutputVolume() * volume});

        size = nodes_[node_id]->GetOutputVolume() * volume;
      } else {
        nodes_[child_id]->AddInputVolume(
            {.local = nodes_[node_id]->GetOutputVolume() * volume});
        nodes_[node_id]->AddOutputVolume(
            {.local = nodes_[node_id]->GetOutputVolume() * volume});
        size = nodes_[node_id]->GetOutputVolume() * volume;
      }
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

std::vector<Graph::FullLink> Graph::GetEdgeList() const {
  std::vector<FullLink> res;
  int ind = 0;
  for (const auto& row : adjacency_list_) {
    for (const auto& [val, volume, size] : row) {
      // std::cout << ind << ">" << val << "\n";
      res.emplace_back(nodes_[ind]->GetNodeId(), nodes_[val]->GetNodeId(),
                       volume, size);
    }
    // std::cout << "\n";
    ++ind;
  }
  return res;
}

Graph Graph::GetCopy() const {
  Graph res = Graph(this->size_);
  res.adjacency_list_ = this->adjacency_list_;

  res.sources_ = this->sources_;

  res.total_usage_ = this->total_usage_;

  for (const auto& node : this->nodes_) {
    res.nodes_[res.size_++] = node->GetCopy();
  }

  return res;
}

}  // namespace yql_model
