#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Edge {
    int to;
    int id;
};

// Represents a discovered path and its edge footprint
struct Circuit {
    vector<int> path;
    vector<int> footprint;
    int total_edges;
};

// Flat memory pool Trie Node
struct TrieNode {
    vector<pair<int, int>> children; 
};

int n, m;
int MAX_VISITS = 2; // The capacity rule

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

// THE SET-TRIE FILTER PIPELINE
int create_node() {
    trie.push_back(TrieNode());
    return trie.size() - 1;
}

bool has_superset(int u, const vector<int>& B, int b_idx) {
    if (b_idx == B.size()) return true;

    for (const auto& child : trie[u].children) {
        int edge_id = child.first;
        int next_u = child.second;

        if (edge_id == B[b_idx]) {
            if (has_superset(next_u, B, b_idx + 1)) return true;
        } 
        else if (edge_id < B[b_idx]) {
            if (has_superset(next_u, B, b_idx)) return true;
        } 
        else {
            break; // Prune: edge_id > B[b_idx]
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

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    n = 6;
    m = 9;
    
    adj.resize(n + 1);
    edge_visits.assign(m, 0);

    vector<pair<int, int>> edges = {
        {1, 2}, {2, 3}, {3, 1}, 
        {1, 4}, {6, 4}, {5, 6}, 
        {5, 3}, {3, 4}, {4, 5}
    };

    for (int i = 0; i < m; ++i) {
        adj[edges[i].first].push_back({edges[i].second, i});
    }

    // 1. Generate all possible bounded circuits
    for (int i = 1; i <= n; ++i) {
        current_path.clear();
        current_path.push_back(i);
        dfs(i, i);
    }

    // 2. Sort by length descending for the Trie pipeline
    sort(all_circuits.begin(), all_circuits.end(), [](const Circuit& a, const Circuit& b) {
        return a.total_edges > b.total_edges;
    });

    // 3. Initialize Trie and Filter
    trie.clear();
    create_node(); // Root node is index 0
    
    vector<Circuit> maximal_circuits;

    for (const auto& circuit : all_circuits) {
        vector<int> B;
        for (int e = 0; e < m; ++e) {
            for (int k = 0; k < circuit.footprint[e]; ++k) {
                B.push_back(e); // Creates naturally sorted multiset
            }
        }

        if (!has_superset(0, B, 0)) {
            maximal_circuits.push_back(circuit);
            insert_trie(0, B, 0);
        }
    }

    cout << "Raw circuits generated: " << all_circuits.size() << "\n";
    cout << "Maximal complex circuits after Trie filter: " << maximal_circuits.size() << "\n\n";
    
    for (const auto& circuit : maximal_circuits) {
        for (size_t i = 0; i < circuit.path.size(); ++i) {
            cout << circuit.path[i] << (i == circuit.path.size() - 1 ? "" : " ");
        }
        cout << "\n";
    }

    return 0;
}