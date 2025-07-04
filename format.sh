find $1 -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} +

echo "All .cpp and .h files formatted with clang-format style: $STYLE"
