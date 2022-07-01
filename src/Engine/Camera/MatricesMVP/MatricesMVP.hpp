#ifndef MATRICES_MVP_HPP
#define MATRICES_MVP_HPP

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
        template <typename T, float, int>
        void translate(T x, T y, T z);
        void translate(const glm::vec3& translation);
        template <typename T, float, int>
        void rotate(T x, T y, T z);
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
        bool m_operationDone; // in getMVP, updates MVP is any operation has been done before
};

#endif // MATRICES_MVP_HPP