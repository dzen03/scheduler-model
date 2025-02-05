#include <exception>
#include <iostream>
#include <memory>

#include "Graph.h"
#include "Server.h"
#include "Stats.h"
#include "System.h"
#include "nodes/FilterNode.h"
#include "nodes/SourceNode.h"
#include "scheduler/RoundRobin.h"

// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
int main() {
  try {
    auto graph = yql_model::Graph(6);

    graph
        .AddNode(std::make_unique<yql_model::SourceNode>(10))            // 0
        ->AddNode(std::make_unique<yql_model::SourceNode>(20))           // 1
        ->AddNode(std::make_unique<yql_model::FilterNode>(0.1), {0, 1})  // 2
        ->AddNode(std::make_unique<yql_model::FilterNode>(0.5), {0, 1})  // 3
        ->AddNode(std::make_unique<yql_model::FilterNode>(0.5), {0, 1})  // 4
        ->AddNode(std::make_unique<yql_model::SinkNode>(), {3, 4});      // 5

    graph.CalculateThroughput();

    int node_id = 0;
    for (const auto& node : graph.GetNodes()) {
      std::cout << "Node #" << node_id << ": " << node->GetInputVolume()
                << " -> " << node->GetOutputVolume() << "\n";
      ++node_id;
    }

    auto system = yql_model::System(std::make_unique<yql_model::RoundRobin>());

    system
        .AddServer(yql_model::Server(
            Stats({.cpu = 10000, .memory = 8000, .network = 1000})))
        ->AddServer(yql_model::Server(
            Stats({.cpu = 20000, .memory = 2000, .network = 100})));

    system.ExecuteQuery(graph);

    for (const auto& server : system.GetServers()) {
      std::cout << "cpu: " << server.GetCpuUsage() << " of "
                << server.GetCpuLimit() << " = "
                << 100 * server.GetCpuUsage() / server.GetCpuLimit()
                << "%\nmemory: " << server.GetMemoryUsage() << " of "
                << server.GetMemoryLimit() << " = "
                << 100 * server.GetMemoryUsage() / server.GetMemoryLimit()
                << "%\nnetwork: " << server.GetNetworkUsage() << " of "
                << server.GetNetworkLimit() << " = "
                << 100 * server.GetNetworkUsage() / server.GetNetworkLimit()
                << "%\n";
    }
  } catch (std::exception& exc) {
    std::cerr << exc.what() << "\n";
  }

  return 0;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
// NOLINTEND(readability-magic-numbers)
