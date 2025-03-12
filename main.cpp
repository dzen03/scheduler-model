#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
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
#include "scheduler/SingleHost.h"
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

    std::vector<yql_model::Graph::Link> parents(prev_layer.size());

    for (std::size_t i = 0; i < parents.size(); ++i) {
      parents[i] = {.id = prev_layer[i],
                    .volume = 1};  // TODO(dzen): make not equal
    }

    const int layer = layer_count(gen);
    prev_layer.clear();
    prev_layer.resize(layer);

    for (int i = 0; i < layer; ++i) {
      prev_layer[i] = node_id++;
      graph.AddNode(
          std::make_unique<yql_model::FilterNode>(double_filter(gen) / layer),
          parents);
    }

    remaining_size -= layer;
  }

  const auto& sinks = graph.GetSinks();

  std::vector<yql_model::Graph::Link> parents(sinks.size() - 1);

  std::size_t index = 0;
  for (const auto& parent : sinks) {
    if (parent == node_id) {
      continue;
    }
    parents[index++] = {.id = parent, .volume = 1};
  }
  graph.AddNode(std::make_unique<yql_model::SinkNode>(), parents);

  return graph;
}

void generateNodes(
    const std::vector<std::shared_ptr<yql_model::System>>& systems,  // NOLINT
    int count) {
  try {
    for (int i = 0; i < count; ++i) {
      auto graph = GenerateGraph();

      graph.CalculateThroughput();

      for (const auto& node : graph.GetNodes()) {
        std::cout << "Node #" << node->GetNodeId() << ": "
                  << node->GetInputVolume() << " -> " << node->GetOutputVolume()
                  << "\n";
      }

      for (const auto& system : systems) {
        if (!system->ExecuteQuery(graph.GetCopy())) {
          throw std::runtime_error("cannot emplace");
        }
      }
    }
  } catch (std::exception& exc) {
    std::cerr << exc.what();
  }
}

auto init() {
  std::vector<std::shared_ptr<yql_model::System>> systems = {
      std::make_shared<yql_model::System>(
          std::make_unique<yql_model::SingleHost>()),
      std::make_shared<yql_model::System>(
          std::make_unique<yql_model::RoundRobin>())};

  const auto& servers_count = params.get_servers_count();
  const auto& server_stat = params.get_servers_stat();

  for (auto& system : systems) {
    for (int i = 0; i < servers_count; ++i) {
      system->AddServer(yql_model::Server(server_stat));
    }
  }

  return systems;
}

// TODO(dzen): decompose
// NOLINTNEXTLINE
void server_thread(std::vector<std::shared_ptr<yql_model::System>>& systems) {
  simple_http_server::Server server("127.0.0.1", 8080);
  server.MapUrl(
      "/api/reset",
      // NOLINTNEXTLINE
      [&systems](const simple_http_server::Request&) noexcept(false) {
        systems = init();
        return simple_http_server::Response(
            simple_http_server::Response::HttpStatusCodes::OK);
      },
      true);
  server.MapUrl(
      "/api/addnodes",
      // NOLINTNEXTLINE
      [&systems](const simple_http_server::Request& request) noexcept(false) {
        generateNodes(systems, std::stoi(request.GetArguments().at("count")));
        return simple_http_server::Response(
            simple_http_server::Response::HttpStatusCodes::OK);
      },
      true);

  server.MapUrl(
      "/api/params",
      // NOLINTNEXTLINE
      [](const simple_http_server::Request& request) noexcept(false) {
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
      // NOLINTNEXTLINE
      [&systems = std::as_const(systems)](
          const simple_http_server::Request&) noexcept(false) {
        std::ostringstream res;

        res << '[';
        for (const auto& system : systems) {
          res << R"({"name":")" << system->GetSchedulerName()
              << R"(","servers":[)";
          for (const auto& server : system->GetServers()) {
            res << R"({"usage":)" << server.GetUsages().ToJson()
                << R"(,"limits":)" << server.GetLimits().ToJson() << "},";
          }
          res.seekp(-1, std::ios_base::cur);
          res << "]},";
        }
        res.seekp(-1, std::ios_base::cur);
        res << "]";

        return simple_http_server::Response(
            simple_http_server::Response::HttpStatusCodes::OK, res.str(),
            {{"Content-Type", "application/json"}});
      },
      true); /*
    server.MapUrl(
        "/api/nodes",
        [systems](const simple_http_server::Request& request) {
          std::ostringstream res;

          const auto server = request.GetArguments().at("server");
          const std::size_t server_id = std::stoull(server);
          res << '[';
          for (const auto& [ind, stats] :
               system->GetServerById(server_id).GetNodes()) {
            res << R"({"id":)" << ind << R"(,"usage":)" << stats.ToJson() <<
    "},";
          }
          res.seekp(-1, std::ios_base::cur);
          res << ']';

          return simple_http_server::Response(
              simple_http_server::Response::HttpStatusCodes::OK, res.str(),
              {{"Content-Type", "application/json"}});
        },
        true);*/
  server.MapUrl(
      "/api/graphs",
      // NOLINTNEXTLINE
      [&systems =
           std::as_const(systems)](const simple_http_server::Request& request) {
        std::ostringstream res;

        const auto& arguments = request.GetArguments();

        std::optional<std::size_t> index = std::nullopt;

        if (arguments.contains("id")) {
          const auto index_str = arguments.at("id");
          index = {std::stoull(index_str)};
        }

        res << '[';
        for (const auto& system : systems) {
          auto graphs = (!index ? system->GetGraphs()
                                : std::vector<yql_model::Graph>{
                                      system->GetGraphById(index.value())});
          res << R"({"name":")" << system->GetSchedulerName()
              << R"(","graphs":[)";

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
              res << R"({"source":")" << edge.source_id << R"(","target":")"
                  << edge.destination_id << R"(","volume":)" << edge.volume
                  << "},";
            }
            res.seekp(-1, std::ios_base::cur);
            res << "]},";
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
    std::random_device random_device{};
    const std::mt19937 generator{random_device()};

    auto systems = init();

    std::thread server_th(server_thread, std::ref(systems));

    while (true) {
      try {
        const auto start = std::chrono::steady_clock::now();
        for (const auto& system : systems) {
          system->Tick();
        }
        const auto end = std::chrono::steady_clock::now();
        using namespace std::chrono_literals;
        std::cerr << 1000. / ((end - start) / 1.ms) << "\n";  //  NOLINT
        std::this_thread::sleep_until(start + 50ms);          //  NOLINT
      } catch (std::exception& exc) {
        std::cerr << exc.what() << "\n";
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
