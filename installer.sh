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
NAME="needForSpeed2D"
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
  --no-install-deps     Do not install dependencies via apt (requires sudo)
  --no-build            Skip build
  --build-type TYPE     CMake build type (Debug|Release)
  --install-dir DIR     Directory to install binaries (default: ${INSTALL_BIN_DIR})
  -h, --help            Show this help

Examples:
  sudo $0 --install-deps
  $0 --build-type Debug --install-dir /opt/taller/bin
EOF
}

INSTALL_DEPS=true
DO_BUILD=true

while [[ $# -gt 0 ]]; do
    case "$1" in
        --name) NAME="$2"; shift 2;;
        --no-install-deps) INSTALL_DEPS=false; shift;;
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
    cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DINSTALL_MODE=ON
    cmake --build "${BUILD_DIR}" -- -j$(nproc)
else
    echo "[installer] Skipping build (--no-build)"
fi

######################### Tests (if any) ###############################
if [ -d "${BUILD_DIR}" ]; then
    if cmake --build "${BUILD_DIR}" --target help | grep -q "needForSpeed2DTests"; then
        echo "[installer] Running tests (needForSpeed2DTests)"
        cmake --build "${BUILD_DIR}" --target needForSpeed2DTests -- -j$(nproc) || echo "[installer] Tests finished (exit non-zero)"
    else
        echo "[installer] Target 'needForSpeed2DTests' not found in build - skipping tests"
    fi
fi

######################### Installing binaries ##########################
echo "[installer] Installing binaries in ${INSTALL_BIN_DIR}"
sudo mkdir -p "${INSTALL_BIN_DIR}"
BINARIES=(needForSpeed2DServer needForSpeed2DClient needForSpeed2DEditor)
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
# Clean previous assets
sudo rm -rf "${INSTALL_VAR_DIR}" || true

# Create asset directory
sudo mkdir -p "${INSTALL_VAR_DIR}"

# Client assets
echo "[installer] Installing assets in ${INSTALL_VAR_DIR}"
if [ -d "${ROOT_DIR}/client/assets" ]; then
    sudo mkdir -p "${INSTALL_VAR_DIR}/client"
    sudo cp -r "${ROOT_DIR}/client/assets" "${INSTALL_VAR_DIR}/client"
    echo "[installer] Copied client/assets -> ${INSTALL_VAR_DIR}/client"
else
    echo "[installer] client/assets not found - skipping"
fi

# Lobby assets

# Editor assets

# Clean previous configs
sudo rm -rf "${INSTALL_ETC_DIR}" || true

# Create config directory
sudo mkdir -p "${INSTALL_ETC_DIR}"

# Main config file (config.yaml)
echo "[installer] Installing configs in ${INSTALL_ETC_DIR}"
sudo mkdir -p "${INSTALL_ETC_DIR}"
if [ -f "${ROOT_DIR}/config.yaml" ]; then
    sudo cp "${ROOT_DIR}/config.yaml" "${INSTALL_ETC_DIR}"
    echo "[installer] Copied config.yaml -> ${INSTALL_ETC_DIR}"
else
    echo "[installer] config.yaml not found in root - skipping"
fi

# Maps folder (server/gameLogic/collisions/maps)
if [ -d "${ROOT_DIR}/server/gameLogic/collisions/maps" ]; then
    sudo mkdir -p "${INSTALL_ETC_DIR}/server/gameLogic/collisions"
    sudo cp -r "${ROOT_DIR}/server/gameLogic/collisions/maps" "${INSTALL_ETC_DIR}/server/gameLogic/collisions"
    echo "[installer] Copied maps -> ${INSTALL_ETC_DIR}/server/gameLogic/collisions"
fi

# Races folder (server/gameLogic/races)
if [ -d "${ROOT_DIR}/server/gameLogic/races" ]; then
    sudo mkdir -p "${INSTALL_ETC_DIR}/server/gameLogic"
    sudo cp -r "${ROOT_DIR}/server/gameLogic/races" "${INSTALL_ETC_DIR}/server/gameLogic"
    echo "[installer] Copied races -> ${INSTALL_ETC_DIR}/server/gameLogic"
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
