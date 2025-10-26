#!/bin/bash

mkdir -p .vscode

cat > ~/.config/Code/User/settings.json << 'EOF'
{
  "editor.insertSpaces": false,
  "editor.tabSize": 8,
  "editor.detectIndentation": false,
  "editor.rulers": [80],
  "files.trimTrailingWhitespace": true,
  "files.insertFinalNewline": true,
  "[c]": {
    "editor.defaultFormatter": "ms-vscode.cpptools",
    "editor.insertSpaces": false,
    "editor.tabSize": 8
  }
}
EOF

# Copy the .editorconfig above

echo "✓ VS Code configured for Linux kernel development"
