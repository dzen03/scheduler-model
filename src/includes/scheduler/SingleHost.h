#ifndef SRC_INCLUDES_SCHEDULER_SINGLEHOST_H_
#define SRC_INCLUDES_SCHEDULER_SINGLEHOST_H_

#include "Graph.h"
#include "IScheduler.h"

namespace yql_model {

class SingleHost : public IScheduler {
 public:
  bool Schedule(const Graph& graph, std::vector<Server>& servers) override;
  bool Remove(const Graph& graph, std::vector<Server>& servers) override;

  [[nodiscard]] std::string GetName() const override;

 private:
  std::size_t next_server_ = 0;
};
}  // namespace yql_model

#endif  // SRC_INCLUDES_SCHEDULER_SINGLEHOST_H_
