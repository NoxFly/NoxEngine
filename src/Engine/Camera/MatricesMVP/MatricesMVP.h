#ifndef MATRICES_MVP_H
#define MATRICES_MVP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>


class MatricesMVP {
    public:
        MatricesMVP(double fov, double aspect, double near, double far, const glm::vec3& position, const glm::vec3& verticalAxis);
        ~MatricesMVP();

        void updateMVP();
        void push();
        void pop();
        void translate(int x, int y, int z);
        void translate(float x, float y, float z);
        void translate(const glm::vec3& translation);
        void rotate(int x, int y, int z);
        void rotate(float x, float y, float z);
        void rotate(const glm::vec3& rotation);

        glm::mat4& getProjection();
        glm::mat4& getModel();
        glm::mat4& getView();
        glm::mat4& getMVP();
        const glm::mat4& getMVP() const;

        void setView(const glm::mat4& lookAt);

    private:
        glm::mat4 m_projection, m_model, m_view;
        glm::mat4 m_MVP;
        std::vector<glm::mat4> m_saves;
        bool operationDone; // in getMVP, updates MVP is any operation has been done before
};

#endif // MATRICES_MVP_H