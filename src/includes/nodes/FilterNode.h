#ifndef SRC_INCLUDES_NODES_FILTERNODE_H_
#define SRC_INCLUDES_NODES_FILTERNODE_H_

#include "Node.h"
#include "Stats.h"

namespace yql_model {

class FilterNode : public Node {
 private:
  static constexpr double cpu_usage_multiplier_ = 0.5;

 public:
  explicit FilterNode(double filter = 1) : filter_(filter) {}

  double GetOutputVolume() override { return filter_ * GetInputVolume(); }
  Stats GetUsage() override { return GetUsage(true); }
  Stats GetUsage(bool need_network) override {
    return Stats(
        {.cpu = cpu_usage_multiplier_ * GetInputVolume(),
         .memory = GetInputVolume(),
         .network = (need_network ? GetInputVolume() + GetOutputVolume() : 0)});
  }

 private:
  double filter_;
};

using SinkNode = FilterNode;

}  // namespace yql_model

#endif  // SRC_INCLUDES_NODES_FILTERNODE_H_
