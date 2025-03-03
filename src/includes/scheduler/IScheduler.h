#ifndef SRC_INCLUDES_SCHEDULER_ISCHEDULER_H_
#define SRC_INCLUDES_SCHEDULER_ISCHEDULER_H_

#include "Graph.h"
#include "Server.h"

namespace yql_model {

class IScheduler {
 public:
  struct Response {
    std::size_t node_id;
    std::size_t server_id;
  };

  IScheduler() = default;
  virtual ~IScheduler() = default;

  IScheduler(const IScheduler& source) = default;
  IScheduler(IScheduler&& source) = default;

  IScheduler& operator=(const IScheduler& source) = default;
  IScheduler& operator=(IScheduler&& source) = default;

  virtual bool Schedule(const Graph& graph, std::vector<Server>& servers) = 0;
  virtual bool Remove(const Graph& graph, std::vector<Server>& servers) = 0;
};
}  // namespace yql_model

#endif  // SRC_INCLUDES_SCHEDULER_ISCHEDULER_H_
