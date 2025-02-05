#include "Server.h"

#include <iostream>
#include <memory>

#include "nodes/Node.h"

namespace yql_model {
bool Server::EmplaceNode(const std::shared_ptr<Node> node, bool need_network) {
  bool res = false;

  // std::cout << usages_.GetCpu() << " " << limits_.GetCpu() << " "
  //           << node->GetUsage().GetCpu() << "\n"
  //           << usages_.GetMemory() << " " << limits_.GetMemory() << " "
  //           << node->GetUsage().GetMemory() << "\n"
  //           << usages_.GetNetwork() << " " << limits_.GetNetwork() << " "
  //           << node->GetUsage().GetNetwork() << "\n";

  if (usages_ + node->GetUsage(need_network) <= limits_) {
    usages_ = usages_ + node->GetUsage(need_network);
    res = true;
  }

  return res;
}
}  // namespace yql_model
