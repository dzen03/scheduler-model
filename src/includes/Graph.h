#ifndef SRC_INCLUDES_GRAPH_H_
#define SRC_INCLUDES_GRAPH_H_

#include <cstddef>
#include <memory>
#include <unordered_set>
#include <vector>

#include "Stats.h"
#include "nodes/Node.h"

namespace yql_model {

class Graph {
 public:
  explicit Graph(std::size_t size);

  const std::shared_ptr<Node>& operator[](std::size_t idx) const {
    return nodes_[idx];
  }

  [[nodiscard]] auto GetNodes() const { return nodes_; }
  [[nodiscard]] auto GetSize() const { return size_; }
  [[nodiscard]] auto GetSources() const { return sources_; }
  [[nodiscard]] auto GetTotalUsage() const { return total_usage_; }
  [[nodiscard]] std::unordered_set<std::size_t> GetSinks() const;
  [[nodiscard]] std::vector<std::pair<std::size_t, std::size_t>> GetEdgeList()
      const;

  Graph* AddNode(std::unique_ptr<Node> node,
                 const std::vector<std::size_t>& parents = {});

  void CalculateThroughput();

 private:
  std::vector<std::vector<std::size_t>> adjacency_list_;
  std::vector<std::shared_ptr<Node>> nodes_;

  std::size_t size_ = 0;

  Stats total_usage_{};

  std::unordered_set<std::size_t> sources_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_GRAPH_H_
