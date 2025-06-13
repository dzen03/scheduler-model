#include "scheduler/New.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "Graph.h"
#include "Server.h"
#include "Stats.h"
#include "nodes/Node.h"

namespace yql_model {
bool New::Schedule(const Graph& graph, std::vector<Server>& servers) {
  if (graph.GetSize() <= params_->get_max_count_local()) {
    return singlehost_.Schedule(graph, servers);
  }

  const auto& nodes_count = graph.GetSize();
  auto adj = graph.GetAdjacencyList();

  for (auto& edges : adj) {
    edges.erase(std::remove_if(edges.begin(), edges.end(),
                               [&](Graph::Link& pr) {
                                 return pr.size <
                                        params_->get_max_distributed_traffic();
                               }),
                edges.end());
  }

  for (const auto& q : adj) {
    for (const auto& [id, _, size] : q) {
      std::cout << id << '/' << size << ' ';
    }
    std::cout << '\n';
  }

  std::vector<int> comp(nodes_count, -1);
  int compCnt = 0;
  for (std::size_t i = 0; i < nodes_count; ++i) {
    if (comp[i] >= 0) continue;
    // BFS
    std::vector<std::size_t> q{i};
    comp[i] = compCnt;
    for (size_t qi = 0; qi < q.size(); ++qi) {
      int u = q[qi];
      for (auto& [v, volume, size] : adj[u]) {
        if (comp[v] < 0) {
          comp[v] = compCnt;
          q.push_back(v);
        }
      }
    }
    ++compCnt;
  }
  struct Cluster {
    std::vector<std::size_t> ids;
    Stats stats = {};
  };
  std::vector<Cluster> clusters(compCnt);
  for (std::size_t i = 0; i < nodes_count; ++i) {
    clusters[comp[i]].ids.push_back(i);
  }

  for (auto& cl : clusters) {
    for (int idx : cl.ids) {
      const auto& node = graph[idx];

      cl.stats = cl.stats + node->GetUsage();
    }
  }

  std::vector<std::size_t> sorted_server_ids(servers.size());

  std::iota(sorted_server_ids.begin(), sorted_server_ids.end(), 0);

  auto update_servers = [&servers, &sorted_server_ids]() {
    std::sort(
        sorted_server_ids.begin(), sorted_server_ids.end(),
        [&](std::size_t left, std::size_t right) {
          return (servers[left].GetCpuUsage() < servers[right].GetCpuUsage());
        });
  };

  auto try_schedule = [&](bool use_cpu) {
    for (const auto& cluster : clusters) {
      update_servers();
      std::vector<std::shared_ptr<Node>> nodes;

      for (int idx : cluster.ids) {
        nodes.emplace_back(graph[idx]);
      }

      bool emplaced = false;

      for (auto& server_id : sorted_server_ids) {
        if ((servers[server_id].GetMemoryUsage() + cluster.stats.GetMemory() <
             servers[server_id].GetMemoryLimit()) &&
            (!use_cpu ||
             servers[server_id].GetCpuUsage() + cluster.stats.GetCpu() <
                 servers[server_id].GetCpuLimit())) {
          emplaced = servers[server_id].EmplaceNodes(
              nodes, std::vector<Node::NetworkMode>(nodes.size(), Node::NONE),
              server_id);

          if (emplaced) {
            break;
          }
        }
      }
      if (!emplaced) {
        return false;
      }
    }
    return true;
  };

  if (try_schedule(true)) {
    return true;
  }

  std::cerr << "Cannot emplace with cpu limits";

  return try_schedule(false);
}

bool New::Remove(const Graph& graph, std::vector<Server>& servers) {
  for (const auto& node : graph.GetNodes()) {
    servers[node->GetServer()].RemoveNode(node->GetNodeId());
    node->SetServer(-1);
  }
  return true;
}

std::string New::GetName() const { return "New"; };

}  // namespace yql_model
