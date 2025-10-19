# NoxEngine - Guide de compilation

## Prérequis

Avant de compiler, vous devez installer les dépendances suivantes :
- **GLEW** (OpenGL Extension Wrangler)
- **SDL2** avec SDL2_image et SDL2_ttf
- **GLM** (OpenGL Mathematics - header-only)
- **Visual Studio 2022** (ou compilateur compatible C++23)

## Configuration des dépendances

### Création du fichier config.cmake

1. Copiez `config-example.cmake` vers `config.cmake`
2. Modifiez les chemins ROOT pour chaque bibliothèque :

```cmake
# Exemple de configuration
set(GLEW_ROOT "C:/libs/glew-2.2.0")
set(SDL2_ROOT "C:/libs/SDL2-2.26.2")
set(GLM_ROOT "C:/libs/glm-0.9.9")
```

3. Les chemins des includes, libraries et DLLs sont automatiquement déduits du ROOT
4. Vous pouvez ajuster les chemins relatifs si votre structure est différente

**Note** : Le fichier `config.cmake` est ignoré par Git (ajouté au .gitignore) car il contient des chemins spécifiques à votre machine.

## Configuration du projet

Le projet NoxEngine compile désormais une **bibliothèque partagée** (`NoxEngine.dll`) et des **exemples exécutables** qui s'y lient.

### Structure de compilation

- **Bibliothèque** : `src/` → `bin/NoxEngine.dll`
- **Headers publics** : `src/` → `bin/include/NoxEngine/` (copie automatique et optimisée)
  - Copie des fichiers `.hpp`, `.h`, `.inl` (pas de `.cpp`)
  - Remontée des fichiers redondants (ex: `BoxGeometry/BoxGeometry.hpp` → `BoxGeometry.hpp`)
  - Conversion des `#include` pour être relatifs à `bin/include/NoxEngine/`
  - Suppression des dossiers vides
- **Exemples** : `examples/` → `bin/examples/<nom>.exe`

**Important** : Les exemples utilisent les headers de `bin/include/NoxEngine/` et non directement ceux de `src/`. Cela simule l'utilisation de la bibliothèque comme si elle était installée sur le système. Les headers sont automatiquement copiés et optimisés lors de la compilation de la bibliothèque.

#### Exemple de transformation des headers

**Source (`src/`)** :
```
src/core/Actor/Light/AmbientLight/
├── AmbientLight.cpp  (implémentation)
└── AmbientLight.hpp  (interface)
```

**Headers publics (`bin/include/NoxEngine/`)** :
```
bin/include/NoxEngine/core/Actor/Light/
└── AmbientLight.hpp  (interface uniquement, remonté d'un niveau)
```

Cela permet des includes plus simples et évite les conflits de noms :
```cpp
// Dans vos exemples ou projets externes
#include "NoxEngine/core/Actor/Light/AmbientLight.hpp"  // ✅ Simplifié avec namespace
// au lieu de
#include "core/Actor/Light/AmbientLight/AmbientLight.hpp"  // ❌ Redondant
```

## Utilisation avec VS Code et l'extension CMake

### 1. Configuration initiale

1. Assurez-vous d'avoir créé et configuré `config.cmake`
2. Ouvrez le projet dans VS Code
3. Dans la vue **CMake**, cliquez sur "Configure" et sélectionnez le kit :
   - **Visual Studio Community 2022 Release - amd64** (recommandé)
   - ou un autre kit compatible

4. Sélectionnez le preset de configuration :
   - **x64-debug** (pour le débogage)
   - **x64-release** (pour la version optimisée)

### 2. Compilation de la bibliothèque

**Option A : Via l'extension CMake**
- Dans la vue CMake, sélectionnez la cible `NoxEngine`
- Cliquez sur "Build"

**Option B : Via les tâches VS Code**
- `Ctrl+Shift+B` → "Build NoxEngine Library"
- Ou via la palette de commandes : "Tasks: Run Build Task"

### 3. Compilation des exemples

**Pour compiler TOUS les exemples :**
- Tâche : "Build All Examples"
- Via la palette : `Ctrl+Shift+P` → "Tasks: Run Task" → "Build All Examples"

**Pour compiler UN exemple spécifique :**
- Tâche : "Build Current Example"
- Vous serez invité à entrer le nom de la cible (ex: `base3D`, `camera_pointerLock`)

### 4. Exécution et débogage

**Lancer un exemple :**
1. Ouvrez le fichier de l'exemple dans l'éditeur (ex: `examples/base3D.cpp`)
2. Appuyez sur `F5` ou allez dans "Run and Debug"
3. Choisissez une configuration :
   - **(Windows) Launch Example** : Vous demande quel exemple lancer
   - **(Windows) Launch base3D** : Lance directement base3D
   - **(Windows) Launch camera_pointerLock** : Lance directement camera_pointerLock

**Note importante :** Les exemples s'exécutent depuis la racine du projet (`${workspaceFolder}`) pour accéder aux ressources dans `res/`.

## Commandes CMake en ligne de commande

Si vous préférez utiliser la ligne de commande :

```bash
# Configuration
cmake --preset=x64-debug

# Compilation de la bibliothèque seulement
cmake --build out/build/x64-debug --target NoxEngine

# Compilation de tous les exemples
cmake --build out/build/x64-debug

# Compilation d'un exemple spécifique
cmake --build out/build/x64-debug --target base3D
cmake --build out/build/x64-debug --target camera_pointerLock
```

## Nommage des cibles d'exemples

Les cibles des exemples suivent cette convention :
- Fichiers à la racine : `examples/base3D.cpp` → cible `base3D`
- Fichiers dans des sous-dossiers : `examples/camera/pointerLock.cpp` → cible `camera_pointerLock`

## Résolution des problèmes

### "No Kit Selected"
1. Cliquez sur "[No Kit Selected]" dans la vue CMake
2. Sélectionnez un kit Visual Studio ou MSYS2/MinGW

### Erreurs de compilation
- Vérifiez que `config.cmake` existe et contient les bonnes variables d'environnement
- Assurez-vous que GLEW, SDL2 (avec SDL2_image et SDL2_ttf), et GLM sont correctement installés
- Vérifiez les chemins dans `config.cmake`

### Les DLL ne sont pas trouvées
Les DLL GLEW et SDL2 (SDL2.dll, SDL2_image.dll, SDL2_ttf.dll) sont automatiquement copiées dans `bin/` et `bin/examples/` lors de la compilation.
Si vous rencontrez des erreurs, vérifiez que les chemins dans le CMakeLists.txt sont corrects.

## Structure du projet après compilation

```
bin/
├── NoxEngine.dll          # Bibliothèque partagée
├── glew32d.dll            # Dépendance GLEW (Debug)
├── SDL2.dll               # Dépendance SDL2
├── SDL2_image.dll         # Extension SDL2 pour les images
├── SDL2_ttf.dll           # Extension SDL2 pour les fonts
├── include/               # Headers publics (copie de src/)
│   ├── core/
│   │   ├── engine.hpp
│   │   ├── engine.typedef.hpp
│   │   ├── Actor/
│   │   ├── Camera/
│   │   ├── Controls/
│   │   ├── Renderer/
│   │   └── Scene/
│   ├── Console/
│   ├── IniSet/
│   └── utils/
└── examples/              # Exécutables des exemples
    ├── base3D.exe
    ├── camera_pointerLock.exe
    ├── NoxEngine.dll      # Copie de la bibliothèque
    ├── glew32d.dll        # Copie des dépendances
    ├── SDL2.dll
    ├── SDL2_image.dll
    └── SDL2_ttf.dll
```

**Note** : Les exemples incluent les headers depuis `bin/include/NoxEngine/`, pas directement depuis `src/`. Cela garantit qu'ils utilisent l'API publique de la bibliothèque avec le namespace approprié (`#include "NoxEngine/core/engine.hpp"`).
