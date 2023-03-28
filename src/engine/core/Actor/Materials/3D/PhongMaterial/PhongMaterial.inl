namespace NoxEngine {
    
    void PhongMaterial::transferUniforms(Matrices<V3D>& mvp, const Scene<V3D>* scene) {
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

}