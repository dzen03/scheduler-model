#ifndef SRC_INCLUDES_SYSTEM_H_
#define SRC_INCLUDES_SYSTEM_H_

#include <iostream>
#include <memory>
#include <vector>

#include "Graph.h"
#include "Server.h"
#include "scheduler/IScheduler.h"

namespace yql_model {

class System {
 public:
  explicit System(std::unique_ptr<IScheduler> scheduler)
      : scheduler_(std::move(scheduler)) {}
  System* AddServer(const Server& server);

  bool ExecuteQuery(const Graph& graph);
  bool RemoveQuery(std::size_t index);

  [[nodiscard]] auto GetServers() const { return servers_; }
  [[nodiscard]] auto GetServerById(std::size_t ind) const {
    return servers_[ind];
  }
  [[nodiscard]] auto GetGraphs() const { return graphs_; }
  [[nodiscard]] auto GetGraphById(std::size_t ind) const {
    return graphs_[ind];
  }
  [[nodiscard]] auto GetGraphsSize() const {
    std::cerr << graphs_.size() << "\n";
    return graphs_.size();
  }

  [[nodiscard]] auto GetSchedulerName() const { return scheduler_->GetName(); }

  void Tick();

  void ReexecuteAll();

  void Reset() {
    servers_.clear();
    graphs_.clear();
  }

 private:
  std::vector<Server> servers_;
  std::vector<Graph> graphs_;
  std::unique_ptr<IScheduler> scheduler_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_SYSTEM_H_
