#include <stdlib.h>
#include <string.h>
#include "sha3.h"

#include "ckb_syscalls.h"

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

int run(int i);

int main() {
  uint32_t value;
  uint64_t len = 4;
  int ret = ckb_load_witness((void*) &value, &len, 0, 0,
                             CKB_SOURCE_GROUP_INPUT);
  if (ret != CKB_SUCCESS) {
    return ret;
  }
  if (len < 4) {
    return -1;
  }

  uint32_t times = value >> 8;
  value = value & 0xFF;
  uint8_t result = 0;

  for (int i = 0; i < times; i++) {
    result += run(value);
  }

  return result;
}

int run(int i)
{
  char buf[256];
  int len;

  const char* pubkey_hex = "033f8cf9c4d51a33206a6c1c6b27d2cc5129daa19dbd1fc148d395284f6b26411f";
  const char* signature_hex = "304402203679d909f43f073c7c1dcf8468a485090589079ee834e6eed92fea9b09b06a2402201e46f1075afa18f306715e7db87493e7b7e779569aa13c64ab3d09980b3560a3";
  char* message;
  if (i == 1) {
    message = "foobar";
  } else {
    message = "foobarinvalid";
  }

  secp256k1_context context;
  int ret = secp256k1_custom_verify_only_initialize(
      &context,
      (secp256k1_ge_storage (*)[]) &secp256k1_ecmult_static_pre_context,
      (secp256k1_ge_storage (*)[]) &secp256k1_ecmult_static_pre128_context);
  if (ret == 0) {
    return 4;
  }

  len = hex_to_bin(buf, 65, pubkey_hex);
  CHECK_LEN(len);
  secp256k1_pubkey pubkey;

  ret = secp256k1_ec_pubkey_parse(&context, &pubkey, buf, len);
  if (ret == 0) {
    return 1;
  }

  len = hex_to_bin(buf, 256, signature_hex);
  CHECK_LEN(len);
  secp256k1_ecdsa_signature signature;
  secp256k1_ecdsa_signature_parse_der(&context, &signature, buf, len);
  if (ret == 0) {
    return 3;
  }

  sha3_ctx_t sha3_ctx;
  unsigned char hash[SHA3_BLOCK_SIZE];
  sha3_init(&sha3_ctx, SHA3_BLOCK_SIZE);
  sha3_update(&sha3_ctx, message, strlen(message));
  sha3_final(hash, &sha3_ctx);

  sha3_init(&sha3_ctx, SHA3_BLOCK_SIZE);
  sha3_update(&sha3_ctx, hash, SHA3_BLOCK_SIZE);
  sha3_final(hash, &sha3_ctx);

  ret = secp256k1_ecdsa_verify(&context, &signature, hash, &pubkey);
  if (ret == 1) {
    ret = 0;
  } else {
    ret = 1;
  }

  return ret;
}
