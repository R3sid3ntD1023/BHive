# Enable strict warnings for all targets
function(enable_strict_warnings target)
    if (MSVC)
        target_compile_options(${target} PRIVATE
            /W4            # High warning level
            /WX            # Treat warnings as errors
            /permissive-   # Stricter C++ conformance
            /w14242        # Conversion, possible loss of data
            /w14263        # Signed/unsigned mismatch
            /w14265        # Class-to-class conversion
            /w14287        # Unsigned/negative mismatch
            /w14296        # Narrowing conversion
            /w14311        # Pointer truncation
            /w14545        # Expression truncation
            /w14546
            /w14547
            /w14549
            /w14555
            /w14619
            /w14640
            /w14826
            /w14905
            /w14906
            /w14928
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wconversion          # Warn on implicit conversions
            -Wsign-conversion     # Warn on signed/unsigned mismatch
            -Werror               # Treat warnings as errors
        )
    endif()
endfunction()