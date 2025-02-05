#include "System.h"

#include "Server.h"

namespace yql_model {

System* System::AddServer(const Server& server) {
  servers_.emplace_back(std::move(server));

  return this;
}

void System::ExecuteQuery(const Graph& graph) {
  scheduler_->Schedule(graph, servers_);
}

}  // namespace yql_model
