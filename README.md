# Shakti's Algorithm: Efficient Enumeration of Bounded Circuits

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.20422392.svg)](https://doi.org/10.5281/zenodo.20422392)
[![License: CC BY 4.0](https://img.shields.io/badge/License-CC_BY_4.0-lightgrey.svg)](https://creativecommons.org/licenses/by/4.0/)
[![Language: C++](https://img.shields.io/badge/Language-C++-blue.svg)](https://isocpp.org/)

This repository contains the official C++ implementation for **Shakti's Algorithm**, a novel Depth-First Search architecture optimized for circuit enumeration in directed graphs with capacity constraints. 

## 📄 Academic Publication
The full theoretical paper, complexity proofs, and mathematical models are published on Zenodo as an open-access preprint.

**Read the Paper:** [https://doi.org/10.5281/zenodo.20422392](https://doi.org/10.5281/zenodo.20422392)

**Citation:**
> Singh, S. (2026). Shakti's Algorithm: Efficient Enumeration of Bounded Circuits Using Set-Trie Filtering. *Zenodo*. https://doi.org/10.5281/zenodo.20422392

## ⚡ Performance & Optimization
Extracting non-simple circuits (where nodes may repeat but edges are bounded) traditionally introduces severe combinatorial explosion. This implementation addresses the O(C^2 * M) bottleneck of filtering subsumed topological sub-cycles by introducing a highly cache-friendly **Set-Trie optimization**, drastically reducing time complexity on dense graphs.

## 📊 Empirical Benchmarks & Hardware Performance

Shakti's Algorithm was benchmarked against the highly skewed **Stanford Network Analysis Project (SNAP) Web Graph** dataset to evaluate its resilience against combinatorial explosion and memory fragmentation. 

The C++ engine utilizes a zero-overhead Compressed Sparse Row (CSR) parser to maximize L1/L2 cache hits during array traversal, completely bypassing standard I/O bottlenecks.

**Dataset:** `web-Google.txt` (SNAP)
* **Nodes:** 916,428
* **Edges:** 5,105,039
* **Hardware Profile:** 24GB DDR4 Memory

**Performance Metrics (Capacity Limit = 5):**
* **Graph Parse & Build Time:** 1.20 seconds
* **Total Cycles Evaluated:** 311,730,081
* **Unique Topological Sets Filtered:** 66,181,920
* **Total Execution Time:** 435.39 seconds
* **Throughput:** ~715,000 cycles evaluated per second

*These empirical results demonstrate the algorithm's ability to safely allocate and filter over 66 million unique Set-Trie nodes in main memory without triggering OOM bounds, successfully bypassing the $O(C^2 \cdot M)$ subsumption bottleneck.*

## 📂 Repository Structure
* `shakti_nodes_only.cpp` - Base implementation focusing on node-level cycle detection.
* `shakti_edges_trie.cpp` - Optimized implementation featuring the Set-Trie data structure for high-speed sub-cycle filtering.
* `benchmark_v2.cpp` - Benchmarking suite to test enumeration speed and memory efficiency against standard implementations.

## 🚀 How to Run
To compile and run the optimized algorithm, use the `g++` compiler with the `-O3` optimization flag:

```bash
g++ -O3 shakti_edges_trie.cpp -o shakti_algo
./shakti_algo
