#ifndef SRC_INCLUDES_SYSTEM_H_
#define SRC_INCLUDES_SYSTEM_H_

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

  void ExecuteQuery(const Graph& graph);

  [[nodiscard]] auto GetServers() const { return servers_; }

 private:
  std::vector<Server> servers_;
  std::unique_ptr<IScheduler> scheduler_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_SYSTEM_H_
