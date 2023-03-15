namespace NoxEngine {

    template <Dimension D>
	void Renderer::render(Scene<D>* scene, Camera<D>* camera) {
        Uint32 frameRate = 1000 / m_FPS;
        m_earlyLoop = SDL_GetTicks();

        camera->update();


        clear(m_clearColor);

        // render the scene through camera

        const std::vector<std::shared_ptr<Actor<D>>> objects = scene->getActors();

        for (std::shared_ptr<Actor<D>> o : objects) {
            o->render(camera->getMatrices());
        }


        swapWindow();


        m_endLoop = SDL_GetTicks();
        m_spentTime = m_endLoop - m_earlyLoop;

        if (m_FPS == -1 && m_spentTime < frameRate)
            SDL_Delay(frameRate - m_spentTime);
	}

}