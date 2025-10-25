# Scripts de build

Ce dossier contient des scripts CMake utilitaires pour le processus de build.

## remove_cpp_from_headers.cmake

Script de nettoyage et réorganisation des headers publics dans `bin/include/`.

### Opérations effectuées

1. **Suppression des fichiers .cpp**
   - Tous les fichiers `.cpp` sont supprimés de `bin/include/`
   - Seuls les headers (`.hpp`, `.h`, `.inl`) sont conservés

2. **Remontée des fichiers redondants**
   - Si un fichier porte le même nom que son dossier parent, il est remonté d'un niveau
   - Exemple : `BoxGeometry/BoxGeometry.hpp` → `BoxGeometry.hpp`
   - Les déplacements sont mémorisés pour la mise à jour des includes
   - **Important** : TOUS les fichiers sont déplacés AVANT toute modification des includes

3. **Mise à jour complète des includes** ⭐ (après tous les déplacements)
   - **Étape 3a** : Correction des chemins pour les fichiers déplacés
     - Exemple : `#include "engine/core/.../BoxGeometry/BoxGeometry.hpp"` → `#include "engine/core/.../BoxGeometry.hpp"`
   - **Étape 3b** : Conversion en chemins relatifs
     - Tous les `#include` deviennent relatifs au fichier qui les inclut
     - Exemple dans `core/engine.hpp` :
       - Avant : `#include "engine/core/Camera/PerspectiveCamera.hpp"`
       - Après : `#include "Camera/PerspectiveCamera.hpp"`

4. **Suppression des dossiers vides**
   - Après toutes les opérations, les dossiers vides sont supprimés
   - Exécuté plusieurs fois pour gérer les dossiers imbriqués

### Utilisation

Le script est automatiquement appelé par CMake après la compilation de la bibliothèque :

```cmake
add_custom_command(
    TARGET NoxEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/scripts/remove_cpp_from_headers.cmake"
    ...
)
```

Vous pouvez aussi l'exécuter manuellement :

```bash
cmake -P scripts/remove_cpp_from_headers.cmake
```

### Exemple de transformation

**Avant (copie brute de src/) :**
```
bin/include/
├── core/
│   ├── Actor/
│   │   ├── Geometries/
│   │   │   └── 3D/
│   │   │       └── BoxGeometry/
│   │   │           ├── BoxGeometry.cpp  ❌
│   │   │           └── BoxGeometry.hpp
│   │   ├── Light/
│   │   │   └── AmbientLight/
│   │   │       ├── AmbientLight.cpp  ❌
│   │   │       └── AmbientLight.hpp
```

**Après (headers publics optimisés) :**
```
bin/include/
├── core/
│   ├── engine.hpp (includes relatifs: "Camera/PerspectiveCamera.hpp")
│   ├── Actor/
│   │   ├── Geometries/
│   │   │   └── 3D/
│   │   │       └── BoxGeometry.hpp  ✅ (remonté)
│   │   ├── Light/
│   │   │   └── AmbientLight.hpp  ✅ (remonté)
```

**Includes dans `core/engine.hpp` :**
```cpp
// Avant (chemins absolus depuis root)
#include "engine/core/Camera/PerspectiveCamera.hpp"
#include "engine/core/Actor/Light/AmbientLight.hpp"

// Après (chemins relatifs au fichier)
#include "Camera/PerspectiveCamera.hpp"  ✅
#include "Actor/Light/AmbientLight.hpp"  ✅
```

### Avantages

- **API plus propre** : Moins de niveaux de dossiers inutiles
- **Includes simplifiés** : Pas de redondance (ex: `AmbientLight.hpp` au lieu de `AmbientLight/AmbientLight.hpp`)
- **Chemins relatifs** : Les includes sont relatifs au fichier, plus lisibles et maintenables
- **Portabilité** : Structure indépendante de la racine du projet
- **Pas de fichiers .cpp** : L'API publique ne contient que les déclarations
- **Structure optimisée** : Pas de dossiers vides
