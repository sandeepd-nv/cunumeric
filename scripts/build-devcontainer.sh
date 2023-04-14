#!/bin/bash

set -eo pipefail

apt update;
DEBIAN_FRONTEND=noninteractive \
apt install -y --no-install-recommends \
    numactl rlwrap;

git clone https://github.com/trxcllnt/legate-separate-build-scripts.git /tmp/repo;
mv /tmp/repo/home/coder/.gitconfig /home/coder/.gitconfig;
mv /tmp/repo/home/coder/.local /home/coder/.local;
chmod a+x /home/coder/.local/bin/*;
chown -R coder:coder /home/coder/;
rm -rf /tmp/repo;

su - coder -Pc "                                   \
PATH=${PATH}                                       \
VAULT_HOST=${VAULT_HOST}                           \
GH_TOKEN=${GITHUB_TOKEN}                           \
GITHUB_TOKEN=${GITHUB_TOKEN}                       \
SCCACHE_BUCKET=${SCCACHE_BUCKET}                   \
SCCACHE_REGION=${SCCACHE_REGION}                   \
SCCACHE_S3_KEY_PREFIX=${SCCACHE_S3_KEY_PREFIX}     \
PYTHONDONTWRITEBYTECODE=${PYTHONDONTWRITEBYTECODE} \
~/.local/bin/entrypoint bash -i build-all";