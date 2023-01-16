# CXL.mem Simulator


## User input
Bandwidth, Latency: Both use 2 input in the vector, first for read, second for write
Weight: Use the heuristic to calculate the bandwidth
Topology: Construct the topology using newick tree syntax (1,(2,3)) stands for 
```bash
            1
          /
0 - local
          \
                   2
         switch  / 
                 \ 
                  3
```
