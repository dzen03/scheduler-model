#ifndef SRC_INCLUDES_SCHEDULER_NEW_H_
#define SRC_INCLUDES_SCHEDULER_NEW_H_

#include <memory>

#include "Graph.h"
#include "IScheduler.h"
#include "Parameters.h"
#include "scheduler/RoundRobin.h"
#include "scheduler/SingleHost.h"

namespace yql_model {

class New : public IScheduler {
 public:
  explicit New(std::shared_ptr<Parameters> params)
      : params_(std::move(params)) {}
  bool Schedule(const Graph& graph, std::vector<Server>& servers) override;
  bool Remove(const Graph& graph, std::vector<Server>& servers) override;

  [[nodiscard]] std::string GetName() const override;

 private:
  std::size_t next_server_ = 0;
  std::shared_ptr<Parameters> params_;
  RoundRobin roundrobin_{};
  SingleHost singlehost_{};
};
}  // namespace yql_model

#endif  // SRC_INCLUDES_SCHEDULER_NEW_H_
