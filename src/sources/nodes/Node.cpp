#include "nodes/Node.h"

#include <cstddef>

namespace {
// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
std::size_t current_node_id = 0;
}  // namespace

namespace yql_model {
Node::Node() : node_id_(current_node_id++) {}
}  // namespace yql_model
