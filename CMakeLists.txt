add_executable(test-nolibc src/main.c)

target_link_libraries(test-nolibc
    libsel4
    libsel4assert
    libsel4benchmark
    libsel4printf
    libsel4putchar
    libsel4string
    libsel4startstop
)

add_dependencies(test-nolibc sel4_kernel)
