#ifndef SRC_INCLUDES_NODES_SOURCENODE_H_
#define SRC_INCLUDES_NODES_SOURCENODE_H_

#include "Node.h"
#include "Stats.h"

namespace yql_model {

class SourceNode : public Node {
 private:
  static constexpr double cpu_usage_multiplier_ = 0.02;
  static constexpr double cpu_usage_multiplier_for_serialization_ = 0.03;

 public:
  explicit SourceNode(double rate) : rate_(rate) {
    SetInputVolume({.local = 0, .remote = rate});
  }

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
  Stats GetTrueUsage() override {
    const auto& input = GetInputVolume_();
    const auto& output = GetOutputVolume_();
    return Stats(
        {.cpu = (cpu_usage_multiplier_ * input.local) +
                (cpu_usage_multiplier_for_serialization_ * input.remote) +
                (cpu_usage_multiplier_for_serialization_ * output.remote),
         .memory = GetInputVolume(),
         .network = input.remote + output.remote,
         .disk = 0});
  }

  [[nodiscard]] std::shared_ptr<Node> GetCopy() const override {
    auto res = std::make_shared<SourceNode>(this->rate_);
    res->SetInputVolume(this->GetInputVolume_());

    return res;
  }

 private:
  double rate_;
};

}  // namespace yql_model

#endif  // SRC_INCLUDES_NODES_SOURCENODE_H_
