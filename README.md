## Cearch
inspired by https://github.com/tsoding/seroost

Local Search engine with Web Interface

## Dependencies
- Pugixml (libpugixml-dev)
- Boost Asio (libboost-all-dev)
- poppler (lib-poppler)

## Build the project
make build

## Run Cearch
./cearch 8080 docs.gl index

# Container
## build container
docker build -t cearch .

## run container with output and mount with testdata
docker run --rm -p 8088:8088 -v ./testdata:/app/testdata cearch