// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/editor/SceneSerializer.hpp>
#include <NoxEngine/core/Logger.hpp>
#include <NoxEngine/scene/Geometry.hpp>
#include <NoxEngine/scene/Light.hpp>
#include <NoxEngine/scene/Material.hpp>
#include <NoxEngine/scene/Mesh.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#include <fstream>
#include <format>
#include <sstream>
#include <string>

namespace Nox {

    namespace {

        // ── Minimal JSON writer ────────────────────────────────────

        class JsonWriter {
        public:
            void beginObject() { out_ += "{"; indent_++; }
            void endObject()   { indent_--; newline(); out_ += "}"; }
            void beginArray()  { out_ += "["; indent_++; }
            void endArray()    { indent_--; newline(); out_ += "]"; }

            void key(std::string_view k) {
                newline();
                out_ += std::format("\"{}\":", k);
            }

            void value(std::string_view v) { out_ += std::format("\"{}\"", v); }
            void value(float v)            { out_ += std::format("{:.6f}", v); }
            void value(int v)              { out_ += std::format("{}", v); }
            void value(bool v)             { out_ += (v ? "true" : "false"); }

            void comma() { out_ += ","; }

            void vec3(const Math::Vec3& v) {
                out_ += std::format("[{:.6f},{:.6f},{:.6f}]", v.x, v.y, v.z);
            }

            void vec4(float x, float y, float z, float w) {
                out_ += std::format("[{:.6f},{:.6f},{:.6f},{:.6f}]", x, y, z, w);
            }

            void quat(const Math::Quat& q) {
                out_ += std::format("[{:.6f},{:.6f},{:.6f},{:.6f}]", q.w, q.x, q.y, q.z);
            }

            [[nodiscard]] const std::string& str() const { return out_; }

        private:
            void newline() {
                out_ += "\n";
                for (int i = 0; i < indent_; ++i) { out_ += "  "; }
            }

            std::string out_;
            int indent_ = 0;
        };

        // ── Minimal JSON reader (token-based) ──────────────────────

        struct JsonToken {
            enum Type { String, Number, Bool, Null, ObjectStart, ObjectEnd, ArrayStart, ArrayEnd, Comma, Colon, Eof };
            Type type = Eof;
            std::string str;
            float num = 0.0f;
            bool boolVal = false;
        };

        class JsonReader {
        public:
            explicit JsonReader(std::string_view input) : data_(input), pos_(0) {}

            JsonToken next() {
                skipWhitespace();
                if (pos_ >= data_.size()) { return { JsonToken::Eof }; }

                char c = data_[pos_];
                switch (c) {
                    case '{': pos_++; return { JsonToken::ObjectStart };
                    case '}': pos_++; return { JsonToken::ObjectEnd };
                    case '[': pos_++; return { JsonToken::ArrayStart };
                    case ']': pos_++; return { JsonToken::ArrayEnd };
                    case ',': pos_++; return { JsonToken::Comma };
                    case ':': pos_++; return { JsonToken::Colon };
                    case '"': return readString();
                    case 't': case 'f': return readBool();
                    case 'n': return readNull();
                    default:
                        if (c == '-' || (c >= '0' && c <= '9')) {
                            return readNumber();
                        }
                        pos_++;
                        return { JsonToken::Eof };
                }
            }

            JsonToken peek() {
                size_t saved = pos_;
                auto tok = next();
                pos_ = saved;
                return tok;
            }

            void expect(JsonToken::Type type) {
                auto tok = next();
                if (tok.type != type) {
                    NOX_LOG_ERROR("JSON parse error: unexpected token");
                }
            }

            std::string expectString() {
                auto tok = next();
                return tok.str;
            }

            float expectNumber() {
                auto tok = next();
                return tok.num;
            }

            Math::Vec3 readVec3() {
                expect(JsonToken::ArrayStart);
                float x = expectNumber(); expect(JsonToken::Comma);
                float y = expectNumber(); expect(JsonToken::Comma);
                float z = expectNumber();
                expect(JsonToken::ArrayEnd);
                return { x, y, z };
            }

            Math::Quat readQuat() {
                expect(JsonToken::ArrayStart);
                float w = expectNumber(); expect(JsonToken::Comma);
                float x = expectNumber(); expect(JsonToken::Comma);
                float y = expectNumber(); expect(JsonToken::Comma);
                float z = expectNumber();
                expect(JsonToken::ArrayEnd);
                return { w, x, y, z };
            }

            Color readColor() {
                expect(JsonToken::ArrayStart);
                float r = expectNumber(); expect(JsonToken::Comma);
                float g = expectNumber(); expect(JsonToken::Comma);
                float b = expectNumber(); expect(JsonToken::Comma);
                float a = expectNumber();
                expect(JsonToken::ArrayEnd);
                return { r, g, b, a };
            }

            void skipValue() {
                auto tok = next();
                switch (tok.type) {
                    case JsonToken::ObjectStart: {
                        while (peek().type != JsonToken::ObjectEnd) {
                            next(); // key
                            expect(JsonToken::Colon);
                            skipValue();
                            if (peek().type == JsonToken::Comma) { next(); }
                        }
                        expect(JsonToken::ObjectEnd);
                        break;
                    }
                    case JsonToken::ArrayStart: {
                        while (peek().type != JsonToken::ArrayEnd) {
                            skipValue();
                            if (peek().type == JsonToken::Comma) { next(); }
                        }
                        expect(JsonToken::ArrayEnd);
                        break;
                    }
                    default: break;
                }
            }

        private:
            void skipWhitespace() {
                while (pos_ < data_.size() && (data_[pos_] == ' ' || data_[pos_] == '\n'
                       || data_[pos_] == '\r' || data_[pos_] == '\t')) {
                    pos_++;
                }
            }

            JsonToken readString() {
                pos_++; // skip opening quote
                std::string s;
                while (pos_ < data_.size() && data_[pos_] != '"') {
                    if (data_[pos_] == '\\' && pos_ + 1 < data_.size()) {
                        pos_++;
                        switch (data_[pos_]) {
                            case '"':  s += '"'; break;
                            case '\\': s += '\\'; break;
                            case 'n':  s += '\n'; break;
                            case 't':  s += '\t'; break;
                            default:   s += data_[pos_]; break;
                        }
                    }
                    else {
                        s += data_[pos_];
                    }
                    pos_++;
                }
                if (pos_ < data_.size()) { pos_++; } // skip closing quote
                return { JsonToken::String, std::move(s) };
            }

            JsonToken readNumber() {
                size_t start = pos_;
                if (data_[pos_] == '-') { pos_++; }
                while (pos_ < data_.size() && ((data_[pos_] >= '0' && data_[pos_] <= '9')
                       || data_[pos_] == '.' || data_[pos_] == 'e' || data_[pos_] == 'E'
                       || data_[pos_] == '+' || data_[pos_] == '-')) {
                    pos_++;
                }
                std::string numStr(data_.substr(start, pos_ - start));
                return { JsonToken::Number, numStr, std::stof(numStr) };
            }

            JsonToken readBool() {
                if (data_.substr(pos_, 4) == "true") {
                    pos_ += 4;
                    return { JsonToken::Bool, "true", 0, true };
                }
                pos_ += 5;
                return { JsonToken::Bool, "false", 0, false };
            }

            JsonToken readNull() {
                pos_ += 4;
                return { JsonToken::Null };
            }

            std::string_view data_;
            size_t pos_;
        };

        // ── Helpers ────────────────────────────────────────────────

        void serializeTransform(JsonWriter& w, const Transform& t) {
            w.key("position"); w.vec3(t.position()); w.comma();
            w.key("rotation"); w.quat(t.rotation()); w.comma();
            w.key("scale"); w.vec3(t.scale());
        }

        void serializeMaterial(JsonWriter& w, const Material& mat) {
            w.key("material"); w.beginObject();
            w.key("type"); w.value(mat.isLit() ? "standard" : "unlit"); w.comma();
            const auto& c = mat.color();
            w.key("color"); w.vec4(c.r, c.g, c.b, c.a); w.comma();
            w.key("roughness"); w.value(mat.roughness()); w.comma();
            w.key("metallic"); w.value(mat.metallic()); w.comma();
            const auto& ec = mat.emissiveColor();
            w.key("emissiveColor"); w.vec4(ec.r, ec.g, ec.b, ec.a); w.comma();
            w.key("emissiveIntensity"); w.value(mat.emissiveIntensity()); w.comma();
            w.key("albedoMap"); w.value(mat.albedoMapPath()); w.comma();
            w.key("normalMap"); w.value(mat.normalMapPath()); w.comma();
            w.key("emissiveMap"); w.value(mat.emissiveMapPath());
            w.endObject();
        }

        void serializeObject(JsonWriter& w, const SceneObject& obj) {
            w.beginObject();

            // Type (using type tags — no RTTI)
            w.key("type");
            switch (obj.objectType()) {
                case SceneObjectType::Mesh:             w.value("Mesh"); break;
                case SceneObjectType::DirectionalLight: w.value("DirectionalLight"); break;
                case SceneObjectType::PointLight:       w.value("PointLight"); break;
                case SceneObjectType::AmbientLight:     w.value("AmbientLight"); break;
                default:                                w.value("SceneObject"); break;
            }
            w.comma();

            // Name
            w.key("name"); w.value(obj.name()); w.comma();

            // Transform
            serializeTransform(w, obj.transform());

            // Type-specific (using type tags — no RTTI)
            switch (obj.objectType()) {
                case SceneObjectType::Mesh: {
                    const auto& mesh = static_cast<const Mesh&>(obj);
                    w.comma();
                    serializeMaterial(w, *mesh.material());
                    break;
                }
                case SceneObjectType::DirectionalLight: {
                    const auto& dir = static_cast<const DirectionalLight&>(obj);
                    w.comma();
                    const auto& c = dir.color();
                    w.key("color"); w.vec4(c.r, c.g, c.b, c.a); w.comma();
                    w.key("intensity"); w.value(dir.intensity()); w.comma();
                    w.key("direction"); w.vec3(dir.direction());
                    break;
                }
                case SceneObjectType::PointLight: {
                    const auto& pt = static_cast<const PointLight&>(obj);
                    w.comma();
                    const auto& c = pt.color();
                    w.key("color"); w.vec4(c.r, c.g, c.b, c.a); w.comma();
                    w.key("intensity"); w.value(pt.intensity()); w.comma();
                    w.key("range"); w.value(pt.range());
                    break;
                }
                case SceneObjectType::AmbientLight: {
                    const auto& amb = static_cast<const AmbientLight&>(obj);
                    w.comma();
                    const auto& c = amb.color();
                    w.key("color"); w.vec4(c.r, c.g, c.b, c.a); w.comma();
                    w.key("intensity"); w.value(amb.intensity());
                    break;
                }
                default:
                    break;
            }

            w.endObject();
        }

        void deserializeObject(JsonReader& reader, Scene3D& scene) {
            reader.expect(JsonToken::ObjectStart);

            std::string type;
            std::string name;
            Math::Vec3 position{ 0.0f };
            Math::Quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
            Math::Vec3 scale{ 1.0f };
            Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
            float intensity = 1.0f;
            Math::Vec3 direction{ -1.0f, -1.0f, -0.5f };
            float range = 10.0f;

            // Material properties
            std::string matType = "standard";
            Color matColor{ 1.0f, 1.0f, 1.0f, 1.0f };
            float roughness = 0.5f;
            float metallic = 0.0f;
            Color emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
            float emissiveIntensity = 0.0f;
            std::string albedoMap, normalMap, emissiveMap;

            while (reader.peek().type != JsonToken::ObjectEnd) {
                std::string key = reader.expectString();
                reader.expect(JsonToken::Colon);

                if (key == "type") { type = reader.expectString(); }
                else if (key == "name") { name = reader.expectString(); }
                else if (key == "position") { position = reader.readVec3(); }
                else if (key == "rotation") { rotation = reader.readQuat(); }
                else if (key == "scale") { scale = reader.readVec3(); }
                else if (key == "color") { color = reader.readColor(); }
                else if (key == "intensity") { intensity = reader.expectNumber(); }
                else if (key == "direction") { direction = reader.readVec3(); }
                else if (key == "range") { range = reader.expectNumber(); }
                else if (key == "material") {
                    reader.expect(JsonToken::ObjectStart);
                    while (reader.peek().type != JsonToken::ObjectEnd) {
                        std::string mKey = reader.expectString();
                        reader.expect(JsonToken::Colon);
                        if (mKey == "type") { matType = reader.expectString(); }
                        else if (mKey == "color") { matColor = reader.readColor(); }
                        else if (mKey == "roughness") { roughness = reader.expectNumber(); }
                        else if (mKey == "metallic") { metallic = reader.expectNumber(); }
                        else if (mKey == "emissiveColor") { emissiveColor = reader.readColor(); }
                        else if (mKey == "emissiveIntensity") { emissiveIntensity = reader.expectNumber(); }
                        else if (mKey == "albedoMap") { albedoMap = reader.expectString(); }
                        else if (mKey == "normalMap") { normalMap = reader.expectString(); }
                        else if (mKey == "emissiveMap") { emissiveMap = reader.expectString(); }
                        else { reader.skipValue(); }
                        if (reader.peek().type == JsonToken::Comma) { reader.next(); }
                    }
                    reader.expect(JsonToken::ObjectEnd);
                }
                else {
                    reader.skipValue();
                }

                if (reader.peek().type == JsonToken::Comma) { reader.next(); }
            }

            reader.expect(JsonToken::ObjectEnd);

            // Construct object
            if (type == "Mesh") {
                auto mat = (matType == "unlit") ? Material::unlit() : Material::standard();
                mat->setColor(matColor);
                mat->setRoughness(roughness);
                mat->setMetallic(metallic);
                mat->setEmissiveColor(emissiveColor);
                mat->setEmissiveIntensity(emissiveIntensity);
                if (!albedoMap.empty()) { mat->setAlbedoMap(albedoMap); }
                if (!normalMap.empty()) { mat->setNormalMap(normalMap); }
                if (!emissiveMap.empty()) { mat->setEmissiveMap(emissiveMap); }

                auto geom = Geometry::box(1.0f, 1.0f, 1.0f); // default geometry
                auto mesh = std::make_shared<Mesh>(geom, mat);
                mesh->setName(name);
                mesh->transform().setPosition(position);
                mesh->transform().setRotation(rotation);
                mesh->transform().setScale(scale);
                scene.add(mesh);
            }
            else if (type == "DirectionalLight") {
                auto light = std::make_shared<DirectionalLight>(color, intensity);
                light->setName(name);
                light->setDirection(direction);
                light->transform().setPosition(position);
                light->transform().setRotation(rotation);
                light->transform().setScale(scale);
                scene.add(light);
            }
            else if (type == "PointLight") {
                auto light = std::make_shared<PointLight>(color, intensity);
                light->setName(name);
                light->setRange(range);
                light->transform().setPosition(position);
                light->transform().setRotation(rotation);
                light->transform().setScale(scale);
                scene.add(light);
            }
            else if (type == "AmbientLight") {
                auto light = std::make_shared<AmbientLight>(color, intensity);
                light->setName(name);
                light->transform().setPosition(position);
                light->transform().setRotation(rotation);
                light->transform().setScale(scale);
                scene.add(light);
            }
        }

    } // anonymous namespace

    // ── SceneSerializer ────────────────────────────────────────────

    bool SceneSerializer::save(const Scene3D& scene, const std::filesystem::path& path) {
        JsonWriter w;
        w.beginObject();
        w.key("version"); w.value("0.9"); w.comma();
        w.key("objects"); w.beginArray();

        const auto& objects = scene.objects();
        for (size_t i = 0; i < objects.size(); ++i) {
            if (i > 0) { w.comma(); }
            serializeObject(w, *objects[i]);
        }

        w.endArray();
        w.endObject();

        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            NOX_LOG_ERROR("Failed to open file for writing: {}", path.string());
            return false;
        }

        file << w.str() << '\n';
        NOX_LOG_INFO("Scene saved to {}", path.string());
        return true;
    }

    bool SceneSerializer::load(Scene3D& scene, const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::in);
        if (!file.is_open()) {
            NOX_LOG_ERROR("Failed to open scene file: {}", path.string());
            return false;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        std::string content = ss.str();

        JsonReader reader(content);

        reader.expect(JsonToken::ObjectStart);

        while (reader.peek().type != JsonToken::ObjectEnd) {
            std::string key = reader.expectString();
            reader.expect(JsonToken::Colon);

            if (key == "objects") {
                reader.expect(JsonToken::ArrayStart);
                while (reader.peek().type != JsonToken::ArrayEnd) {
                    deserializeObject(reader, scene);
                    if (reader.peek().type == JsonToken::Comma) { reader.next(); }
                }
                reader.expect(JsonToken::ArrayEnd);
            }
            else {
                reader.skipValue();
            }

            if (reader.peek().type == JsonToken::Comma) { reader.next(); }
        }

        reader.expect(JsonToken::ObjectEnd);

        NOX_LOG_INFO("Scene loaded from {} ({} objects)", path.string(), scene.objects().size());
        return true;
    }

} // namespace Nox
