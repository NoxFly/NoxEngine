#ifndef MATRICESMVP_H
#define MATRICESMVP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

class MatricesMVP {
	public:
		MatricesMVP(double fovy, double aspect, double near, double far);
		~MatricesMVP();

        void updateMVP();

        glm::mat4& getProjection();
        glm::mat4& getModel();
        glm::mat4& getView();
        const glm::mat4& getMVP();

    private:
        glm::mat4 m_projection, m_model, m_view;
        glm::mat4 m_MVP;
};

#endif // MATRICESMVP_H