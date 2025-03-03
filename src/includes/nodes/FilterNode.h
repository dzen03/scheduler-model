#ifndef SRC_INCLUDES_NODES_FILTERNODE_H_
#define SRC_INCLUDES_NODES_FILTERNODE_H_

#include "Node.h"
#include "Stats.h"

namespace yql_model {

class FilterNode : public Node {
 private:
  static constexpr double cpu_usage_multiplier_ = 0.05;

 public:
  explicit FilterNode(double filter = 1) : filter_(filter) {}

  double GetOutputVolume() override { return filter_ * GetInputVolume(); }
  Stats GetUsage() override { return GetUsage(BOTH); }
  Stats GetUsage(NetworkMode need_network) override {
    double network_usage = 0;
    switch (need_network) {
      case BOTH:
        network_usage = GetInputVolume() + GetOutputVolume();
        break;
      case INPUT:
        network_usage = GetInputVolume();
        break;
      case OUTPUT:
        network_usage = GetOutputVolume();
        break;
      case NONE:
        break;
    }
    return Stats({.cpu = cpu_usage_multiplier_ * GetInputVolume(),
                  .memory = GetInputVolume(),
                  .network = network_usage,
                  .disk = 0});
  }

 private:
  double filter_;
};

using SinkNode = FilterNode;

}  // namespace yql_model

#endif  // SRC_INCLUDES_NODES_FILTERNODE_H_
