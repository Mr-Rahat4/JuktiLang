CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
SRC = $(wildcard src/*.cpp)
TARGET = juktilangc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Run every .jukti file in tests/valid and tests/errors through the
# compiler, then execute every generated .py to prove it actually runs.
test: $(TARGET)
	@echo "--- valid programs ---"
	@for f in tests/valid/*.jukti; do \
		echo "== $$f =="; ./$(TARGET) $$f && python3 $${f%.jukti}.py; \
	done
	@echo "--- programs expected to error ---"
	@for f in tests/errors/*.jukti; do \
		echo "== $$f =="; ./$(TARGET) $$f || true; \
	done

clean:
	rm -f $(TARGET) src/*.o tests/valid/*.py tests/errors/*.py examples/*.py

.PHONY: all test clean
