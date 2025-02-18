#ifndef SRC_INCLUDES_GRAPH_H_
#define SRC_INCLUDES_GRAPH_H_

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <unordered_set>
#include <vector>

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
  [[nodiscard]] std::unordered_set<std::size_t> GetSinks() const;

  Graph* AddNode(std::unique_ptr<Node> node,
                 std::initializer_list<int> parents = {});

  void CalculateThroughput();

 private:
  std::vector<std::vector<std::size_t>> adjacency_list_;
  std::vector<std::shared_ptr<Node>> nodes_;

  std::size_t size_;

  std::unordered_set<std::size_t> sources_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_GRAPH_H_
