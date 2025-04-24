# CKB-VM Bench Scripts

Non-trivial CKB VM Scripts which can be used as benchmarks.

## Build

Compile the bench program. Note that we use llvm-18 to compile C programs.

```sh
$ git submodule update --init
$ make build
```

Run all bench script by [ckb-debugger](https://github.com/nervosnetwork/ckb-standalone-debugger/tree/develop/ckb-debugger) and report the running results.

```sh
$ make report
```
