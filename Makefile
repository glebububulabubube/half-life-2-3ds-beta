name: Build HL-DEMO

on:
  push:
  workflow_dispatch:

jobs:
  build:
    runs-on: ubuntu-latest
    container:
      image: devkitpro/devkitarm
    env:
      DEVKITPRO: /opt/devkitpro
      DEVKITARM: /opt/devkitpro/devkitARM
    steps:
      - uses: actions/checkout@v4

      - name: Install makerom
        run: |
          cd /tmp
          curl -sSL -o makerom.zip https://github.com/3DSGuy/Project_CTR/releases/download/makerom-v0.18.4/makerom-v0.18.4-ubuntu_x86_64.zip
          unzip -o makerom.zip
          mkdir -p /usr/local/bin
          cp makerom /usr/local/bin/
          chmod +x /usr/local/bin/makerom

      - name: Build
        run: make

      - name: Upload CIA + 3DSX
        uses: actions/upload-artifact@v4
        with:
          name: hl-demo-3ds
          path: |
            hl-demo.cia
            hl-demo.3dsx
