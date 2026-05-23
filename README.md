# Shakti's Algorithm

![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)
![Language: C++](https://img.shields.io/badge/Language-C++17-orange.svg)

This repository contains the C++ source code and benchmarking harnesses for **Shakti's Algorithm**, a novel Depth-First Search architecture optimized for enumerating bounded circuits in directed graphs. 

This codebase accompanies the paper: *"Shakti's Algorithm: Efficient Enumeration of Bounded Circuits Using Set-Trie Filtering"*.

## Overview
Traditional cycle-finding algorithms (like Johnson's) halt upon vertex repetition. Shakti's Algorithm shifts the bounding constraint from vertices to edges, allowing for the extraction of complex, self-intersecting closed walks. 

To resolve the $O(C^2 \cdot M)$ bottleneck of filtering subsumed topological sub-cycles, this implementation introduces a highly cache-friendly **Set-Trie optimization**, drastically reducing filtering time to $O(C \log C + C \cdot D_{Trie})$.

## Files in this Repository
* `shakti_nodes_only.cpp`: The base DFS generator for standard graph topologies (Capacity = 1).
* `shakti_edges_trie.cpp`: The full implementation featuring the Set-Trie optimization for bounded circuits (Capacity = k).
* `benchmark_v2.cpp`: The parameter-sweep harness used to generate the empirical execution data for the paper.

## Compilation and Execution
This code leverages standard C++17. For accurate benchmarking, it is highly recommended to compile with the `-O3` optimization flag.

```bash
# Compile the Set-Trie implementation
g++ -O3 shakti_edges_trie.cpp -o shakti_trie

# Execute
./shakti_trie
