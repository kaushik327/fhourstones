# Profiling
```
make
sudo perf record -g --call-graph=dwarf ./SearchGame < inputs
sudo perf report --stdio --no-children -g graph,0.5,caller
```
