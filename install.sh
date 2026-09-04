#!/bin/sh
# Install the Competitive Programming Problem Generation skill pack from GitHub.
#
# Usage:
#   install.sh [target]      target: claude | cursor | project   (default: claude)
#   install.sh <dest-dir>    install into an explicit directory
#
# Targets:
#   claude   ~/.claude/skills/cp-problem-generation                        (Claude Code, same machine)
#   cursor   ~/.cursor/skills/competitive-programming-problem-generation   (Cursor, personal)
#   project  ./.agents/skills/competitive-programming-problem-generation   (Cursor, one project)
#
# An existing clone is updated with `git pull` instead of being clobbered.
set -eu

REPO="Anhdung1478/Competitive-Programming-Problem-Generation"
BRANCH="main"

target="${1:-claude}"
case "$target" in
  claude)  dest="${HOME}/.claude/skills/cp-problem-generation" ;;
  cursor)  dest="${HOME}/.cursor/skills/competitive-programming-problem-generation" ;;
  project) dest="$(pwd)/.agents/skills/competitive-programming-problem-generation" ;;
  *)       dest="$target" ;;
esac

if [ -d "$dest/.git" ]; then
  echo "==> $dest already exists; pulling latest"
  git -C "$dest" pull --ff-only
  exit 0
fi

mkdir -p "$(dirname "$dest")"
if command -v git >/dev/null 2>&1; then
  git clone --branch "$BRANCH" "https://github.com/$REPO.git" "$dest"
else
  echo "git not found; falling back to a tarball download" >&2
  if ! command -v curl >/dev/null 2>&1; then
    echo "need either git or curl" >&2
    exit 1
  fi
  tmp="$(mktemp -d)"
  curl -fsSL "https://codeload.github.com/$REPO/tar.gz/refs/heads/$BRANCH" | tar -xz -C "$tmp"
  mv "$tmp/$(basename "$REPO")-$BRANCH" "$dest"
  rm -rf "$tmp"
fi

echo
echo "Installed to: $dest"
case "$target" in
  claude)  echo "Next: open Claude Code and run /reload-plugins (or start a new session)." ;;
  cursor)  echo "Next: restart Cursor so the new skills are picked up." ;;
  project) echo "Next: reload or restart Cursor for this project." ;;
esac
