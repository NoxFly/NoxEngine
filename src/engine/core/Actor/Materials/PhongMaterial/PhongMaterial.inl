namespace NoxEngine {
    
    void PhongMaterial::transferUniforms(
        const M4& mvp, const M4& m, const M4& v, const M4& p,
        const std::vector<std::shared_ptr<Light>>& lights
    ) {
        // sends the matrices
        m_shader->setMat4("MVP", mvp);
        m_shader->setMat4("M", m);
        m_shader->setMat4("V", v);
        m_shader->setVec3("objectColor", m_color.vec3());
        m_shader->setFloat("specularPower", m_specularIntensity);

        // sends the lights
        if (lights.size() > 0) {
            m_shader->setVec3("lightPos", lights[0]->getPosition());
            m_shader->setVec3("lightColor", lights[0]->getColor().vec3());
            m_shader->setFloat("lightPower", lights[0]->getIntensity());
        }
    }

}