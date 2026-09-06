"""Configuration, read from the environment the MCP client launches us with.

Adapted from the `competitive-programming` plugin by LamTer
(<https://github.com/LLaammTTeerr/competitive-programming>). Two things differ from
that original, both deliberate:

* **`root` falls back to the process's working directory** instead of being
  unset. There, an unset `POLYGON_MCP_ROOT` disabled `path=` entirely, and the
  variable had to be exported in the shell that launched Claude Code — a
  footgun its own skill documents. Claude Code starts this server in the
  directory the session was opened in, which is the problem repository, so the
  common case now needs no configuration and the override still works.
* **Files are read as bytes and decoded**, never through text mode. See
  `read_text_verbatim`.

The credentials live only here. Nothing in this package writes them to disk,
puts them in a log line, or lets them out through a tool's return value —
`PolygonError` messages are composed by hand rather than stringified from an
exception, because the signed query string carries `apiKey` and httpx embeds
the request URL in the text of every error it raises.
"""

from __future__ import annotations

import os
from dataclasses import dataclass, field
from pathlib import Path

BASE_URL = "https://polygon.codeforces.com/api/"

# Polygon rejects a request whose `time` is more than five minutes off its own
# clock, so a slow retry has to be re-signed rather than replayed.
MAX_CLOCK_SKEW_SECONDS = 300


class PolygonError(Exception):
    """Anything a tool should report as `{"ok": false, "error": ...}`.

    Carries the API method so a failure names what was being attempted, which
    matters when a tool call is one step of a long upload flow. `details` holds
    whatever structured `result` a FAILED envelope carried alongside its
    comment — `problem.deleteTest` says which tests it refused that way, and a
    comment reading "Some tests can not be deleted." on its own is not enough
    for the caller to act on.
    """

    def __init__(self, message: str, method: str = "", details: object = None):
        super().__init__(message)
        self.method = method
        self.details = details


def _env(name: str, default: str = "") -> str:
    """Read one environment variable, treating an unexpanded placeholder as unset.

    An MCP client that substitutes `${VAR}` into its server config hands the
    process the *literal* placeholder when the variable is not set in the
    launching environment. Left alone that is far worse than an empty value: a
    root of `"${POLYGON_MCP_ROOT}"` refuses every `path=` read with a message
    about a directory that does not exist, and a key of `"${POLYGON_API_KEY}"`
    reaches Polygon and comes back as `Incorrect signature` rather than as the
    missing credential it is.
    """
    value = os.environ.get(name)
    if not value:
        return default
    value = value.strip()
    if value.startswith("${") and value.endswith("}"):
        return default
    return value


@dataclass
class Config:
    api_key: str = ""
    api_secret: str = ""
    base_url: str = BASE_URL
    timeout: float = 30.0
    # The only directory a `path=` argument may point into. Defaults to the
    # working directory this server was launched in.
    root: Path = field(default_factory=Path.cwd)
    # Floor on the gap between two requests, so a scripted upload of a few
    # hundred tests does not hammer Polygon.
    min_interval: float = 0.5

    @classmethod
    def from_env(cls) -> "Config":
        root = _env("POLYGON_MCP_ROOT")
        return cls(
            api_key=_env("POLYGON_API_KEY"),
            api_secret=_env("POLYGON_API_SECRET"),
            base_url=_env("POLYGON_BASE_URL", default=BASE_URL),
            timeout=float(_env("POLYGON_TIMEOUT", default="30")),
            root=Path(root).expanduser() if root else Path.cwd(),
            min_interval=float(_env("POLYGON_MIN_INTERVAL", default="0.5")),
        )

    @property
    def has_credentials(self) -> bool:
        return bool(self.api_key and self.api_secret)


def read_text_verbatim(path: Path) -> str:
    """Read a file as UTF-8 without touching its line endings.

    `Path.read_text()` opens in text mode, and Python's universal-newline
    translation silently rewrites every CRLF to LF on the way in. That is the
    wrong default for this server: the skill driving it spends a whole phase on
    line endings, because Polygon generates and validates tests under a Windows
    toolchain whose strict testlib `eoln()` wants CRLF. A file uploaded through
    here goes up as the bytes on disk, and any line-ending decision stays the
    one the package already made.
    """
    try:
        raw = path.read_bytes()
    except OSError as error:
        # Passing the root check is not permission to read: the file can be
        # mode 000, or a dangling mount. Without this the tool would raise
        # instead of returning the {"ok": false} shape every other one does.
        raise PolygonError(
            f"Could not read {path}: {error.strerror or error}"
        ) from None
    try:
        return raw.decode("utf-8")
    except UnicodeDecodeError:
        raise PolygonError(
            f"{path} is not UTF-8 text. This server sends uploads as form "
            "fields, so source, statements and test data go as text; binary "
            "resources have to be uploaded through the web interface."
        ) from None


def resolve_local_path(raw: str, root: Path) -> Path:
    """Resolve a caller-supplied path, or refuse it.

    A tool that accepts `path=` is a file-read primitive handed to a model, so
    it is confined to one directory: `POLYGON_MCP_ROOT` when the operator names
    one, otherwise the directory this server was launched in. Both sides are
    fully resolved before the comparison, so a symlink pointing out of the root
    is caught the same way `../..` is.

    **A relative path is resolved against the root, not the process's working
    directory.** The caller is a model in a session whose own notion of "here"
    moves — a `cd` in its shell does not move this server — so anchoring to the
    root is the only reading that stays true. In the default case the two are
    the same directory anyway.
    """
    resolved = Path(raw).expanduser()
    if not resolved.is_absolute():
        resolved = root / resolved
    try:
        base = root.resolve(strict=True)
    except OSError:
        raise PolygonError(
            f"The root this server may read from does not exist: {root}. Set "
            "POLYGON_MCP_ROOT to the problem directory, or pass the content "
            "inline instead."
        ) from None
    try:
        resolved = resolved.resolve(strict=True)
    except OSError:
        raise PolygonError(f"File not found: {raw}") from None
    if resolved != base and base not in resolved.parents:
        raise PolygonError(
            f"Refusing to read {resolved}: it is outside {base}. Set "
            "POLYGON_MCP_ROOT to a directory that contains it, or pass the "
            "content inline."
        )
    if not resolved.is_file():
        raise PolygonError(f"Not a regular file: {resolved}")
    return resolved
