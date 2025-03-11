#include <algorithm>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "Directory.h"
#include "Graph.h"
#include "Parameters.h"
#include "Response.h"
#include "Server.h"
#include "Stats.h"
#include "System.h"
#include "nodes/FilterNode.h"
#include "nodes/SourceNode.h"
#include "scheduler/RoundRobin.h"
// #include "scheduler/SingleHost.h"
//

#include "server/libs/includes/Request.h"
#include "server/libs/includes/Server.h"

// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
namespace {

yql_model::Parameters params{};  // NOLINT

yql_model::Graph GenerateGraph() {
  std::random_device rand;
  std::mt19937 gen(rand());
  std::uniform_int_distribution<> int_size(params.get_graph_size().min,
                                           params.get_graph_size().max);
  std::uniform_int_distribution<> int_source_volume(
      params.get_source_volume().min, params.get_source_volume().max);
  std::uniform_real_distribution<> double_filter(
      params.get_filter_volume().min, params.get_filter_volume().max);

  const int total_size = int_size(gen);
  const int sources_size = std::max(1, total_size / 10);
  const int sinks_size = 1;
  int remaining_size = total_size - sources_size - sinks_size;
  auto graph = yql_model::Graph(total_size);

  std::size_t node_id = 0;

  std::vector<std::size_t> prev_layer(sources_size);

  for (int i = 0; i < sources_size; ++i) {
    prev_layer[i] = node_id++;
    graph.AddNode(
        std::make_unique<yql_model::SourceNode>(int_source_volume(gen)));
  }

  while (remaining_size > 0) {
    std::uniform_int_distribution<> layer_count(1, std::min(5, remaining_size));

    const std::vector<std::size_t> parents(prev_layer);

    const int layer = layer_count(gen);
    prev_layer.clear();
    prev_layer.resize(layer);

    for (int i = 0; i < layer; ++i) {
      prev_layer[i] = node_id++;
      graph.AddNode(
          std::make_unique<yql_model::FilterNode>(double_filter(gen) / layer),
          parents);
    }  // TODO(dzen): make not equal

    remaining_size -= layer;
  }

  const auto& sinks = graph.GetSinks();

  std::vector<std::size_t> parents(sinks.size() - 1);

  std::size_t index = 0;
  for (const auto& parent : sinks) {
    if (parent == node_id) {
      continue;
    }
    parents[index++] = parent;
  }
  graph.AddNode(std::make_unique<yql_model::SinkNode>(), parents);

  return graph;
}

void server_thread(std::shared_ptr<yql_model::System> system) {  // NOLINT
  simple_http_server::Server server("127.0.0.1", 8080);

  server.MapUrl(
      "/api/params",
      [system](const simple_http_server::Request& request) {
        if (request.GetType() == simple_http_server::Request::GET) {
          return simple_http_server::Response(
              simple_http_server::Response::HttpStatusCodes::OK,
              params.ToJson(), {{"Content-Type", "application/json"}});
        }
        if (request.GetType() == simple_http_server::Request::POST) {
          params.Update(request.GetArguments());
          return simple_http_server::Response(simple_http_server::Response::OK);
        }
        return simple_http_server::Response(
            simple_http_server::Response::NOT_FOUND);
      },
      true);

  server.MapUrl(
      "/api/servers",
      [system](const simple_http_server::Request&) {
        std::ostringstream res;

        res << '[';
        for (const auto& server : system->GetServers()) {
          res << R"({"usage":)" << server.GetUsages().ToJson()
              << R"(,"limits":)" << server.GetLimits().ToJson() << "},";
        }
        res.seekp(-1, std::ios_base::cur);
        res << "]";

        return simple_http_server::Response(
            simple_http_server::Response::HttpStatusCodes::OK, res.str(),
            {{"Content-Type", "application/json"}});
      },
      true);
  server.MapUrl(
      "/api/nodes",
      [system](const simple_http_server::Request& request) {
        std::ostringstream res;

        const auto server = request.GetArguments().at("server");
        const std::size_t server_id = std::stoull(server);
        res << '[';
        for (const auto& [ind, stats] :
             system->GetServerById(server_id).GetNodes()) {
          res << R"({"id":)" << ind << R"(,"usage":)" << stats.ToJson() << "},";
        }
        res.seekp(-1, std::ios_base::cur);
        res << ']';

        return simple_http_server::Response(
            simple_http_server::Response::HttpStatusCodes::OK, res.str(),
            {{"Content-Type", "application/json"}});
      },
      true);
  server.MapUrl(
      "/api/graphs",
      [system](const simple_http_server::Request& request) {
        std::ostringstream res;

        std::vector<yql_model::Graph> graphs;

        const auto& arguments = request.GetArguments();
        if (arguments.contains("id")) {
          const auto index_str = arguments.at("id");
          const std::size_t index = std::stoull(index_str);

          graphs = {system->GetGraphById(index)};
        } else {
          graphs = system->GetGraphs();
        }

        res << '[';
        for (const auto& graph : graphs) {
          res << R"({"nodes":[)";
          for (const auto& node : graph.GetNodes()) {
            res << R"({"id":")" << node->GetNodeId() << R"(","val":)"
                << node->GetOutputVolume() << R"(,"server":)"
                << node->GetServer() << "},";
          }
          res.seekp(-1, std::ios_base::cur);
          res << R"(],"links":[)";
          for (const auto& edge : graph.GetEdgeList()) {
            res << R"({"source":")" << edge.first << R"(","target":")"
                << edge.second << "\"},";
          }
          res.seekp(-1, std::ios_base::cur);
          res << "]},";
        }
        res.seekp(-1, std::ios_base::cur);
        res << ']';

        return simple_http_server::Response(
            simple_http_server::Response::HttpStatusCodes::OK, res.str(),
            {{"Content-Type", "application/json"}});
      },
      true);

  server.MapDirectory("/front", simple_http_server::Directory(
                                    std::filesystem::current_path() / "html"));

  server.Start();
}

}  // namespace

int main() {
  try {
    std::random_device rand;
    std::mt19937 gen(rand());

    // auto graph = yql_model::Graph(6);
    //
    // graph
    //     .AddNode(std::make_unique<yql_model::SourceNode>(10))            // 0
    //     ->AddNode(std::make_unique<yql_model::SourceNode>(20))           // 1
    //     ->AddNode(std::make_unique<yql_model::FilterNode>(0.1), {0, 1})  // 2
    //     ->AddNode(std::make_unique<yql_model::FilterNode>(0.5), {2})     // 3
    //     ->AddNode(std::make_unique<yql_model::FilterNode>(0.5), {2})     // 4
    //     ->AddNode(std::make_unique<yql_model::SinkNode>(), {3, 4});      // 5
    auto system = std::make_shared<yql_model::System>(
        std::make_unique<yql_model::RoundRobin>());

    // system->AddServer(yql_model::Server(params.GetServerStat()))
    //     ->AddServer(yql_model::Server(
    //         Stats({.cpu = 34 * 100, .memory = 100 * 1000, .network =
    //         10000})));

    auto servers_count = params.get_servers_count();
    auto server_stat = params.get_servers_stat();

    for (int i = 0; i < servers_count; ++i) {
      system->AddServer(yql_model::Server(server_stat));
    }

    // const auto metrics = yql_model::Metrics(system);
    //
    std::thread server_th(server_thread, system);

    while (true) {
      try {
        auto graph = GenerateGraph();

        graph.CalculateThroughput();

        for (const auto& node : graph.GetNodes()) {
          std::cout << "Node #" << node->GetNodeId() << ": "
                    << node->GetInputVolume() << " -> "
                    << node->GetOutputVolume() << "\n";
        }

        if (!system->ExecuteQuery(graph)) {
          throw std::runtime_error("cannot emplace");
        }
        // system->RemoveQuery(graph);
        // for (const auto& server : system->GetServers()) {
        //   std::cout << "cpu: " << server.GetCpuUsage() << " of "
        //             << server.GetCpuLimit() << " = "
        //             << 100 * server.GetCpuUsage() / server.GetCpuLimit()
        //             << "%\nmemory: " << server.GetMemoryUsage() << " of "
        //             << server.GetMemoryLimit() << " = "
        //             << 100 * server.GetMemoryUsage() /
        //             server.GetMemoryLimit()
        //             << "%\nnetwork: " << server.GetNetworkUsage() << " of "
        //             << server.GetNetworkLimit() << " = "
        //             << 100 * server.GetNetworkUsage() /
        //             server.GetNetworkLimit()
        //             << "%\n";
        // }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(50ms);  // NOLINT
      } catch (std::exception& exc) {
        std::cerr << exc.what() << "\n";
        break;
        std::uniform_int_distribution<std::size_t> random_query(
            0, system->GetGraphsSize() - 1);
        const std::size_t ind = random_query(gen);
        std::cerr << "removing from 0 to " << system->GetGraphsSize() - 1
                  << ": " << ind << "\n";
        // for (const auto& server : system->GetServers()) {
        //   std::cout << "cpu: " << server.GetCpuUsage() << " of "
        //             << server.GetCpuLimit() << " = "
        //             << 100 * server.GetCpuUsage() / server.GetCpuLimit()
        //             << "%\nmemory: " << server.GetMemoryUsage() << " of "
        //             << server.GetMemoryLimit() << " = "
        //             << 100 * server.GetMemoryUsage() /
        //             server.GetMemoryLimit()
        //             << "%\nnetwork: " << server.GetNetworkUsage() << " of "
        //             << server.GetNetworkLimit() << " = "
        //             << 100 * server.GetNetworkUsage() /
        //             server.GetNetworkLimit()
        //             << "%\n";
        // }
        while (system->GetGraphsSize() > 0) {
          system->RemoveQuery(0);
        }
        for (const auto& server : system->GetServers()) {
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
        // break;
      }
    }
    server_th.join();
  } catch (std::exception& exc) {
    std::cerr << exc.what() << "\n";
  }

  return 0;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
// NOLINTEND(readability-magic-numbers)
