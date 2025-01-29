/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

namespace NoxEngine {

    template <Dimension D>
	void Renderer::render(Scene<D>& scene, Camera<D>& camera) {
        camera.update(m_input, m_deltaTime);

        auto earlyLoop = SDL_GetTicks();

        clear(m_clearColor);

        const std::vector<std::shared_ptr<Actor<D>>> objects = scene.getActors();

        for (std::shared_ptr<Actor<D>> o : objects) {
            o->render(&scene, &camera);
        }

        swapWindow();

        Uint64 now = SDL_GetPerformanceCounter();
        m_deltaTime = static_cast<double>(
			(now - m_previousTime) / static_cast<double>(SDL_GetPerformanceFrequency())
		);
		m_previousTime = now;
        m_totalTime += m_deltaTime;

        m_frameRate = std::round(1.0f / m_deltaTime);

        auto endLoop = SDL_GetTicks();
        auto spentTime = endLoop - earlyLoop;

        if (spentTime < m_frameRate)
            SDL_Delay(m_frameRate - spentTime);
	}

}