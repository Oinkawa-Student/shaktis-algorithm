#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <set>
#include <algorithm>

using namespace std;
using namespace std::chrono;

struct Edge { int to; int id; };
struct Circuit {
    vector<int> path;
    vector<int> footprint;
    int total_edges;
};
struct TrieNode { vector<pair<int, int>> children; };

// Global State
int n, m;
int MAX_VISITS = 2; // Edge capacity
vector<vector<Edge>> adj;
vector<int> edge_visits;
vector<int> current_path;

vector<Circuit> all_circuits;
vector<TrieNode> trie;

void dfs(int u, int entry_node) {
    for (const Edge& e : adj[u]) {
        if (edge_visits[e.id] >= MAX_VISITS) continue;
        if (e.to < entry_node) continue;

        edge_visits[e.id]++;
        current_path.push_back(e.to);

        if (e.to == entry_node) {
            all_circuits.push_back({current_path, edge_visits, (int)current_path.size() - 1});
        }

        dfs(e.to, entry_node);

        edge_visits[e.id]--;
        current_path.pop_back();
    }
}

int create_node() {
    trie.push_back(TrieNode());
    return trie.size() - 1;
}

bool has_superset(int u, const vector<int>& B, int b_idx) {
    if (b_idx == B.size()) return true;
    for (const auto& child : trie[u].children) {
        if (child.first == B[b_idx]) {
            if (has_superset(child.second, B, b_idx + 1)) return true;
        } else if (child.first < B[b_idx]) {
            if (has_superset(child.second, B, b_idx)) return true;
        } else {
            break;
        }
    }
    return false;
}

void insert_trie(int u, const vector<int>& A, int a_idx) {
    if (a_idx == A.size()) return;
    for (auto& child : trie[u].children) {
        if (child.first == A[a_idx]) {
            insert_trie(child.second, A, a_idx + 1);
            return;
        }
    }
    int next_u = create_node();
    trie[u].children.push_back({A[a_idx], next_u});
    sort(trie[u].children.begin(), trie[u].children.end());
    insert_trie(next_u, A, a_idx + 1);
}

vector<pair<int, int>> generate_random_graph(int nodes, int edges_count) {
    vector<pair<int, int>> edges;
    set<pair<int, int>> existing_edges;
    random_device rd; mt19937 gen(rd());
    uniform_int_distribution<> dist(1, nodes);

    while (edges.size() < edges_count) {
        int u = dist(gen); int v = dist(gen);
        if (u != v && existing_edges.find({u, v}) == existing_edges.end()) {
            existing_edges.insert({u, v}); edges.push_back({u, v});
        }
    }
    return edges;
}

int main() {
    ofstream csv_file("benchmark_v2.csv");
    csv_file << "Nodes,Edges,Raw_Circuits,Maximal_Circuits,DFS_Time_ms,Filter_Time_ms\n";

    // Keep N smaller here because MAX_VISITS=2 expands the search space factorially
    vector<int> node_counts = {8, 10, 12};
    vector<double> edge_densities = {1.5, 2.0, 2.5};

    cout << "--- Starting V2 (Set-Trie) Benchmarks ---\n";

    for (int current_n : node_counts) {
        for (double density : edge_densities) {
            n = current_n;
            m = static_cast<int>(n * density);
            
            // 1. Reset Global State
            adj.assign(n + 1, vector<Edge>());
            edge_visits.assign(m, 0);
            all_circuits.clear();
            current_path.clear();
            trie.clear();

            // 2. Generate Graph
            vector<pair<int, int>> edges = generate_random_graph(n, m);
            for (int i = 0; i < m; ++i) {
                adj[edges[i].first].push_back({edges[i].second, i});
            }

            // 3. MEASURE DFS TIME
            auto start_dfs = high_resolution_clock::now();
            for (int i = 1; i <= n; ++i) {
                current_path.clear(); current_path.push_back(i);
                dfs(i, i);
            }
            auto stop_dfs = high_resolution_clock::now();
            auto dfs_duration = duration_cast<milliseconds>(stop_dfs - start_dfs);

            // 4. MEASURE TRIE FILTER TIME
            auto start_filter = high_resolution_clock::now();
            
            sort(all_circuits.begin(), all_circuits.end(), [](const Circuit& a, const Circuit& b) {
                return a.total_edges > b.total_edges;
            });

            create_node(); 
            vector<Circuit> maximal_circuits;

            for (const auto& circuit : all_circuits) {
                vector<int> B;
                for (int e = 0; e < m; ++e) {
                    for (int k = 0; k < circuit.footprint[e]; ++k) {
                        B.push_back(e); 
                    }
                }
                if (!has_superset(0, B, 0)) {
                    maximal_circuits.push_back(circuit);
                    insert_trie(0, B, 0);
                }
            }
            
            auto stop_filter = high_resolution_clock::now();
            auto filter_duration = duration_cast<milliseconds>(stop_filter - start_filter);

            // 5. Log Results
            csv_file << n << "," << m << "," << all_circuits.size() << "," 
                     << maximal_circuits.size() << "," << dfs_duration.count() << "," << filter_duration.count() << "\n";
            
            cout << "N=" << n << " M=" << m << " | Raw: " << all_circuits.size() 
                 << " -> Maximal: " << maximal_circuits.size() 
                 << " | DFS: " << dfs_duration.count() << "ms | Filter: " << filter_duration.count() << "ms\n";
        }
    }

    csv_file.close();
    cout << "V2 Benchmarks saved to benchmark_v2.csv\n";
    return 0;
}