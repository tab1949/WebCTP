#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd ../.. && pwd)"

INSTALL_ROOT="/usr/local/lib/webctp/recorder"
SYSTEMD_UNIT_DIR="/etc/systemd/system"

die() {
    echo "Error: $*" >&2
    exit 1
}

require_cmd() {
    command -v "$1" >/dev/null 2>&1 || die "$1 is required but not found in PATH"
}

[[ $(id -u) -eq 0 ]] || die "this installer must run as root (sudo) to write to $RECORDER_DIR and $SYSTEMD_UNIT_DIR"

require_cmd npm
require_cmd systemctl

echo "Copying source files..."
pushd "$PROJECT_ROOT" >/dev/null
mkdir -p "$INSTALL_ROOT/examples/recorder"
mkdir -p "$INSTALL_ROOT/src"
cp -r examples/recorder/record.ts $INSTALL_ROOT/examples/recorder/record.ts
cp examples/recorder/test.config.local.json $INSTALL_ROOT/examples/recorder/test.config.local.json
cp -r src $INSTALL_ROOT/
cp package.json $INSTALL_ROOT/package.json
cp tsconfig.json $INSTALL_ROOT/tsconfig.json
chmod -R 755 $INSTALL_ROOT
pushd $INSTALL_ROOT >/dev/null
echo "Installing npm dependencies..."
npm install
popd >/dev/null
popd >/dev/null

echo "Installing systemd units into $SYSTEMD_UNIT_DIR..."
mkdir -p "$SYSTEMD_UNIT_DIR"
install -m 0644 "./record-start.service" "$SYSTEMD_UNIT_DIR/record-start.service"
install -m 0644 "./record-stop.service" "$SYSTEMD_UNIT_DIR/record-stop.service"
install -m 0644 "./record-archive.service" "$SYSTEMD_UNIT_DIR/record-archive.service"
install -m 0644 "./record.timer" "$SYSTEMD_UNIT_DIR/record.timer"

echo "Reloading systemd daemon..."
systemctl daemon-reload

echo "Recorder installed to $INSTALL_ROOT. Enable and start the timer when ready with:"
echo "  systemctl enable --now record.timer"