cmake_minimum_required(VERSION 3.26)
project(Kyber)

add_compile_options(-mavx2 -mbmi2 -mpopcnt -maes -march=native -mtune=native -O3 -fomit-frame-pointer)

enable_language(ASM)
find_package(OpenSSL REQUIRED)

set(SRCS
    src/cbd.c
    src/consts.c
    src/indcpa.c
    src/kem.c
    src/poly.c
    src/polyvec.c
    src/rejsample.c
    src/rng.c
    src/verify.c
    src/aes256ctr.c
    src/fq.S
    src/invntt.S
    src/ntt.S
    src/shuffle.S
    src/basemul.S
)

add_library(${PROJECT_NAME} STATIC ${SRCS})
target_compile_definitions(${PROJECT_NAME} PUBLIC KYBER_90S KYBER_K=3)
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC OpenSSL::Crypto)
