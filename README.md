## Cearch
inspired by https://github.com/tsoding/seroost

Local Search engine with Web Interface

## Dependencies
- Pugixml (libpugixml-dev)
- Boost Asio (libboost-all-dev)
- nlohmann json (nlohmann-json3-dev)

## Build the project
make 

## Run Cearch
./cearch 8080 docs.gl index

## time without threading
./docs.gl -> 169.1s
./docs.gl/gl4/ -> 7.0s

## Time to build the index with threads
- ./docs.gl 
    - 1 thread-> 112.7s
    - 4 thread-> 43.6s
    - 8 thread-> 35.2s

- ./docs.gl/gl4 
    - 1 thread -> 7.0s
    - 4 thread -> 3.8s
    - 8 thread -> 3.4s