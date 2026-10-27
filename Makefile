# Makefile - Practica 2 ADA (Karatsuba)
# Linux / macOS:  make, make test, make clean
# Windows MinGW:  mingw32-make, mingw32-make test, mingw32-make clean
#
# No se define NDEBUG: los assert (por ejemplo, la precondicion A >= B de
# restar) deben seguir activos.

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

# En Windows, mingw32-make ejecuta las recetas con cmd.exe: cambian la
# extension de los ejecutables, el comando para borrar y la forma de
# invocar un programa del directorio actual.
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM  = del /Q
    RUN = .\\
else
    EXE =
    RM  = rm -f
    RUN = ./
endif

ARITMETICA = src/aritmetica.cpp src/aritmetica.h

TEST_ARITMETICA = test_aritmetica$(EXE)

all: $(TEST_ARITMETICA)

$(TEST_ARITMETICA): tests/test_aritmetica.cpp $(ARITMETICA)
	$(CXX) $(CXXFLAGS) -o $@ tests/test_aritmetica.cpp src/aritmetica.cpp

test: $(TEST_ARITMETICA)
	$(RUN)$(TEST_ARITMETICA)

clean:
	-$(RM) $(TEST_ARITMETICA)

.PHONY: all test clean
