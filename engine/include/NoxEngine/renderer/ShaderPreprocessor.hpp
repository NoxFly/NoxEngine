// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Nox {

    /// @brief Preprocessor for GLSL shaders that resolves #include directives.
    ///
    /// Supports two include styles:
    /// - `#include "relative/path.glsl"` — resolved relative to the including file
    /// - `#include <alias:path/to/file.glsl>` — resolved via registered aliases
    ///
    /// Example alias: register "nox" → "engine/shaders/" then use
    /// `#include <nox:lighting/pbr.glsl>` which resolves to "engine/shaders/lighting/pbr.glsl".
    class ShaderPreprocessor {
    public:
        /// @brief Register an alias for shader includes.
        /// @param alias The alias name (e.g., "nox").
        /// @param basePath The base directory path that the alias maps to.
        void registerAlias(std::string_view alias, const std::filesystem::path& basePath);

        /// @brief Process a shader source string, resolving all #include directives.
        /// @param source The raw shader source.
        /// @param sourceDir The directory of the source file (for relative includes).
        /// @return The fully preprocessed shader source with includes inlined.
        [[nodiscard]] std::string process(std::string_view source,
                                           const std::filesystem::path& sourceDir = "") const;

        /// @brief Process a shader loaded from a file.
        /// @param filePath Path to the shader file.
        /// @return The fully preprocessed shader source.
        [[nodiscard]] std::string processFile(const std::filesystem::path& filePath) const;

    private:
        /// @brief Recursively resolve includes in the source.
        /// @param source The source to process.
        /// @param sourceDir Directory for resolving relative includes.
        /// @param depth Current recursion depth (to detect cycles).
        /// @param included Set of already-included files (to prevent duplicates).
        [[nodiscard]] std::string resolveIncludes(
            std::string_view source,
            const std::filesystem::path& sourceDir,
            int depth,
            std::unordered_map<std::string, bool>& included) const;

        /// Alias name → base directory path.
        std::unordered_map<std::string, std::filesystem::path> aliases_;

        static constexpr int MaxIncludeDepth = 32;
    };

} // namespace Nox
