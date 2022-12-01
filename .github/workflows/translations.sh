#! /usr/bin/env bash
set -e -u -o pipefail

restore_po_creation_date_from_git() {
    local filename="${1}"
    local original="$(git show "HEAD:./${filename}" 2>/dev/null | grep -nF POT-Creation-Date:)"
    if [[ -z ${original} ]]; then
        echo "[*] Skipped unversioned file \"${filename}\"."
        return 0
    fi
    local line_number="${original%%:*}"
    local line_content="${original#*:}"
    {
        sed -n "1,$(( line_number - 1 ))p" "${filename}"
        echo "${line_content}"
        sed -n "$(( line_number + 1 )),\$p" "${filename}"
    } | sponge "${filename}"
    echo "[+] Restored POT-Creation-Date for file \"${filename}\"."
}

gettext --version | head -n1

echo '[*] Update existing .pot files...'
for i in libvisual libvisual-plugins ; do
    make -C "${i}"/po update-pot >/dev/null
done

echo '[*] Update existing .po files...'
for i in libvisual libvisual-plugins ; do
    make -C "${i}"/po update-po >/dev/null
done

echo '[*] Restoring creation date headers...'
for i in */po/*.{po,pot}; do
    restore_po_creation_date_from_git "${i}"
done

echo '[+] DONE.'
