set(target "test-nolibc")

add_executable(${target} src/main.c)

target_link_libraries(${target}
    libsel4
    libsel4assert
    libsel4benchmark
    libsel4printf
    libsel4putchar
    libsel4string
    libsel4startstop
)

AddRunTarget(${target})

add_dependencies(${target} sel4_kernel)
