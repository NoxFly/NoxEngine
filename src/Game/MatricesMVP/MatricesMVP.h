#ifndef MATRICESMVP_H
#define MATRICESMVP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>

class MatricesMVP {
	public:
		MatricesMVP(double fovy, double aspect, double near, double far);
		~MatricesMVP();

        void updateMVP();
        void push();
        void pop();
        void translate(int x, int y, int z);
        void translate(float x, float y, float z);
        void translate(glm::vec3 translation);

        glm::mat4& getProjection();
        glm::mat4& getModel();
        glm::mat4& getView();
        glm::mat4& getMVP();
        const glm::mat4& getMVP() const;

    private:
        glm::mat4 m_projection, m_model, m_view;
        glm::mat4 m_MVP;
        std::vector<glm::mat4> m_saves;
};

#endif // MATRICESMVP_H