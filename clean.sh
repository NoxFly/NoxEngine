#!/bin/bash
# Clean build artifacts

echo "Cleaning build artifacts..."

# Remove build directories
rm -rf out/build
rm -rf out/install

# Remove binary outputs
rm -rf bin/*.dll
rm -rf bin/*.lib
rm -rf bin/*.exp
rm -rf bin/examples

echo "Clean complete!"
