# Cearch
inspired by https://github.com/tsoding/seroost

Local Search engine with Web Interface

# Build the project
make 

# Run Cearch
./cearch 8080 docs.gl index

# Time to build the index
./docs.gl 
    1 thread-> 225.6s
    4 thread-> 241.8s
    8 thread-> 325.4s

./docs.gl/gl4 
    1 thread -> 7.6s
    4 thread -> 8.1s
    8 thread -> 9.9s

# Time with 4 Threads

