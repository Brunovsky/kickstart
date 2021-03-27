# Warnings

[Clang warnings](https://clang.llvm.org/docs/DiagnosticsReference.html)

[GCC Warnings](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)

- Unused code
  - Disallow all unused stuff except functions and methods.
- Type casting, promotions
  - Use constructor style `int(X)`, or `static_cast<>` and friends.
  - Warn on all suspicious casts.
- Virtual methods
  - Enable all warnings, treat them as errors if possible
- Variable shadowing
  - Warn on local only
