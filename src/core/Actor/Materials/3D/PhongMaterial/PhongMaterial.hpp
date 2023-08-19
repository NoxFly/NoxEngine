#ifndef PHONG_MATERIAL_HPP
#define PHONG_MATERIAL_HPP

#include <vector>

#include "core/MatricesMVP/Matrices.hpp"
#include "core/Actor/Materials/Material.hpp"
#include "core/Actor/Light/Light.hpp"
#include "utils/Color.hpp"


namespace NoxEngine {

    class PhongMaterial : public Material<V3D> {
        public:
            explicit PhongMaterial();
            explicit PhongMaterial(const Color& color);
            explicit PhongMaterial(const std::string& textureName);
            explicit PhongMaterial(const Color& color, const std::string& textureName);
            ~PhongMaterial();

            void transferUniforms(Matrices<V3D>& mvp, const Scene<V3D>* scene) override {
                const auto lights = scene->getLights();

                // sends the matrices
                m_shader->setMat4("MVP", mvp.get());
                m_shader->setMat4("M", mvp.getModel());
                m_shader->setMat4("V", mvp.getView());
                m_shader->setVec3("objectColor", m_color.vec3());
                m_shader->setFloat("specularPower", m_specularIntensity);

                // sends the lights
                if (lights.size() > 0) {
                    m_shader->setVec3("lightPos", lights[0]->getPosition());
                    m_shader->setVec3("lightColor", lights[0]->getColor().vec3());
                    m_shader->setFloat("lightPower", lights[0]->getIntensity());
                }
            }

            float getSpecular() const noexcept;

            void setSpecular(const float intensity) noexcept;

        protected:
            float m_specularIntensity;
    };

}

#endif // PHONG_MATERIAL_HPP