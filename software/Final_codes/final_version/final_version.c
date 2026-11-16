#include <stdint.h>
#include <string.h>

static inline uint32_t load32(const uint8_t *p)
{
    return ((uint32_t)p[0]) | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static inline void store32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v);
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

void expand_key(uint8_t *key, uint8_t *round_keys) {
    //uint32_t time0 = rdcycle();   
    #pragma clang loop unroll(full)
    for (int i = 0; i < 16; i++)
        round_keys[i] = key[i];
    #define KSU_ROUND_1(round_num){                                         \
        uint32_t _w3   = load32(&round_keys[(round_num)*16 - 4]);           \
        uint32_t _temp, _w0, _w1, _w2, _w4;                                 \
        _temp = __builtin_riscv_ksu_f(_w3, round_num);                      \
        _w0 = load32(&round_keys[(round_num)*16 - 16]) ^ _temp;             \
        _w1 = load32(&round_keys[(round_num)*16 - 12]) ^ _w0;               \
        _w2 = load32(&round_keys[(round_num)*16 -  8]) ^ _w1;               \
        _w4 = load32(&round_keys[(round_num)*16 -  4]) ^ _w2;               \
        store32(&round_keys[(round_num)*16 +  0], _w0);                     \
        store32(&round_keys[(round_num)*16 +  4], _w1);                     \
        store32(&round_keys[(round_num)*16 +  8], _w2);                     \
        store32(&round_keys[(round_num)*16 + 12], _w4);                     \
    }

    KSU_ROUND_1(1);
    KSU_ROUND_1(2);
    KSU_ROUND_1(3);
    KSU_ROUND_1(4);
    KSU_ROUND_1(5);
    KSU_ROUND_1(6);
    KSU_ROUND_1(7);
    KSU_ROUND_1(8);
    KSU_ROUND_1(9);
    KSU_ROUND_1(10);

    #undef KSU_ROUND_1  
}

void aes128_encrypt_block(uint8_t *plaintext, uint8_t *round_keys, uint8_t *ciphertext)
{
    uint32_t s0, s1, s2, s3;
    uint32_t t0, t1, t2, t3;

    s0 = load32(&plaintext[0]);
    s1 = load32(&plaintext[4]);
    s2 = load32(&plaintext[8]);
    s3 = load32(&plaintext[12]);

    s0 ^= load32(&round_keys[0]);
    s1 ^= load32(&round_keys[4]);
    s2 ^= load32(&round_keys[8]);
    s3 ^= load32(&round_keys[12]);
    #pragma clang loop unroll(full)
    for (int round = 1; round < 10; round++)
    {
        const uint8_t *rk = &round_keys[round * 16];

        t0 = load32(&rk[0]);
        t0 = __builtin_riscv_aes32esmi(t0, s0, 0);
        t0 = __builtin_riscv_aes32esmi(t0, s1, 1);
        t0 = __builtin_riscv_aes32esmi(t0, s2, 2);
        t0 = __builtin_riscv_aes32esmi(t0, s3, 3);

        t1 = load32(&rk[4]);
        t1 = __builtin_riscv_aes32esmi(t1, s1, 0);
        t1 = __builtin_riscv_aes32esmi(t1, s2, 1);
        t1 = __builtin_riscv_aes32esmi(t1, s3, 2);
        t1 = __builtin_riscv_aes32esmi(t1, s0, 3);

        t2 = load32(&rk[8]);
        t2 = __builtin_riscv_aes32esmi(t2, s2, 0);
        t2 = __builtin_riscv_aes32esmi(t2, s3, 1);
        t2 = __builtin_riscv_aes32esmi(t2, s0, 2);
        t2 = __builtin_riscv_aes32esmi(t2, s1, 3);

        t3 = load32(&rk[12]);
        t3 = __builtin_riscv_aes32esmi(t3, s3, 0);
        t3 = __builtin_riscv_aes32esmi(t3, s0, 1);
        t3 = __builtin_riscv_aes32esmi(t3, s1, 2);
        t3 = __builtin_riscv_aes32esmi(t3, s2, 3);

        s0 = t0;
        s1 = t1;
        s2 = t2;
        s3 = t3;
    }

    const uint8_t *rk = &round_keys[10 * 16];
    t0 = load32(&rk[0]);
    t0 = __builtin_riscv_aes32esi(t0, s0, 0);
    t0 = __builtin_riscv_aes32esi(t0, s1, 1);
    t0 = __builtin_riscv_aes32esi(t0, s2, 2);
    t0 = __builtin_riscv_aes32esi(t0, s3, 3);

    t1 = load32(&rk[4]);
    t1 = __builtin_riscv_aes32esi(t1, s1, 0);
    t1 = __builtin_riscv_aes32esi(t1, s2, 1);
    t1 = __builtin_riscv_aes32esi(t1, s3, 2);
    t1 = __builtin_riscv_aes32esi(t1, s0, 3);

    t2 = load32(&rk[8]);
    t2 = __builtin_riscv_aes32esi(t2, s2, 0);
    t2 = __builtin_riscv_aes32esi(t2, s3, 1);
    t2 = __builtin_riscv_aes32esi(t2, s0, 2);
    t2 = __builtin_riscv_aes32esi(t2, s1, 3);

    t3 = load32(&rk[12]);
    t3 = __builtin_riscv_aes32esi(t3, s3, 0);
    t3 = __builtin_riscv_aes32esi(t3, s0, 1);
    t3 = __builtin_riscv_aes32esi(t3, s1, 2);
    t3 = __builtin_riscv_aes32esi(t3, s2, 3);

    store32(&ciphertext[0], t0);
    store32(&ciphertext[4], t1);
    store32(&ciphertext[8], t2);
    store32(&ciphertext[12], t3);
}

void write_to_address(uintptr_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

void write_v_to_address(uintptr_t addr, uint8_t vector[16])
{
    uint32_t *vector_word = (uint32_t *)vector;
    for (int i = 0; i < 4; i++)
    {
        write_to_address(addr + i * 0x4, vector_word[i]);
    }
}

void aes128_ecb_encrypt(uint8_t *plaintext, size_t len, uint8_t *key, uint8_t *ciphertext)
{
    if (len % 16 != 0)
    {
        return;
    }
    
    uint8_t round_keys[176];
    expand_key(key, round_keys);
    
    for (size_t i = 0; i < len; i += 16)
    {
        aes128_encrypt_block(&plaintext[i], round_keys, &ciphertext[i]);
    }
}

int main()
{
    asm volatile ("csrw 0x320, %0" :: "r"(0));
    uint8_t plaintext[16] = {
        'H', 'e', 'l', 'l', 'o', ',', ' ', 'W',
        'o', 'r', 'l', 'd', '!', '0', '0', '0'
    };
    uint8_t key[16] = {
        'c', 'e', 's', 'e', '4', '0', '4', '0',
        'p', 'a', 's', 's', 'w', 'o', 'r', 'd'
    };
    uint8_t expected_output[16] = {0x14, 0x09, 0xA5, 0xFB, 0x1F, 0xF4, 0x4B, 0x71,
                                   0xBE, 0xAA, 0x25, 0x2E, 0x0F, 0x08, 0xF9, 0xAA};
    uint8_t ciphertext[16];
    size_t len = 16;

    uintptr_t addr;
    uint32_t value;

    aes128_ecb_encrypt(plaintext, len, key, ciphertext);

    addr = 0x0100000 + 0x2000 + 0x30;
    write_v_to_address(addr, expected_output);

    addr = 0x0100000 + 0x2000 + 0x40;
    write_v_to_address(addr, ciphertext);

    addr = 0x0100000 + 0x2000 + 0x04;
    value = 0xCAFEBABE;
    for (int i = 0; i < 16; i++) {
        if (ciphertext[i] != expected_output[i]) {
            value = 0xBAAAAAAD;
            break;
        }
    }
    write_to_address(addr, value);

    //END OF TEST WRITE (used to identify when the execution of the C code finished): DO NOT REMOVE!
    addr = 0x0100000 + 0x2000;
    value = 0xDEADBEEF;
    write_to_address(addr, value);
    return 0;
}
