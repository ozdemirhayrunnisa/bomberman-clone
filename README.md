# Bomberman Clone

C++ ve SFML 3 ile geliştirilen Bomberman oyunu.

## Kontroller

| Eylem     | Tuş    |
|-----------|--------|
| Hareket   | W A S D |
| Bomba     | Space  |
| Çıkış     | Escape |

## Derleme

**Gereksinimler:** CMake 3.16+, SFML 3, C++17 derleyici

```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
./build/BombermanClone.exe
```

MSYS2/MinGW kullanıyorsanız önce:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-sfml
```
