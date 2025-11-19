#!/bin/zsh

# === CONFIGURATION ===
BUILD_DIR="build"
EXECUTABLE="./Engine"

# === COLOR HELPERS ===
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
RED="\033[1;31m"
RESET="\033[0m"

# === SAFE MODE ===
set -e  # stop on error

# === FUNCTIONS ===

build_project() {
  echo "${YELLOW}-> Building project...${RESET}"
  mkdir -p "$BUILD_DIR"
  cd "$BUILD_DIR"
  cmake .. && make
  cd ..
  echo "${GREEN} Build complete.${RESET}"
}

run_project() {
  cd "$BUILD_DIR"
  if [[ -x "$EXECUTABLE" ]]; then
    echo "${YELLOW}-> Running project...${RESET}"
    "$EXECUTABLE"
  else
    echo "${RED}Error:${RESET} Executable not found: $EXECUTABLE"
    exit 1
  fi
}

clean_project() {
  echo "${YELLOW}→ Cleaning build directory...${RESET}"
  rm -rf "$BUILD_DIR"
  echo "${GREEN}✓ Clean complete.${RESET}"
}

# === INPUT HANDLING ===
choice=$1

if [[ -z $choice ]]; then
  read "choice?Run(r), Build+Run (br), Build only (b), or Clean+Build (c)? [r/br/b/c]: "
fi

case "$choice" in
  r)
    run_project
    ;;
  br)
    build_project
    run_project
    ;;
  b)
    build_project
    ;;
  c)
    clean_project
    build_project
    ;;
  *)
    echo "${RED}Invalid option.${RESET} Please choose: r (run), b (build), or c (clean)."
    exit 1
    ;;
esac
