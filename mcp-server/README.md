# cp-polygon-mcp

The MCP server behind `cp-problem-generation:uploading-to-polygon`. It wraps
the [Codeforces Polygon API](https://codeforces.github.io/polygon-misc/API):
one tool per API method, signed and paced, with the credentials confined to the
server's own process.

## Setup

1. **Get an API key.** Polygon → *Settings* → *API keys* → *Add API key*. You
   get a key and a secret; the secret is shown once.

2. **Put them in the environment Claude Code launches with.** `.mcp.json`
   passes them through as `${POLYGON_API_KEY}` / `${POLYGON_API_SECRET}`, and
   that expansion reads Claude Code's own process environment. Either:

   ```powershell
   # Windows, persisted for your user
   setx POLYGON_API_KEY  "<key>"
   setx POLYGON_API_SECRET "<secret>"
   ```

   ```bash
   # macOS / Linux, in ~/.bashrc or ~/.zshrc
   export POLYGON_API_KEY="<key>"
   export POLYGON_API_SECRET="<secret>"
   ```

   or an `env` block in your **user** settings, `~/.claude/settings.json`:

   ```json
   { "env": { "POLYGON_API_KEY": "<key>", "POLYGON_API_SECRET": "<secret>" } }
   ```

   Then restart the terminal *and* Claude Code. The server reads its
   environment once at launch, so exporting a variable in some other terminal
   mid-session changes nothing.

3. **Check it.** Ask Claude to run `polygon_whoami` — the cheapest call that
   proves the key, the secret and the clock are all good. Polygon refuses a
   request whose timestamp is more than five minutes off its own clock, so a
   skewed system clock fails exactly like a bad signature.

`uvx` runs the server straight from this directory; nothing needs installing by
hand. It requires [uv](https://docs.astral.sh/uv/) and Python 3.10+.

### Not a `.env` file, and not inside this plugin

Nothing here loads a dotenv — the server reads `os.environ` and only that. More
to the point, an installed plugin lives at a version-scoped path
(`~/.claude/plugins/cache/<plugin>/<plugin>/<version>/`), so a secret written
beside these files would be orphaned by the next version bump and silently stop
working. Credentials belong to your user, not to a copy of the plugin.

If the key never arrives, the failure is loud by design: an unset variable
reaches the process as the literal string `${POLYGON_API_KEY}`, and the server
treats an unexpanded placeholder as unset so you get "no credentials" rather
than `Incorrect signature`.

## Configuration

| Variable | Default | What it does |
| --- | --- | --- |
| `POLYGON_API_KEY` | — | required |
| `POLYGON_API_SECRET` | — | required |
| `POLYGON_MCP_ROOT` | the server's working directory | the one directory a `path=` argument may read from |
| `POLYGON_BASE_URL` | `https://polygon.codeforces.com/api/` | for a private Polygon instance |
| `POLYGON_TIMEOUT` | `30` | seconds per request |
| `POLYGON_MIN_INTERVAL` | `0.5` | seconds between requests |

`POLYGON_MCP_ROOT` normally needs no setting: Claude Code starts the server in
the directory the session was opened in, which is the problem repository, and
that becomes the root. Set it only when the files you want to upload live
somewhere else.

## Secrets

The credentials are read from this process's environment and never leave it.
No tool returns them, logs them or writes them to disk, and no error message is
built by stringifying an `httpx` exception — httpx spells the request URL into
everything it raises, and a signed GET's URL carries `apiKey`.

If a tool reports the credentials are missing, fix it here. Never paste a key
or a secret into a conversation.

## Tools

Thirty, listed by upload phase in
[`../skills/uploading-to-polygon/references/polygon-tools.md`](../skills/uploading-to-polygon/references/polygon-tools.md),
with the API method each wraps. The loaded tool schema is always the authority
over that file.

A tool never raises. Every call returns a dict carrying `ok`; a failure is
`{"ok": false, "error": "<Polygon's own comment>", "method": "<the API
method>"}`, sometimes with `details` — `problem.deleteTest` reports which tests
it refused, and why, that way. Read the comment and correct the call rather
than retrying the identical request.

**There are no group tools.** `problem.enableGroups`, `problem.setTestGroup`,
`problem.saveTestGroup` and `problem.viewTestGroup` have no wrapper, because
the skill scores every test individually and never enables groups. So do
`problem.setInteractor`, `problem.saveTags` and
`problem.saveGeneralDescription`. This is deliberate: a rule that cannot be
violated beats a rule that is written down.

## Tests

```bash
uv run --extra dev pytest
```

Offline: an `httpx.MockTransport` stands in for Polygon, so the signature, the
read/write verb split, the pacing and the retry are all the real code and only
the socket is fake.

## Provenance

Adapted from the bundled Polygon MCP server in the
[`competitive-programming`](https://github.com/LLaammTTeerr/competitive-programming)
plugin by LamTer, reused with attribution. The divergences — the pruned tool
set, `str | None` parameters so an empty string can clear a field, verbatim
line endings, the working-directory root default, and the five added tools —
are documented at the top of `src/polygon_mcp/server.py` and covered by the
test suite.
