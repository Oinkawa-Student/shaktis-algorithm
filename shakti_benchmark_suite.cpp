#include <iostream>
#include <vector>
#include <chrono>
#include <cstdio>
#include <algorithm>
#include <unordered_map>

using namespace std;
using namespace std::chrono;

// 1. HARDWARE-OPTIMIZED DATA STRUCTURES

// Compressed Sparse Row (CSR) Graph
// Maximizes L1/L2 cache hits by storing all edges contiguously.
struct CSRGraph {
    int num_nodes;
    int num_edges;
    vector<int> head;  
    vector<int> edges; 
};

// Set-Trie Node for O(C^2 * M) Subsumption Filtering
// Filters out cycles that are just permutations of the same topological set.
struct TrieNode {
    unordered_map<int, TrieNode*> children;
    bool is_end_of_circuit = false;
    
    // Memory cleanup
    ~TrieNode() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

// 2. ZERO-OVERHEAD FAST I/O PARSER

CSRGraph load_snap_graph_fast(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        cerr << "[ERROR] Could not open " << filename << ". Did you download and extract it?\n";
        exit(1);
    }

    vector<pair<int, int>> temp_edges;
    int max_node = 0;
    char buf[1 << 16]; 
    int from = -1, to = -1;
    
    cout << "[INFO] Parsing raw edge list from " << filename << "..." << flush;
    while (fgets(buf, sizeof(buf), file)) {
        if (buf[0] == '#') continue; 
        
        sscanf(buf, "%d %d", &from, &to);
        temp_edges.push_back({from, to});
        
        if (from > max_node) max_node = from;
        if (to > max_node) max_node = to;
    }
    fclose(file);
    cout << " Done.\n";

    cout << "[INFO] Constructing cache-friendly CSR graph..." << flush;
    int num_nodes = max_node + 1;
    CSRGraph g;
    g.num_nodes = num_nodes;
    g.num_edges = temp_edges.size();
    g.head.assign(num_nodes + 1, 0);
    g.edges.resize(g.num_edges);

    vector<int> degree(num_nodes, 0);
    for (const auto& e : temp_edges) degree[e.first]++;
    for (int i = 0; i < num_nodes; ++i) g.head[i + 1] = g.head[i] + degree[i];

    vector<int> cur_head = g.head;
    for (const auto& e : temp_edges) {
        g.edges[cur_head[e.first]++] = e.second;
    }
    cout << " Done.\n";

    return g;
}

// 3. SHAKTI'S ALGORITHM ENGINE

class ShaktiEngine {
private:
    const CSRGraph& g;
    vector<bool> visited;
    vector<int> current_path;
    TrieNode* trie_root;
    long long total_circuits_evaluated = 0;
    long long unique_circuits_found = 0;
    int max_capacity; 

    // The Set-Trie Insertion & Subsumption Check
    // Returns true if the circuit is a novel topological set.
    bool filter_and_insert_trie(const vector<int>& cycle) {
        // Sort to represent the topological nodes as a canonical set
        vector<int> canonical_set = cycle;
        sort(canonical_set.begin(), canonical_set.end());

        TrieNode* curr = trie_root;
        for (int node : canonical_set) {
            if (curr->children.find(node) == curr->children.end()) {
                curr->children[node] = new TrieNode();
            }
            curr = curr->children[node];
        }

        if (!curr->is_end_of_circuit) {
            curr->is_end_of_circuit = true;
            return true; // It's a new, un-subsumed set
        }
        return false; // Subsumed / Duplicate topology
    }

    void dfs_enumerate(int u, int start_node, int depth) {
        // Capacity constraint (Bounded depth)
        if (depth > max_capacity) return;

        visited[u] = true;
        current_path.push_back(u);

        // Hardware-optimized contiguous array traversal
        int edge_start = g.head[u];
        int edge_end = g.head[u + 1];
        
        for (int i = edge_start; i < edge_end; ++i) {
            int v = g.edges[i];

            if (v == start_node && current_path.size() > 1) {
                total_circuits_evaluated++;
                // Route through Set-Trie to filter subsumed topologies
                if (filter_and_insert_trie(current_path)) {
                    unique_circuits_found++;
                }
            } 
            else if (!visited[v] && v > start_node) { 
                // "v > start_node" optimization breaks symmetry 
                // and prevents re-evaluating the same cycle shifted.
                dfs_enumerate(v, start_node, depth + 1);
            }
        }

        current_path.pop_back();
        visited[u] = false;
    }

public:
    ShaktiEngine(const CSRGraph& graph, int capacity) : g(graph), max_capacity(capacity) {
        visited.assign(g.num_nodes, false);
        trie_root = new TrieNode();
    }

    ~ShaktiEngine() {
        delete trie_root;
    }

    void run() {
        for (int i = 0; i < g.num_nodes; ++i) {
            if (g.head[i + 1] > g.head[i]) { // Only start if node has outbound edges
                dfs_enumerate(i, i, 1);
            }
        }
    }
    
    long long get_unique_count() { return unique_circuits_found; }
    long long get_total_evaluated() { return total_circuits_evaluated; }
};

// 4. BENCHMARK HARNESS

int main() {
    cout << "  SHAKTI'S ALGORITHM : SET-TRIE PERFORMANCE SUITE   \n";

    // 1. I/O & Memory Allocation Phase
    auto start_io = high_resolution_clock::now();
    CSRGraph graph = load_snap_graph_fast("web-Google.txt");
    auto end_io = high_resolution_clock::now();
    double io_time = duration_cast<milliseconds>(end_io - start_io).count() / 1000.0;
    
    cout << "\n[DATASET METRICS]\n";
    cout << "Nodes      : " << graph.num_nodes << "\n";
    cout << "Edges      : " << graph.num_edges << "\n";
    cout << "Parse Time : " << io_time << " seconds\n";

    // 2. Algorithm Execution Phase
    int capacity_limit = 5; // The bounding constraint for circuit length
    ShaktiEngine engine(graph, capacity_limit);

    cout << "\n[INFO] Initiating Enumeration (Capacity = " << capacity_limit << ")...\n";

    auto start_algo = high_resolution_clock::now();
    engine.run();
    auto end_algo = high_resolution_clock::now();
    
    double algo_time = duration_cast<milliseconds>(end_algo - start_algo).count() / 1000.0;

    // 3. Output Conference-Grade Results
    cout << "\n[BENCHMARK RESULTS]\n";
    cout << "Execution Time       : " << algo_time << " seconds\n";
    cout << "Total Cycles Hit     : " << engine.get_total_evaluated() << "\n";
    cout << "Unique Sets Filtered : " << engine.get_unique_count() << "\n";

    return 0;
}