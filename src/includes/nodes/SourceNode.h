#ifndef SRC_INCLUDES_NODES_SOURCENODE_H_
#define SRC_INCLUDES_NODES_SOURCENODE_H_

#include "Node.h"
#include "Stats.h"

namespace yql_model {

class SourceNode : public Node {
 private:
  static constexpr double cpu_usage_multiplier_ = 0.2;

 public:
  explicit SourceNode(double rate) : rate_(rate) { SetInputVolume(rate); }

  double GetOutputVolume() override { return rate_; }
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
  double rate_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_NODES_SOURCENODE_H_
