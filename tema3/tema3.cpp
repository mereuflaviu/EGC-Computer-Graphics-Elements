#include "lab_m1/tema3/tema3.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace m1;
using namespace std;

Tema3::Tema3() {}

Tema3::~Tema3() { }

void Tema3::Init() {
	TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3",
		"textures"), "ground.jpg", "ground");
	TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3",
		"textures"), "concrete.jpg", "concrete");
	TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3",
		"textures"), "moon.jpg", "moon");
	TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3",
		"textures"), "crate.jpg", "wood");
	TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3",
		"textures"), "water.jpg", "water");
	TextureManager::GetTexture("water")->SetWrappingMode(GL_REPEAT);

	Mesh* mesh;
	{
		mesh = new Mesh("sphere");
		mesh->LoadMesh(
			PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
			"sphere.obj");
		AddMeshToList(mesh);
	}
	{
		mesh = new Mesh("cube");
		mesh->LoadMesh(
			PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
			"box.obj");
		AddMeshToList(mesh);
	}
	{
		mesh = new Mesh("boat");
		mesh->LoadMesh(
			PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3",
				"meshes"),"boat.obj");
		AddMeshToList(mesh);
	}
	{
		mesh = new Mesh("quad");
		mesh->LoadMesh(
			PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
			"quad.obj");
		AddMeshToList(mesh);
	}
	{
		mesh = new Mesh("cylinder");
		mesh->LoadMesh(
			PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
			"cylinder.obj");
		AddMeshToList(mesh);
	}
	{
		Shader* shader;
		shader = new Shader("Shader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3",
			"shaders", "VertexShader.glsl"),
			GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3",
			"shaders", "FragmentShader.glsl"),
			GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}


	for (int i = 0; i < 4; i++) {
		boats.push_back(Boat());
		lights[i].color = glm::vec3((rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f);
		lights[i].intensity = 4.f;
		lights[i].type = POINT;
	}

	lights[4].type = POINT;
	lights[4].intensity = 15;

	for (int i = 5; i < 7; i++) {
		lights[i].type = SPOT;
		lights[i].intensity = 15;
	}

	lights[7].color = glm::vec3(1, 1, 1);
	lights[7].direction = normalize(-glm::vec3(-4, 2, -10));
	lights[7].intensity = 0.5f;
	lights[7].type = DIRECTIONAL;

	GetSceneCamera()->SetPosition(glm::vec3(3, 6, 20));
}

void Tema3::FrameStart() {
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 13 / 255.0, 38 / 255.0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema3::Update(float deltaTimeSeconds) {
	for (int i = 0; i < 4; i++) {
		boats[i].position = glm::vec3(boats[i].radius * glm::cos(boats[i].angle), -0.5f, boats[i].radius * glm::sin(boats[i].angle));
		boats[i].angle += boats[i].direction * boats[i].speed * deltaTimeSeconds / boats[i].radius;

		lights[i].position = boats[i].position;
		lights[i].position.y += 1.5f;
	}

	glm::vec3 color = glm::vec3(colors[0], colors[1], colors[2]);
	glm::vec3 positions[3] = { glm::vec3(0, 0.25f, 0),
								glm::vec3(cos(spotAngle), 0.25f, sin(spotAngle)),
								glm::vec3(cos(RADIANS(180) + spotAngle), 0.25f, sin(RADIANS(180) + spotAngle)) };

	for (int i = 4; i < 7; i++) {
		lights[i].color = color;
		lights[i].position = positions[i - 4];
	}

	spotAngle += deltaTimeSeconds * 2.0f;


	lights[5].direction = glm::normalize(lights[5].position);
	lights[6].direction = glm::normalize(lights[6].position);

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.5f, 0));
	modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(40, 40, 1));
	RenderSimpleMeshWater(meshes["quad"], shaders["Shader"], modelMatrix, TextureManager::GetTexture("water"));

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-12, 7, -10));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3));
	RenderSimpleMeshMoon(meshes["sphere"], shaders["Shader"], modelMatrix, TextureManager::GetTexture("moon"), glm::vec3(1, 1, 1));

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.5f, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(6, 1.5f, 6));
	RenderSimpleMeshNormal(meshes["sphere"], shaders["Shader"], modelMatrix, TextureManager::GetTexture("ground"));

	RenderLight();
	RenderBoats();
	RenderUI();
}


void Tema3::RenderBoats() {
	glm::mat4 modelMatrix;

	for (auto& boat : boats) {
		glm::vec3 direction = boat.position - boat.previous_position;
		boat.previous_position = boat.position;
		float angle = atan2(direction.x, direction.z);
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, boat.position);
		modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f));
		RenderSimpleMeshNormal(meshes["boat"], shaders["Shader"], modelMatrix, TextureManager::GetTexture("wood"));
	}
}

void Tema3::RenderUI() {
	for (int i = 0; i < 3; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.0f, -0.5f - 0.1 * i, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.05f, 1));
		RenderSimpleMeshUI(meshes["quad"], shaders["Shader"], modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.0f, -0.5f - 0.1 * i, -1.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f * colors[i], 0.05f, 1));
		RenderSimpleMeshUI(meshes["quad"], shaders["Shader"], modelMatrix, sliderColors[i]);
	}
}

void Tema3::RenderLight() {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 5, 2));
	RenderSimpleMeshNormal(meshes["cylinder"], shaders["Shader"], modelMatrix, TextureManager::GetTexture("concrete"));

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 5, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 1, 2));
	RenderSimpleMeshEmissive(meshes["cylinder"], shaders["Shader"], modelMatrix, lights[4].color);

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 6, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 1, 2));
	RenderSimpleMeshNormal(meshes["cylinder"], shaders["Shader"], modelMatrix, TextureManager::GetTexture("concrete"));
}

void Tema3::RenderSimpleMeshNormal(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Send light data
	for (int i = 0; i < 8; i++) {
		std::string lightName = "lights[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(shader->program, (lightName + ".position").c_str()), 1, glm::value_ptr(lights[i].position));
		glUniform3fv(glGetUniformLocation(shader->program, (lightName + ".color").c_str()), 1, glm::value_ptr(lights[i].color));
		glUniform3fv(glGetUniformLocation(shader->program, (lightName + ".direction").c_str()), 1, glm::value_ptr(lights[i].direction));
		glUniform1f(glGetUniformLocation(shader->program, (lightName + ".intensity").c_str()), lights[i].intensity);
		glUniform1i(glGetUniformLocation(shader->program, (lightName + ".type").c_str()), lights[i].type);
	}

	int loc_eye = glGetUniformLocation(shader->program, "eye_position");
	glm::vec3 eye_pos = GetSceneCamera()->m_transform->GetWorldPosition();
	glUniform3f(loc_eye, eye_pos.x, eye_pos.y, eye_pos.z);

	int loc_type = glGetUniformLocation(shader->program, "shader_type");
	glUniform1i(loc_type, 0);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderSimpleMeshWater(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Bind light data
	for (int i = 0; i < 8; i++) {
		std::string lightName = "lights[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(shader->program, (lightName + ".position").c_str()), 1, glm::value_ptr(lights[i].position));
		glUniform3fv(glGetUniformLocation(shader->program, (lightName + ".color").c_str()), 1, glm::value_ptr(lights[i].color));
		glUniform3fv(glGetUniformLocation(shader->program, (lightName + ".direction").c_str()), 1, glm::value_ptr(lights[i].direction));
		glUniform1f(glGetUniformLocation(shader->program, (lightName + ".intensity").c_str()), lights[i].intensity);
		glUniform1i(glGetUniformLocation(shader->program, (lightName + ".type").c_str()), lights[i].type);
	}

	int loc_eye = glGetUniformLocation(shader->program, "eye_position");
	glm::vec3 eye_pos = GetSceneCamera()->m_transform->GetWorldPosition();
	glUniform3f(loc_eye, eye_pos.x, eye_pos.y, eye_pos.z);

	// Bind water data
	int loc_time = glGetUniformLocation(shader->program, "time");
	glUniform1f(loc_time, Engine::GetElapsedTime());

	int loc_type = glGetUniformLocation(shader->program, "shader_type");
	glUniform1i(loc_type, 1);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderSimpleMeshEmissive(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3& color) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Bind color
	int loc_color = glGetUniformLocation(shader->program, "color");
	glUniform3f(loc_color, color.x, color.y, color.z);

	int loc_type = glGetUniformLocation(shader->program, "shader_type");
	glUniform1i(loc_type, 2);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderSimpleMeshUI(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3& color) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Bind ui color
	int loc_color = glGetUniformLocation(shader->program, "color");
	glUniform3f(loc_color, color.x, color.y, color.z);

	int loc_type = glGetUniformLocation(shader->program, "shader_type");
	glUniform1i(loc_type, 3);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderSimpleMeshMoon(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture, glm::vec3& color) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Bind moon color
	int loc_color = glGetUniformLocation(shader->program, "color");
	glUniform3f(loc_color, color.x, color.y, color.z);

	int loc_type = glGetUniformLocation(shader->program, "shader_type");
	glUniform1i(loc_type, 4);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::FrameEnd() {}

void Tema3::OnInputUpdate(float deltaTime, int mods) {
	if (window->KeyHold(GLFW_KEY_1)) {
		colors[0] -= deltaTime * 1.f;
	}
	else if (window->KeyHold(GLFW_KEY_2)) {
		colors[0] += deltaTime * 1.f;
	}
	else if (window->KeyHold(GLFW_KEY_3)) {
		colors[1] -= deltaTime * 1.f;
	}
	else if (window->KeyHold(GLFW_KEY_4)) {
		colors[1] += deltaTime * 1.f;
	}
	else if (window->KeyHold(GLFW_KEY_5)) {
		colors[2] -= deltaTime * 1.f;
	}
	else if (window->KeyHold(GLFW_KEY_6)) {
		colors[2] += deltaTime * 1.f;
	}

	for (int i = 0; i < 3; i++) {
		if (colors[i] < 0)
			colors[i] = 0;
		else if (colors[i] > 1)
			colors[i] = 1;
	}
}

void Tema3::OnKeyPress(int key, int mods) {

}

void Tema3::OnKeyRelease(int key, int mods) {
	// Add key release event
}

void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {

}

void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}

void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}

void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Tema3::OnWindowResize(int width, int height) {}
