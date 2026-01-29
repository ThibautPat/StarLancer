#!/bin/bash

echo "======================================"
echo "  Compilation du serveur de jeu"
echo "======================================"
echo ""

# Vérifier si Make est installé
if ! command -v make &> /dev/null; then
    echo "❌ Make n'est pas installé."
    echo "Installez-le avec: sudo apt install make"
    exit 1
fi

# Vérifier si g++ est installé
if ! command -v g++ &> /dev/null; then
    echo "❌ G++ n'est pas installé."
    echo "Installez-le avec: sudo apt install g++"
    exit 1
fi

echo "✅ Outils de compilation détectés"
echo ""

# Nettoyer les anciens builds
echo "🧹 Nettoyage des anciens builds..."
make clean > /dev/null 2>&1

# Compiler
echo "🔨 Compilation en cours..."
if make; then
    echo ""
    echo "✅ Compilation réussie !"
    echo ""
    echo "Pour lancer le serveur:"
    echo "  ./server"
    echo ""
    echo "Pour arrêter le serveur:"
    echo "  Ctrl+C"
    echo ""
else
    echo ""
    echo "❌ Erreur de compilation"
    exit 1
fi
