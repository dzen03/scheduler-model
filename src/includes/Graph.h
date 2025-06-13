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
  struct Link {
    std::size_t id = 0;
    double volume = 0;
    double size = 0;
  };
  struct FullLink {
    std::size_t source_id;
    std::size_t destination_id;
    double volume;
    double size;
  };
  explicit Graph(std::size_t size);

  Graph(const Graph& source) = default;

  Graph(Graph&& source) = default;

  Graph& operator=(const Graph& source) = default;
  Graph& operator=(Graph&& source) = default;

  ~Graph() = default;

  const std::shared_ptr<Node>& operator[](std::size_t idx) const {
    return nodes_[idx];
  }

  [[nodiscard]] auto GetNodes() const { return nodes_; }
  [[nodiscard]] auto GetSize() const { return size_; }
  [[nodiscard]] auto GetSources() const { return sources_; }
  [[nodiscard]] auto GetTotalUsage() const { return total_usage_; }
  [[nodiscard]] std::unordered_set<std::size_t> GetSinks() const;
  [[nodiscard]] std::vector<FullLink> GetEdgeList() const;
  [[nodiscard]] auto GetAdjacencyList() const { return adjacency_list_; }

  Graph* AddNode(std::unique_ptr<Node> node,
                 const std::vector<Link>& parents = {});

  void CalculateThroughput(bool dynamic = false);

  [[nodiscard]] Graph GetCopy() const;

 private:
  std::vector<std::vector<Link>> adjacency_list_;
  std::vector<std::shared_ptr<Node>> nodes_;

  std::size_t size_ = 0;

  Stats total_usage_{};

  std::unordered_set<std::size_t> sources_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_GRAPH_H_
