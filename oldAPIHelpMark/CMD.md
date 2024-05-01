# CMDs

Ethan's guide to some long commands for copying-pasting. TODO: Replace for an easier interface

Generate a Makefile, compile_commands.json (for clangd) and debug mode (can change to release if running benchmarks). And then compile!

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug ..
make

## Syncing files

Send everything in iht_rdma_minimal (but not in scripts/exclude.txt) to cloudlab

python sync.py -u esl225

OR

sh sync.sh -u esl225

(use -h to get usage for sync)

## Installing dependencies

Sync first and then install dependencies

python sync.py -u esl225 -i
sh sync.sh -u esl225 -i

## Running

### Start running the IHT (benchmark)

python launch.py -u esl225 -e exp --runtype bench --from_param_config exp_conf.json

If running correctness tests, use --runtype test or --runtype concurrent_test

### Stopping the IHT if it stalls/deadlocks

python shutdown.py -u esl225

### Manual Normal

LD_LIBRARY_PATH=./build:./build/protos ./iht_rome --node_id 0 --runtime 3 --op_count 10 --contains 80 --insert 10 --remove 10 --key_lb 0 --key_ub 20000 --region_size 25 --thread_count 1 --node_count 1 --qp_max 1 --unlimited_stream

### Manual for GDB

LD_LIBRARY_PATH=./build:./build/protos gdb ./iht_rome

run --node_id 0 --runtime 1 --op_count 10 --contains 80 --insert 10 --remove 10 --key_lb 0 --key_ub 20000 --region_size 25 --thread_count 1 --node_count 2 --qp_max 1 --cache_depth 3 --unlimited_stream

run --node_id 1 --runtime 1 --op_count 10 --contains 80 --insert 10 --remove 10 --key_lb 0 --key_ub 20000 --region_size 25 --thread_count 1 --node_count 2 --qp_max 1 --cache_depth 3 --unlimited_stream


### Manual for GDB (two sided)

LD_LIBRARY_PATH=./build:./build/protos gdb ./iht_twosided

run --node_id 0 --runtime 3 --op_count 1 --contains 100 --insert 0 --remove 0 --key_lb 0 --key_ub 10 --region_size 25 --thread_count 1 --node_count 3 --qp_max 10 --cache_depth 3 --unlimited_stream

run --node_id 1 --runtime 3 --op_count 1 --contains 100 --insert 0 --remove 0 --key_lb 0 --key_ub 10 --region_size 25 --thread_count 1 --node_count 3 --qp_max 10 --cache_depth 3 --unlimited_stream

run --node_id 2 --runtime 3 --op_count 1 --contains 100 --insert 0 --remove 0 --key_lb 0 --key_ub 10 --region_size 25 --thread_count 1 --node_count 3 --qp_max 10 --cache_depth 3 --unlimited_stream


### Manual Testing

LD_LIBRARY_PATH=.:./protos ./iht_rome_test --send_test

LD_LIBRARY_PATH=.:./protos gdb ./iht_rome_test
run --send_test

LD_LIBRARY_PATH=.:./protos ./iht_rome_test --send_bulk

LD_LIBRARY_PATH=.:./protos gdb ./iht_rome_test
run --send_bulk
