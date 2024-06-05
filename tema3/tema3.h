#pragma once

#include "components/simple_scene.h"
#include "lab_camera.h"
#include "components/transform.h"
#include "boat.h"
#include <vector>
#include "light.h"

namespace m1 {
	class Tema3 : public gfxc::SimpleScene {
	public:
		Tema3();
		~Tema3();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		void RenderLight();
		void RenderBoats();
		void RenderUI();

		Light lights[8];
		enum LightType {
			POINT,
			SPOT,
			DIRECTIONAL,
		};
		float spotAngle = 0;


		float colors[3] = { 1.f, 1.f, 1.f };
		glm::vec3 sliderColors[3] = { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),  glm::vec3(0, 0, 1) };

		std::vector<Boat> boats;

		void RenderSimpleMeshNormal(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture);
		void RenderSimpleMeshWater(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture);
		void RenderSimpleMeshEmissive(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3& color);
		void RenderSimpleMeshUI(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3& color);
		void RenderSimpleMeshMoon(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture, glm::vec3& color);
	};
}  // namespace m1
