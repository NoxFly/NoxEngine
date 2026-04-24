// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/renderer/ShaderPreprocessor.hpp>

#include <NoxEngine/core/Logger.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Nox {

    void ShaderPreprocessor::registerAlias(std::string_view alias,
                                            const std::filesystem::path& basePath)
    {
        aliases_[std::string(alias)] = basePath;
    }

    std::string ShaderPreprocessor::process(std::string_view source,
                                             const std::filesystem::path& sourceDir) const
    {
        std::unordered_map<std::string, bool> included;
        return resolveIncludes(source, sourceDir, 0, included);
    }

    std::string ShaderPreprocessor::processFile(const std::filesystem::path& filePath) const
    {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("ShaderPreprocessor: cannot open file '" +
                                     filePath.string() + "'");
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        return process(ss.str(), filePath.parent_path());
    }

    std::string ShaderPreprocessor::resolveIncludes(
        std::string_view source,
        const std::filesystem::path& sourceDir,
        int depth,
        std::unordered_map<std::string, bool>& included) const
    {
        if (depth > MaxIncludeDepth) {
            throw std::runtime_error("ShaderPreprocessor: maximum include depth exceeded "
                                     "(possible circular include)");
        }

        std::string result;
        result.reserve(source.size());

        size_t pos = 0;
        while (pos < source.size()) {
            // Find next line
            size_t lineEnd = source.find('\n', pos);
            if (lineEnd == std::string_view::npos) {
                lineEnd = source.size();
            }

            std::string_view line = source.substr(pos, lineEnd - pos);

            // Check for #include directive
            // Strip leading whitespace
            size_t firstNonSpace = line.find_first_not_of(" \t");
            if (firstNonSpace != std::string_view::npos &&
                line.substr(firstNonSpace).starts_with("#include"))
            {
                std::string_view directive = line.substr(firstNonSpace + 8);
                // Strip whitespace after #include
                size_t start = directive.find_first_not_of(" \t");
                if (start != std::string_view::npos) {
                    directive = directive.substr(start);
                }

                std::filesystem::path includePath;
                bool resolved = false;

                if (directive.size() >= 2 && directive.front() == '"') {
                    // #include "relative/path.glsl"
                    size_t closeQuote = directive.find('"', 1);
                    if (closeQuote != std::string_view::npos) {
                        std::string relPath(directive.substr(1, closeQuote - 1));
                        includePath = sourceDir / relPath;
                        resolved = true;
                    }
                }
                else if (directive.size() >= 2 && directive.front() == '<') {
                    // #include <alias:path/to/file.glsl>
                    size_t closeAngle = directive.find('>');
                    if (closeAngle != std::string_view::npos) {
                        std::string inner(directive.substr(1, closeAngle - 1));
                        size_t colonPos = inner.find(':');
                        if (colonPos != std::string::npos) {
                            std::string alias = inner.substr(0, colonPos);
                            std::string subPath = inner.substr(colonPos + 1);

                            auto it = aliases_.find(alias);
                            if (it != aliases_.end()) {
                                includePath = it->second / subPath;
                                resolved = true;
                            }
                            else {
                                NOX_LOG_WARN("ShaderPreprocessor: unknown alias '{}' in "
                                             "#include <{}:{}>", alias, alias, subPath);
                            }
                        }
                        else {
                            // No alias — treat as plain path
                            includePath = sourceDir / inner;
                            resolved = true;
                        }
                    }
                }

                if (resolved) {
                    auto canonical = std::filesystem::weakly_canonical(includePath).string();

                    if (!included[canonical]) {
                        included[canonical] = true;

                        std::ifstream incFile(includePath);
                        if (!incFile.is_open()) {
                            throw std::runtime_error(
                                "ShaderPreprocessor: cannot open included file '" +
                                includePath.string() + "'");
                        }

                        std::ostringstream ss;
                        ss << incFile.rdbuf();
                        std::string incSource = ss.str();

                        result += "// --- begin include: " + canonical + " ---\n";
                        result += resolveIncludes(incSource,
                                                  includePath.parent_path(),
                                                  depth + 1, included);
                        result += "\n// --- end include: " + canonical + " ---\n";
                    }
                    // else: already included, skip (include guard)
                }
                else {
                    // Not a recognized include — keep the line as-is
                    result += line;
                    result += '\n';
                }
            }
            else {
                result += line;
                result += '\n';
            }

            pos = lineEnd + 1;
        }

        return result;
    }

} // namespace Nox
