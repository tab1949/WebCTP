#!/usr/bin/env bash
set -euo pipefail

INSTALL_ROOT="/usr/local/lib/webctp/recorder"
SYSTEMD_UNIT_DIR="/etc/systemd/system"
DISTRIBUTED_UNITS=(
  record.timer
  record-start.service
  record-stop.service
  record-archive.service
)

die() {
  echo "Error: $*" >&2
  exit 1
}

require_root() {
  [[ $(id -u) -eq 0 ]] || die "uninstall must run as root (sudo)"
}

require_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "$1 is required but missing"
}

require_root
require_cmd systemctl

echo "Disabling and stopping recorder services..."
for unit in "${DISTRIBUTED_UNITS[@]}"; do
  systemctl disable --now "$unit" >/dev/null 2>&1 || true
  if [[ -f "$SYSTEMD_UNIT_DIR/$unit" ]]; then
    rm -f "$SYSTEMD_UNIT_DIR/$unit"
    echo "removed unit $unit"
  elif systemctl list-unit-files "$unit" >/dev/null 2>&1; then
    echo "unit $unit not installed in $SYSTEMD_UNIT_DIR"
  fi
done

echo "Removing recorder files from $INSTALL_ROOT..."
if [[ -d "$INSTALL_ROOT" ]]; then
  rm -rf "$INSTALL_ROOT"
  echo "removed $INSTALL_ROOT"
else
  echo "no recorder files found at $INSTALL_ROOT"
fi

systemctl daemon-reload >/dev/null
echo "Systemd daemon reloaded. Recorder uninstall complete."