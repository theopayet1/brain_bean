# 🧠 brain_bean

Second brain: automatically brings my Obsidian vaults from every project together in one place.

> ⚠️ **Read-only copy.** Every vault folder is overwritten on each sync. To edit a note, change it in its source repo.

## How it works

- `.github/workflows/sync.yml` runs every 6 hours (or manually from the **Actions** tab) and copies each vault listed in `repos.txt`.
- It reads the private source repos with the `DOCS_READ_TOKEN` secret (fine-grained token, *Contents: Read-only*).

## Add a vault

1. Add the repo to the `DOCS_READ_TOKEN` token (GitHub → Settings → Developer settings → Fine-grained tokens).
2. Add a line to `repos.txt`: `<repo>  <source folder>  <destination folder>`.
3. Commit and push; the sync runs automatically.
