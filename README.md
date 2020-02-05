# Thread-safe data structure

Possible data structures are modified to be thread safe and lock free. Test cases included for unit functionality. Performance will be compared among lock-guarded version and lock-free version. 

## Requirement

- GoogleTest
- Google benchmark
- c++11
- pthread
- cmake >= 3.5

## Data Structures

### FIFO

A fixed sized FIFO is implemented by circular buffer. There are two possible way of implementing circular buffer, while the implementation here is to waste one slot to achieve minimal data denpencency and possible race to take care of. Lock-free version is taken care by atomic and memory-order.

## Reference 

1. Michael, Maged M., and Michael L. Scott. "Simple, fast, and practical non-blocking and blocking concurrent queue algorithms." Proceedings of the fifteenth annual ACM symposium on Principles of distributed computing. 1996.


Still updating, see dev branch for code detail