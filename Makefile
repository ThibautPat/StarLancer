# Makefile pour le serveur de jeu spatial
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
LDFLAGS = -pthread

# Nom de l'exécutable
TARGET = server

# Fichiers sources
SOURCES = main.cpp \
          Network.cpp \
          ServerNetwork.cpp \
          EntityServer.cpp \
          EntityShipServer.cpp \
          EntityBulletServer.cpp \
          EntityPlanetServer.cpp \
          SpaceShipMove_Calculator.cpp \
          pch.cpp

# Fichiers objets (générés automatiquement à partir des sources)
OBJECTS = $(SOURCES:.cpp=.o)

# Règle par défaut
all: $(TARGET)

# Règle de compilation de l'exécutable
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "Compilation terminée ! Exécutable: $(TARGET)"

# Règle de compilation des fichiers .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyer les fichiers compilés
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Nettoyage effectué"

# Recompilation complète
rebuild: clean all

# Exécuter le serveur
run: $(TARGET)
	./$(TARGET)

# Règle pour afficher les informations
info:
	@echo "Compilateur: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Sources: $(SOURCES)"
	@echo "Target: $(TARGET)"

.PHONY: all clean rebuild run info
