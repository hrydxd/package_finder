#!/bin/bash

set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}     pckgfind - Package Finder${NC}"
echo -e "${BLUE}========================================${NC}"

if [ ! -f "pckgfind.c" ]; then
    echo -e "${RED}Error: pckgfind.c not found in current directory${NC}"
    exit 1
fi

echo -e "${YELLOW}Compiling pckgfind...${NC}"
gcc -Wall -Wextra -O2 -o pckgfind pckgfind.c

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Compilation successful!${NC}"

echo -e "${YELLOW}Installing to /usr/local/bin...${NC}"
sudo cp pckgfind /usr/local/bin/
sudo chmod +x /usr/local/bin/pckgfind

if [ $? -ne 0 ]; then
    echo -e "${RED}Installation failed! Try running without sudo?${NC}"
    echo -e "${YELLOW}Installing to ~/.local/bin instead...${NC}"
    mkdir -p ~/.local/bin
    cp pckgfind ~/.local/bin/
    chmod +x ~/.local/bin/pckgfind
    
    if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
        echo -e "${YELLOW}Adding ~/.local/bin to PATH in .bashrc...${NC}"
        echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
        echo -e "${GREEN}Added to PATH. Please run: source ~/.bashrc${NC}"
    fi
    
    echo -e "${GREEN}Installed to ~/.local/bin/pckgfind${NC}"
else
    echo -e "${GREEN}Installed to /usr/local/bin/pckgfind${NC}"
fi

echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Installation complete!${NC}"
echo -e "${BLUE}Usage:${NC}"
echo -e "  ${YELLOW}pckgfind <package_name>${NC}"
echo -e "${BLUE}========================================${NC}"
