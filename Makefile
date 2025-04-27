SOURCE_DIR_C = c/secp256k1_ecdsa c/secp256k1_schnorr
SOURCE_DIR_RUST = rust/ed25519 rust/k256_ecdsa rust/k256_schnorr rust/p256 rust/rsa
BINARY_DIR_C = binary/c
BINARY_DIR_RUST = binary/rust

build:
	$(foreach dir, $(SOURCE_DIR_C), $(MAKE) -C $(dir);)
	$(foreach dir, $(SOURCE_DIR_RUST), $(MAKE) -C $(dir);)

	rm -rf binary
	mkdir -p binary/c binary/rust

	$(foreach dir, $(SOURCE_DIR_C), cp $(dir)/build/main $(BINARY_DIR_C)/$(notdir $(dir));)
	$(foreach dir, $(SOURCE_DIR_RUST), cp $(dir)/target/riscv64imac-unknown-none-elf/release/$(notdir $(dir)) $(BINARY_DIR_RUST)/$(notdir $(dir));)

clean:
	$(foreach dir, $(SOURCE_DIR_C), $(MAKE) -C $(dir) clean;)
	$(foreach dir, $(SOURCE_DIR_RUST), $(MAKE) -C $(dir) clean;)
	rm -rf binary binary.tar.gz report.txt

report:
	@$(foreach dir, $(SOURCE_DIR_C), du -h $(BINARY_DIR_C)/$(notdir $(dir)) && ckb-debugger --bin $(BINARY_DIR_C)/$(notdir $(dir));)
	@$(foreach dir, $(SOURCE_DIR_RUST), du -h $(BINARY_DIR_RUST)/$(notdir $(dir)) && ckb-debugger --bin $(BINARY_DIR_RUST)/$(notdir $(dir));)
