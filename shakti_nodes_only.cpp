#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

struct Edge {
    int to;
    int id;
};

int n, m;
vector<vector<Edge>> adj;
vector<uint8_t> used_edge; 
vector<int> current_path;
vector<vector<int>> valid_circuits;

void dfs(int u, int entry_node) {
    for (const Edge& e : adj[u]) {
        // Rule 1: Edges cannot be repeated
        if (used_edge[e.id]) continue;

        // Rule 2: Lexicographical Pruning
        if (e.to < entry_node) continue;

        // Take the step
        used_edge[e.id] = 1;
        current_path.push_back(e.to);

        // Circuit found
        if (e.to == entry_node) {
            valid_circuits.push_back(current_path);
        }

        // Recurse deeper
        dfs(e.to, entry_node);

        // Backtrack
        used_edge[e.id] = 0;
        current_path.pop_back();
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    n = 6;
    m = 9;
    
    adj.resize(n + 1);
    used_edge.assign(m, 0);

    vector<pair<int, int>> edges = {
        {1, 2}, {2, 3}, {3, 1}, 
        {1, 4}, {6, 4}, {5, 6}, 
        {5, 3}, {3, 4}, {4, 5}
    };

    for (int i = 0; i < m; ++i) {
        adj[edges[i].first].push_back({edges[i].second, i});
    }

    for (int i = 1; i <= n; ++i) {
        current_path.clear();
        current_path.push_back(i);
        dfs(i, i);
    }

    cout << "Total circuits found: " << valid_circuits.size() << "\n\n";
    for (const auto& circuit : valid_circuits) {
        for (size_t i = 0; i < circuit.size(); ++i) {
            cout << circuit[i] << (i == circuit.size() - 1 ? "" : " ");
        }
        cout << "\n";
    }

    return 0;
}