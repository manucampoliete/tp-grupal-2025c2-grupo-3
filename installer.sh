#!/usr/bin/env bash
set -euo pipefail

# installer.sh
# - Optional installation of system dependencies (apt)
# - Build using CMake (or skip build with --no-build)

# - Optional test execution if the target exists
# - Copy binaries, assets, and config to system locations
#
# Usage examples:
#   sudo ./installer.sh --install-deps
#   ./installer.sh --no-build --name mypkg --install-dir /opt/mypkg/bin

######################### Default configuration #########################
NAME="taller_tp"
BUILD_TYPE="Release"
INSTALL_BIN_DIR="/usr/bin"
INSTALL_VAR_DIR="/var/${NAME}"
INSTALL_ETC_DIR="/etc/${NAME}"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT_DIR}/build"

# Base list of suggested APT packages (adjust according to distro/version)
APT_PACKAGES=(
    # Basic build tools
    make git gcc g++ build-essential cmake pkg-config

    # SDL dependencies
    libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev fluidsynth libwavpack1 libwavpack-dev libfreetype-dev wavpack
    libsdl2-mixer-dev

    # Box2D dependency
    libbox2d-dev 
    
    # YAML-CPP dependency
    libyaml-cpp-dev

    # Qt6 dependencies
    qt6-base-dev qt6-base-dev-tools qt6-multimedia-dev
)

######################### Help and argument parsing ###########################
usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Options:
  --name NAME           Name of the package (default: ${NAME})
  --install-deps        Install dependencies via apt (requires sudo)
  --no-build            Skip build
  --build-type TYPE     CMake build type (Debug|Release)
  --install-dir DIR     Directory to install binaries (default: ${INSTALL_BIN_DIR})
  -h, --help            Show this help

Examples:
  sudo $0 --install-deps
  $0 --build-type Debug --install-dir /opt/taller/bin
EOF
}

INSTALL_DEPS=false
DO_BUILD=true

while [[ $# -gt 0 ]]; do
    case "$1" in
        --name) NAME="$2"; shift 2;;
        --install-deps) INSTALL_DEPS=true; shift;;
        --no-build) DO_BUILD=false; shift;;
        --build-type) BUILD_TYPE="$2"; shift 2;;
        --install-dir) INSTALL_BIN_DIR="$2"; shift 2;;
        -h|--help) usage; exit 0;;
        *) echo "Unknown arg: $1"; usage; exit 1;;
    esac
done

INSTALL_VAR_DIR="/var/${NAME}"
INSTALL_ETC_DIR="/etc/${NAME}"

echo "installer: NAME=${NAME}, BUILD_TYPE=${BUILD_TYPE}, INSTALL_DEPS=${INSTALL_DEPS}, DO_BUILD=${DO_BUILD}, INSTALL_BIN_DIR=${INSTALL_BIN_DIR}"

######################### System dependencies #########################
if [ "${INSTALL_DEPS}" = true ]; then
    echo "[installer] Installing system dependencies (apt) ..."
    sudo apt-get update
    sudo apt-get install -y "${APT_PACKAGES[@]}"
fi

######################### Build (CMake) #############################
if [ "${DO_BUILD}" = true ]; then
    echo "[installer] Preparing build in: ${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    cmake --build "${BUILD_DIR}" -- -j$(nproc)
else
    echo "[installer] Skipping build (--no-build)"
fi

######################### Tests (if any) ###############################
# Prefer project's Makefile if present: it has a `run-tests` target that
# compiles (debug) and runs the tests. Otherwise fall back to CMake targets.
if [ -f "${ROOT_DIR}/Makefile" ]; then
    echo "[installer] Found Makefile - running 'make run-tests' (if available)"
    if make -C "${ROOT_DIR}" -n run-tests >/dev/null 2>&1; then
        make -C "${ROOT_DIR}" run-tests || echo "[installer] make run-tests finished (non-zero)"
    else
        echo "[installer] Makefile does not expose 'run-tests' target - skipping tests"
    fi
else
    if [ -d "${BUILD_DIR}" ]; then
        if cmake --build "${BUILD_DIR}" --target help | grep -q "taller_tests"; then
            echo "[installer] Running tests (taller_tests)"
            cmake --build "${BUILD_DIR}" --target taller_tests -- -j$(nproc) || echo "[installer] Tests finished (exit non-zero)"
        else
            echo "[installer] Target 'taller_tests' not found in build - skipping tests"
        fi
    fi
fi

######################### Installing binaries ##########################
echo "[installer] Installing binaries in ${INSTALL_BIN_DIR}"
sudo mkdir -p "${INSTALL_BIN_DIR}"
BINARIES=(taller_server taller_client taller_editor)
for b in "${BINARIES[@]}"; do
    SRC="${BUILD_DIR}/${b}"
    if [ -f "${SRC}" ]; then
        echo "[installer] Copying ${b} -> ${INSTALL_BIN_DIR}/"
        sudo cp "${SRC}" "${INSTALL_BIN_DIR}/"
        sudo chmod +x "${INSTALL_BIN_DIR}/${b}"
    else
        echo "[installer] Binary not found: ${SRC} (skipping)"
    fi
done

######################### Assets and configs ################################
echo "[installer] Installing assets in ${INSTALL_VAR_DIR}"
if [ -d "${ROOT_DIR}/client/assets" ]; then
    sudo rm -rf "${INSTALL_VAR_DIR}/assets" || true
    sudo mkdir -p "${INSTALL_VAR_DIR}"
    sudo cp -r "${ROOT_DIR}/client/assets" "${INSTALL_VAR_DIR}/assets"
    echo "[installer] Copied client/assets -> ${INSTALL_VAR_DIR}/assets"
else
    echo "[installer] client/assets not found - skipping"
fi

echo "[installer] Installing configs in ${INSTALL_ETC_DIR}"
sudo mkdir -p "${INSTALL_ETC_DIR}"
if [ -f "${ROOT_DIR}/config.yaml" ]; then
    sudo cp "${ROOT_DIR}/config.yaml" "${INSTALL_ETC_DIR}/config.yaml"
    echo "[installer] Copied config.yaml -> ${INSTALL_ETC_DIR}/config.yaml"
else
    echo "[installer] config.yaml not found in root - skipping"
fi

# maps folder
if [ -d "${ROOT_DIR}/server/gameLogic/collisions/maps" ]; then
    sudo mkdir -p "${INSTALL_ETC_DIR}/maps"
    sudo cp -r "${ROOT_DIR}/server/gameLogic/collisions/maps" "${INSTALL_ETC_DIR}/maps"
    echo "[installer] Copied maps -> ${INSTALL_ETC_DIR}/maps"
fi

######################### Final ###########################################
echo "[installer] Installation completed."
echo "Binaries: ${INSTALL_BIN_DIR}"
echo "Assets: ${INSTALL_VAR_DIR}" 
echo "Configs: ${INSTALL_ETC_DIR}"

echo "Notes:"
echo " - Adjust the list of APT packages in the script if your distro uses different names."
echo " - The project uses FetchContent for SDL2/libSDL2pp; CMake will download/build those deps as needed."

exit 0
