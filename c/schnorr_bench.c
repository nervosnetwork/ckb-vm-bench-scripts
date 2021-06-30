#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha3.h"

#define SHA3_BLOCK_SIZE 32

/*
 * We are including secp256k1 implementation directly so gcc can strip
 * unused functions. For some unknown reasons, if we link in libsecp256k1.a
 * directly, the final binary will include all functions rather than those used.
 */
#define HAVE_CONFIG_H 1
#define USE_EXTERNAL_DEFAULT_CALLBACKS
#include <secp256k1.c>

void secp256k1_default_illegal_callback_fn(const char* str, void* data) {
  (void) str;
  (void) data;
  abort();
}

void secp256k1_default_error_callback_fn(const char* str, void* data) {
  (void) str;
  (void) data;
  abort();
}

int secp256k1_custom_verify_only_initialize(secp256k1_context *context,
                                            secp256k1_ge_storage (*pre_g)[],
                                            secp256k1_ge_storage (*pre_g_128)[]) {
  context->illegal_callback = default_illegal_callback;
  context->error_callback = default_error_callback;

  secp256k1_ecmult_context_init(&context->ecmult_ctx);
  secp256k1_ecmult_gen_context_init(&context->ecmult_gen_ctx);

  context->ecmult_ctx.pre_g = pre_g;
  context->ecmult_ctx.pre_g_128 = pre_g_128;

  return 1;
}

int char_to_int(char ch)
{
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  }
  return -1;
}

int hex_to_bin(char* buf, size_t buf_len, const char* hex)
{
  int i = 0;

  for (; i < buf_len && hex[i * 2] != '\0' && hex[i * 2 + 1] != '\0'; i++) {
    int a = char_to_int(hex[i * 2]);
    int b = char_to_int(hex[i * 2 + 1]);

    if (a < 0 || b < 0) {
      return -1;
    }

    buf[i] = ((a & 0xF) << 4) | (b & 0xF);
  }

  if (i == buf_len && hex[i * 2] != '\0') {
    return -1;
  }
  return i;
}

#define CHECK_LEN(x) if ((x) <= 0) { return x; }

/*
 * Arguments are listed in the following order:
 * 0. Program name, ignored here, only preserved for compatibility reason
 * 1. Pubkey in hex format, 32 bytes for schnorr pub key
 * 2. Signature in hex format, 64 bytes for schnorr signature
 * 3+. messages to hash via sha3
 *
 * This program will run double sha3 on all arguments excluding pubkey and
 * signature(also for simplicity, we are running sha256 on ASCII chars directly,
 * not deserialized raw bytes), then it will use sha256 result calculated as the
 * message to verify the signature. It returns 0 if the signature works, and
 * a non-zero value otherwise.
 *
 * Note all hex values passed in as arguments must have lower case letters for
 * deterministic behavior.
 */
int main(int argc, char* argv[])
{
  char buf[256];
  int len;

  if (argc < 4) {
    return -1;
  }

  secp256k1_context context;
  int ret = secp256k1_custom_verify_only_initialize(
      &context,
      (secp256k1_ge_storage (*)[]) &secp256k1_ecmult_static_pre_context,
      (secp256k1_ge_storage (*)[]) &secp256k1_ecmult_static_pre128_context);
  if (ret == 0) {
    return 4;
  }

  len = hex_to_bin(buf, 32, argv[1]);
  CHECK_LEN(len);
  secp256k1_xonly_pubkey pk;
  ret = secp256k1_xonly_pubkey_parse(&context, &pk, buf);
  if (ret == 0) {
    return 3;
  }

  unsigned char sig[64];
  len = hex_to_bin(sig, 64, argv[2]);
  CHECK_LEN(len);

  sha3_ctx_t sha3_ctx;
  unsigned char hash[SHA3_BLOCK_SIZE];
  sha3_init(&sha3_ctx, SHA3_BLOCK_SIZE);
  for (int i = 3; i < argc; i++) {
    sha3_update(&sha3_ctx, argv[i], strlen(argv[i]));
  }
  sha3_final(hash, &sha3_ctx);

  sha3_init(&sha3_ctx, SHA3_BLOCK_SIZE);
  sha3_update(&sha3_ctx, hash, SHA3_BLOCK_SIZE);
  sha3_final(hash, &sha3_ctx);

  ret = secp256k1_schnorrsig_verify(&context, sig, hash, &pk);
  if (ret == 1) {
    ret = 0;
  } else {
    ret = 2;
  }

  return ret;
}
