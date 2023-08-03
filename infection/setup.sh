#!/bin/bash

repo_url="https://github.com/gophish/gophish.git"
repo_name=$(echo "$repo_url" | awk -F/ '{print $(NF-1)}' | sed 's/\.git$//')

if ! command -v go &>/dev/null; then
  echo "Go is not installed. Please install it to continue."
  exit 1
fi

if ! command -v git &>/dev/null; then
  echo "Git is not installed. Please install it to continue."
  exit 1
fi

echo "Cloning gopish..."
git clone "$repo_url"

cd "$repo_name"

echo "Installing gopish..."
go build

if [ $? -ne 0 ]; then
  echo "The build failed. Please check the errors above."
  exit 1
fi

echo "Launching gopish..."
./"$repo_name"
