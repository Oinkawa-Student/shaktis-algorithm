#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <set>
#include <cstdint>

using namespace std;
using namespace std::chrono;

struct Edge {
    int to;
    int id;
};

// Global State
int n, m;
vector<vector<Edge>> adj;
vector<uint8_t> used_edge; 
vector<int> current_path;
vector<vector<int>> valid_circuits;

void dfs(int u, int entry_node) {
    for (const Edge& e : adj[u]) {
        if (used_edge[e.id]) continue;
        if (e.to < entry_node) continue;

        used_edge[e.id] = 1;
        current_path.push_back(e.to);

        if (e.to == entry_node) {
            valid_circuits.push_back(current_path);
        }

        dfs(e.to, entry_node);

        used_edge[e.id] = 0;
        current_path.pop_back();
    }
}

// Generates a directed graph with no self-loops and no duplicate edges
vector<pair<int, int>> generate_random_graph(int nodes, int edges_count) {
    vector<pair<int, int>> edges;
    set<pair<int, int>> existing_edges;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, nodes);

    while (edges.size() < edges_count) {
        int u = dist(gen);
        int v = dist(gen);
        if (u != v && existing_edges.find({u, v}) == existing_edges.end()) {
            existing_edges.insert({u, v});
            edges.push_back({u, v});
        }
    }
    return edges;
}

int main() {
    ofstream csv_file("benchmark_v1.csv");
    csv_file << "Nodes,Edges,Circuits,DFS_Time_ms\n";

    // Parameter Sweep: Keep N relatively small to start so it doesn't TLE immediately
    vector<int> node_counts = {10, 15, 20};
    vector<double> edge_densities = {1.5, 2.0, 2.5}; 

    cout << "--- Starting V1 (Nodes Only) Benchmarks ---\n";

    for (int current_n : node_counts) {
        for (double density : edge_densities) {
            n = current_n;
            m = static_cast<int>(n * density);
            
            // 1. Reset Global State for this iteration
            adj.assign(n + 1, vector<Edge>());
            used_edge.assign(m, 0);
            valid_circuits.clear();
            current_path.clear();

            // 2. Generate Graph
            vector<pair<int, int>> edges = generate_random_graph(n, m);
            for (int i = 0; i < m; ++i) {
                adj[edges[i].first].push_back({edges[i].second, i});
            }

            // 3. MEASURE DFS TIME
            auto start_dfs = high_resolution_clock::now();
            
            for (int i = 1; i <= n; ++i) {
                current_path.clear();
                current_path.push_back(i);
                dfs(i, i);
            }
            
            auto stop_dfs = high_resolution_clock::now();
            auto dfs_duration = duration_cast<milliseconds>(stop_dfs - start_dfs);

            // 4. Log to CSV and Console
            csv_file << n << "," << m << "," << valid_circuits.size() << "," << dfs_duration.count() << "\n";
            cout << "N=" << n << " M=" << m << " | Circuits: " << valid_circuits.size() 
                 << " | DFS Time: " << dfs_duration.count() << "ms\n";
        }
    }

    csv_file.close();
    cout << "V1 Benchmarks saved to benchmark_v1.csv\n\n";
    return 0;
}