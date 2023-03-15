#ifndef ENGINE_TYPEDEF_HPP
#define ENGINE_TYPEDEF_HPP

#include <type_traits>
#include <glm/glm.hpp>
#include <concepts>
#include <string>


namespace NoxEngine {

    const float PI = static_cast<float>(3.14159265358979323846);

    typedef glm::vec2 V2D;
    typedef glm::vec3 V3D;
    typedef glm::mat3 M3;
    typedef glm::mat4 M4;


    template <typename D>
    inline constexpr bool Is2D = std::is_same_v<D, V2D>;

    template <typename D>
    inline constexpr bool Is3D = std::is_same_v<D, V3D>;


    template <typename D>
    concept Dimension = Is2D<D> || Is3D<D>;

    template <typename D>
    concept Dimension2 = Is2D<D>;

    template <typename D>
    concept Dimension3 = Is3D<D>;

}

#endif // ENGINE_TYPEDEF_HPP