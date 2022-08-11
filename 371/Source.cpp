#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_s.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"

using namespace glm;

//Based off of learnopengl.org
//Adapted for various COMP-371 assignments and quizzes by Ben Marcotte

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
unsigned int felt;
unsigned int snow;
unsigned int carrot;
bool lightSwitch = true;
int renderMode = GL_TRIANGLES;

bool fpsOn = true;
bool frontCam = false;
bool backCam = false;
bool rotatingCam = false;
unsigned int indices[] = {
	0, 1, 3,
	1, 2, 3
};
unsigned int meshVAO;
unsigned int sphereVAO;
unsigned int VAO;
unsigned int EBO;
GLuint sphereVBO;
GLuint sphereIBO;



glm::vec3 rotatingLight = glm::vec3(0.0f, 3.0f, 10.0f);

glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
glm::mat4 benModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -2.0f, 0.0f));
glm::mat4 binoModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
glm::mat4 vobsModel = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -2.0f, -1.0f));
glm::mat4 chloeModel = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, 2.0f));
glm::vec3 baseModelPos = glm::vec3(1.0f);

int activeModel = 1;

Camera camera(glm::vec3(0.0f, 1.0f, 8.0f));
Camera front(glm::vec3(0.0f, 1.0f, -10.0f));
Camera back(glm::vec3(0.0f, 1.0f, 15.0f));
Camera rotating(glm::vec3(0.0f, 3.0f, 10.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float prevtime = (float)glfwGetTime();
float dt =0;
glm::vec3 lightPos(0.0f, 30.0f, 0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//input processor, for organization
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		std::cout << "Process terminated by ESC" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && activeModel == 1)
		benModel = glm::translate(benModel, glm::vec3(0.0f, 0.0f, 1.0f*dt));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && activeModel == 1)
		benModel = glm::translate(benModel, glm::vec3(0.0f , 0.0f, -1.0f * dt));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && activeModel == 1)
		benModel = glm::translate(benModel, glm::vec3(1.0f * dt, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && activeModel == 1)
		benModel = glm::translate(benModel, glm::vec3(-1.0f * dt, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && activeModel == 1)
		benModel = glm::rotate(benModel, 0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && activeModel == 1)
		benModel = glm::rotate(benModel, -0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && activeModel == 2)
		binoModel = glm::translate(binoModel, glm::vec3(0.0f, 0.0f, 1.0f * dt));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && activeModel == 2)
		binoModel = glm::translate(binoModel, glm::vec3(0.0f, 0.0f, -1.0f * dt));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && activeModel == 2)
		binoModel = glm::translate(binoModel, glm::vec3(1.0f * dt, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && activeModel == 2)
		binoModel = glm::translate(binoModel, glm::vec3(-1.0f * dt, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && activeModel == 2)
		binoModel = glm::rotate(binoModel, 0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && activeModel == 2)
		binoModel = glm::rotate(binoModel, -0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && activeModel == 3)
		vobsModel = glm::translate(vobsModel, glm::vec3(0.0f, 0.0f, 1.0f * dt));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && activeModel == 3)
		vobsModel = glm::translate(vobsModel, glm::vec3(0.0f, 0.0f, -1.0f * dt));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && activeModel == 3)
		vobsModel = glm::translate(vobsModel, glm::vec3(1.0f * dt, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && activeModel == 3)
		vobsModel = glm::translate(vobsModel, glm::vec3(-1.0f * dt, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && activeModel == 3)
		vobsModel = glm::rotate(vobsModel, 0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && activeModel == 3)
		vobsModel = glm::rotate(vobsModel, -0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && activeModel == 4)
		chloeModel = glm::translate(chloeModel, glm::vec3(0.0f, 0.0f, 1.0f * dt));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && activeModel == 4)
		chloeModel = glm::translate(chloeModel, glm::vec3(0.0f, 0.0f, -1.0f * dt));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && activeModel == 4)
		chloeModel = glm::translate(chloeModel, glm::vec3(1.0f * dt, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && activeModel == 4)
		chloeModel = glm::translate(chloeModel, glm::vec3(-1.0f * dt, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && activeModel == 4)
		chloeModel = glm::rotate(chloeModel, 0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && activeModel == 4)
		chloeModel = glm::rotate(chloeModel, -0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
}

void keyCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_4 && action != GLFW_RELEASE)
		activeModel = 4;
	if (key == GLFW_KEY_3 && action != GLFW_RELEASE)
		activeModel = 3;
	if (key == GLFW_KEY_2 && action != GLFW_RELEASE)
		activeModel = 2;
	if (key == GLFW_KEY_1 && action != GLFW_RELEASE)
		activeModel = 1;
	
	if (key == GLFW_KEY_B && action != GLFW_RELEASE)
		lightSwitch != lightSwitch;

	if (key == GLFW_KEY_P && action != GLFW_RELEASE)
		renderMode = GL_POINTS;

	if (key == GLFW_KEY_T && action != GLFW_RELEASE)
		renderMode = GL_TRIANGLES;

	if (key == GLFW_KEY_L && action != GLFW_RELEASE)
		renderMode = GL_LINES;

	if (key == GLFW_KEY_U && action != GLFW_RELEASE)
	{
		if (activeModel == 1)
			benModel = glm::scale(benModel, glm::vec3(1.1f, 1.1f, 1.1f));
		if (activeModel == 2)
			binoModel = glm::scale(binoModel, glm::vec3(1.1f, 1.1f, 1.1f));
		if (activeModel == 3)
			vobsModel = glm::scale(vobsModel, glm::vec3(1.1f, 1.1f, 1.1f));
		if (activeModel == 4)
			chloeModel = glm::scale(chloeModel, glm::vec3(1.1f, 1.1f, 1.1f));

	}
	if (key == GLFW_KEY_J && action != GLFW_RELEASE)
	{
		if (activeModel == 1)
			benModel = glm::scale(benModel, glm::vec3(0.9f, 0.9f, 0.9f));
		if (activeModel == 2)
			binoModel = glm::scale(binoModel, glm::vec3(0.9f, 0.9f, 0.9f));
		if (activeModel == 3)
			vobsModel = glm::scale(vobsModel, glm::vec3(0.9f, 0.9f, 0.9f));
		if (activeModel == 4)
			chloeModel = glm::scale(chloeModel, glm::vec3(0.9f, 0.9f, 0.9f));
	}
	if (key == GLFW_KEY_SPACE && action != GLFW_RELEASE)
	{
		benModel = glm::translate(glm::mat4(1.0f), vec3((float)((rand() % 100) - 50), -2.0f, (float)((rand() % 100) - 50)));
		vobsModel = glm::translate(glm::mat4(1.0f), vec3((float)((rand() % 100) - 50), -2.0f, (float)((rand() % 100) - 50)));
		binoModel = glm::translate(glm::mat4(1.0f), vec3((float)((rand() % 100) - 50), -2.0f, (float)((rand() % 100) - 50)));
		chloeModel = glm::translate(glm::mat4(1.0f), vec3((float)((rand() % 100) - 50), -2.0f, (float)((rand() % 100) - 50)));
	}
	if (key == GLFW_KEY_R && action != GLFW_RELEASE)
	{
		benModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -2.0f, 0.0f));
		binoModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
		vobsModel = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -2.0f, -1.0f));
		chloeModel = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, 2.0f));
		frontCam = false;
		backCam = false;
		rotatingCam = false;
		fpsOn = true;
		lightSwitch = true;
		//camera = Camera(glm::vec3(0.0f, 1.0f, 8.0f));
		//rotating = Camera(glm::vec3(0.0f, 3.0f, 10.0f));
	}
}

void renderBen(glm::mat4 baseBpos, Shader shader) 
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snow);

	glm::mat4 leftLeg_olaf = baseBpos;
	leftLeg_olaf = glm::translate(leftLeg_olaf, glm::vec3(-0.5f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setMat4("model", leftLeg_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightLeg_olaf = baseBpos;
	rightLeg_olaf = glm::translate(rightLeg_olaf, glm::vec3(0.5f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setMat4("model", rightLeg_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 body_olaf = baseBpos;
	body_olaf = glm::translate(body_olaf, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(2.0f, 1.5f, 2.0f));
	shader.setMat4("model", body_olaf);
	glDrawArrays(renderMode, 0, 36);

	
	glm::mat4 leftArm_olaf = baseBpos;
	leftArm_olaf = glm::translate(leftArm_olaf, glm::vec3(-1.5f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5f, 0.50f, 0.5f));
	shader.setMat4("model", leftArm_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightArm_olaf = baseBpos;
	rightArm_olaf = glm::translate(rightArm_olaf, glm::vec3(1.5f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5f, 0.50f, 0.5f));
	shader.setMat4("model", rightArm_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 head_olaf = baseBpos;
	head_olaf = glm::translate(head_olaf, glm::vec3(0.0f, 2.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.50f, 1.0f));
	shader.setMat4("model", head_olaf);
	glDrawArrays(renderMode, 0, 36);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carrot);

	glm::mat4 nose_olaf = baseBpos;
	nose_olaf = glm::translate(nose_olaf, glm::vec3(0.0f, 2.5f, 0.6f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.1f, 0.5f));
	shader.setMat4("model", nose_olaf);
	glDrawArrays(renderMode, 0, 36);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);

	glm::mat4 mouth_olaf = baseBpos;
	mouth_olaf = glm::translate(mouth_olaf, glm::vec3(0.0f, 2.1f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.05f, 0.1f));
	shader.setMat4("model", mouth_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 leftEye_olaf = baseBpos;
	leftEye_olaf = glm::translate(leftEye_olaf, glm::vec3(-0.25f, 2.95f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.1f));
	shader.setMat4("model", leftEye_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightEye_olaf = baseBpos;
	rightEye_olaf = glm::translate(rightEye_olaf, glm::vec3(0.25f, 2.95f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.1f));
	shader.setMat4("model", rightEye_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 topButton_olaf = baseBpos;
	topButton_olaf = glm::translate(topButton_olaf, glm::vec3(0.0f, 1.55f, 1.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.2f));
	shader.setMat4("model", topButton_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 midButton_olaf = baseBpos;
	midButton_olaf = glm::translate(midButton_olaf, glm::vec3(0.0f, 1.15f, 1.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.2f));
	shader.setMat4("model", midButton_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 baseHat_olaf = baseBpos;
	baseHat_olaf = glm::translate(baseHat_olaf, glm::vec3(0.0f, 3.15f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.2f, 0.3f, 1.2f));
	shader.setMat4("model", baseHat_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 topHat_olaf = baseBpos;
	topHat_olaf = glm::translate(topHat_olaf, glm::vec3(0.0f, 3.35f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.8f, 1.0f, 0.8f));
	shader.setMat4("model", topHat_olaf);
	glDrawArrays(renderMode, 0, 36);
}
void renderBino(glm::mat4 baseBinoPos, Shader shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snow);
	glm::mat4 e1 = baseBinoPos;
	e1 = translate(e1, vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0), vec3(0.75f, 0.75f, 0.75f)); // base
	shader.setMat4("model", e1);
	glDrawArrays(renderMode, 0, 36);
	glm::mat4 e2 = baseBinoPos;
	e2 = translate(e2, vec3(0.0f, 0.65f, 0.0f)) * scale(mat4(1.0), vec3(0.6f, 0.6f, 0.6f)); // chest
	glm::mat4 e3 = baseBinoPos;
	shader.setMat4("model", e2);
	glDrawArrays(renderMode, 0, 36);
	e3 = translate(e3, vec3(0.0f, 1.1f, 0.0f)) * scale(mat4(1.0), vec3(0.45f, 0.45f, 0.45f)); // head
	shader.setMat4("model", e3);
	glDrawArrays(renderMode, 0, 36);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carrot);

	glm::mat4 e4 = baseBinoPos;
	e4 = translate(e4, vec3(0.0f, 1.15f, 0.3f)) * scale(mat4(1.0), vec3(0.1f, 0.10f, 0.60f)); // nose
	shader.setMat4("model", e4);
	glDrawArrays(renderMode, 0, 36);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);

	glm::mat4 e5 = baseBinoPos;
	glm::mat4 e6 = baseBinoPos;
	glm::mat4 e7 = baseBinoPos;
	glm::mat4 e8 = baseBinoPos;
	glm::mat4 e9 = baseBinoPos;
	glm::mat4 e10 = baseBinoPos;
	e5 = translate(e5, vec3(0.0f, 1.4f, 0.0f)) * scale(mat4(1.0), vec3(1.4f, 0.15f, 0.40f));  // base hat
	e6 = translate(e6, vec3(0.0f, 1.6f, 0.0f)) * scale(mat4(1.0), vec3(0.4f, 0.45f, 0.30f)); // top hat
	e7 = translate(e7, vec3(-0.4f, 0.8f, 0.4f)) * scale(mat4(1.0), vec3(0.15f, 0.1f, 0.9f)); // right arm
	e8 = translate(e8, vec3(0.4f, 0.8f, 0.4f)) * scale(mat4(1.0), vec3(0.15f, 0.1f, 0.9f)); // left arm
	e9 = translate(e9, vec3(-0.2f, -0.6f, 0.0f)) * scale(mat4(1.0), vec3(0.25f, 0.7f, 0.2f)); // right leg
	e10 = translate(e10, vec3(0.2f, -0.6f, 0.0f)) * scale(mat4(1.0), vec3(0.25f, 0.7f, 0.2f)); // left leg

	shader.setMat4("model", e5);
	glDrawArrays(renderMode, 0, 36);
	shader.setMat4("model", e6);
	glDrawArrays(renderMode, 0, 36);
	shader.setMat4("model", e7);
	glDrawArrays(renderMode, 0, 36);
	shader.setMat4("model", e8);
	glDrawArrays(renderMode, 0, 36);
	shader.setMat4("model", e9);
	glDrawArrays(renderMode, 0, 36);
	shader.setMat4("model", e10);
	glDrawArrays(renderMode, 0, 36);
}
void renderVobs(glm::mat4 baseVobspos, Shader shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snow);

	glm::mat4 leftLeg_olaf = baseVobspos;
	leftLeg_olaf = glm::translate(leftLeg_olaf, glm::vec3(-0.5f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5f, 1.0f, 0.5f));
	shader.setMat4("model", leftLeg_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightLeg_olaf = baseVobspos;
	rightLeg_olaf = glm::translate(rightLeg_olaf, glm::vec3(0.5f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5f, 1.0f, 0.5f));
	shader.setMat4("model", rightLeg_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 body_olaf = baseVobspos;
	body_olaf = glm::translate(body_olaf, glm::vec3(0.0f, 1.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(2.5f, 2.0f, 1.5f));
	shader.setMat4("model", body_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 torso_olaf = baseVobspos;
	torso_olaf = glm::translate(torso_olaf, glm::vec3(0.0f, 2.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.8f, 0.8f, 1.3f));
	shader.setMat4("model", torso_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 leftArm_olaf = baseVobspos;
	leftArm_olaf = glm::translate(leftArm_olaf, glm::vec3(-1.5f, 2.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5f, 0.20f, 0.5f));
	shader.setMat4("model", leftArm_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightArm_olaf = baseVobspos;
	rightArm_olaf = glm::translate(rightArm_olaf, glm::vec3(1.5f, 2.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5f, 0.20f, 0.5f));
	shader.setMat4("model", rightArm_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 head_olaf = baseVobspos;
	head_olaf = glm::translate(head_olaf, glm::vec3(0.0f, 3.15f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setMat4("model", head_olaf);
	glDrawArrays(renderMode, 0, 36);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carrot);

	glm::mat4 nose_olaf = baseVobspos;
	nose_olaf = glm::translate(nose_olaf, glm::vec3(0.0f, 3.35f, 0.6f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.2f, 0.2f, 0.7f));
	shader.setMat4("model", nose_olaf);
	glDrawArrays(renderMode, 0, 36);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);

	glm::mat4 mouth_olaf = baseVobspos;
	mouth_olaf = glm::translate(mouth_olaf, glm::vec3(0.0f, 3.05f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.70f, 0.35f, 0.1f));
	shader.setMat4("model", mouth_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 leftEye_olaf = baseVobspos;
	leftEye_olaf = glm::translate(leftEye_olaf, glm::vec3(-0.25f, 3.45f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setMat4("model", leftEye_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightEye_olaf = baseVobspos;
	rightEye_olaf = glm::translate(rightEye_olaf, glm::vec3(0.25f, 3.45f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setMat4("model", rightEye_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 topButton_olaf = baseVobspos;
	topButton_olaf = glm::translate(topButton_olaf, glm::vec3(0.0f, 2.05f, 1.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.2f));
	shader.setMat4("model", topButton_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 midButton_olaf = baseVobspos;
	midButton_olaf = glm::translate(midButton_olaf, glm::vec3(0.0f, 1.65f, 1.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.2f));
	shader.setMat4("model", midButton_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 botButton_olaf = baseVobspos;
	botButton_olaf = glm::translate(botButton_olaf, glm::vec3(0.0f, 1.25f, 1.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.2f));
	shader.setMat4("model", botButton_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 baseHat_olaf = baseVobspos;
	baseHat_olaf = glm::translate(baseHat_olaf, glm::vec3(0.0f, 3.65f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.7f, 0.3f, 1.7f));
	shader.setMat4("model", baseHat_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 topHat_olaf = baseVobspos;
	topHat_olaf = glm::translate(topHat_olaf, glm::vec3(0.0f, 3.85f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.8f, 2.0f, 0.8f));
	shader.setMat4("model", topHat_olaf);
	glDrawArrays(renderMode, 0, 36);
}
void renderChloe(glm::mat4 chloe_olaf, Shader shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snow);

	glm::mat4 leftLeg_olaf = chloe_olaf;
	leftLeg_olaf = glm::translate(leftLeg_olaf, glm::vec3(-0.5f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setMat4("model", leftLeg_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightLeg_olaf = chloe_olaf;
	rightLeg_olaf = glm::translate(rightLeg_olaf, glm::vec3(0.5f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setMat4("model", rightLeg_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 body_olaf = chloe_olaf;
	body_olaf = glm::translate(body_olaf, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(2.0f, 1.5f, 1.0f));
	shader.setMat4("model", body_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 torso_olaf = chloe_olaf;
	torso_olaf = glm::translate(torso_olaf, glm::vec3(0.0f, 2.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5f, 0.5f, 1.0f));
	shader.setMat4("model", torso_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 leftArm_olaf = chloe_olaf;
	leftArm_olaf = glm::translate(leftArm_olaf, glm::vec3(-1.5f, 2.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5f, 0.20f, 0.5f));
	shader.setMat4("model", leftArm_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightArm_olaf = chloe_olaf;
	rightArm_olaf = glm::translate(rightArm_olaf, glm::vec3(1.5f, 2.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5f, 0.20f, 0.5f));
	shader.setMat4("model", rightArm_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 head_olaf = chloe_olaf;
	head_olaf = glm::translate(head_olaf, glm::vec3(0.0f, 2.65f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setMat4("model", head_olaf);
	glDrawArrays(renderMode, 0, 36);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carrot);

	glm::mat4 nose_olaf = chloe_olaf;
	nose_olaf = glm::translate(nose_olaf, glm::vec3(0.0f, 2.75f, 0.6f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.1f, 0.5f));
	shader.setMat4("model", nose_olaf);
	glDrawArrays(renderMode, 0, 36);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);

	glm::mat4 mouth_olaf = chloe_olaf;
	mouth_olaf = glm::translate(mouth_olaf, glm::vec3(0.0f, 2.55f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.05f, 0.1f));
	shader.setMat4("model", mouth_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 leftEye_olaf = chloe_olaf;
	leftEye_olaf = glm::translate(leftEye_olaf, glm::vec3(-0.25f, 2.95f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.1f));
	shader.setMat4("model", leftEye_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 rightEye_olaf = chloe_olaf;
	rightEye_olaf = glm::translate(rightEye_olaf, glm::vec3(0.25f, 2.95f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.1f));
	shader.setMat4("model", rightEye_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 topButton_olaf = chloe_olaf;
	topButton_olaf = glm::translate(topButton_olaf, glm::vec3(0.0f, 1.55f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.2f));
	shader.setMat4("model", topButton_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 midButton_olaf = chloe_olaf;
	midButton_olaf = glm::translate(midButton_olaf, glm::vec3(0.0f, 1.15f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.2f));
	shader.setMat4("model", midButton_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 botButton_olaf = chloe_olaf;
	botButton_olaf = glm::translate(botButton_olaf, glm::vec3(0.0f, 0.75f, 0.5f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.20f, 0.2f, 0.2f));
	shader.setMat4("model", botButton_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 baseHat_olaf = chloe_olaf;
	baseHat_olaf = glm::translate(baseHat_olaf, glm::vec3(0.0f, 3.15f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.2f, 0.1f, 1.2f));
	shader.setMat4("model", baseHat_olaf);
	glDrawArrays(renderMode, 0, 36);

	glm::mat4 topHat_olaf = chloe_olaf;
	topHat_olaf = glm::translate(topHat_olaf, glm::vec3(0.0f, 3.35f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.8f, 0.7f, 0.8f));
	shader.setMat4("model", topHat_olaf);
	glDrawArrays(renderMode, 0, 36);

	
}

void renderScene(Shader squareShader, glm::mat4 benModel, glm::mat4 binoModel, glm::mat4 vobsModel, glm::mat4 chloeModel)
{
	squareShader.use();
	squareShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
	squareShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
	squareShader.setVec3("light.specular", 0.3f, 0.3f, 0.3f);
	squareShader.setFloat("light.shininess", 64.0f);

	//floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snow);
	glm::mat4 model = glm::mat4(1.0f);
	squareShader.setMat4("model", model);
	glBindVertexArray(meshVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(VAO);

	renderBen(benModel, squareShader);
	renderChloe(chloeModel, squareShader);
	renderBino(binoModel, squareShader);
	renderVobs(vobsModel, squareShader);


}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if(fpsOn)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fpsOn)
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

//vertex shader

//fragment shader


int main() {
	//initializing glfw and telling it what version/profile we are running
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	srand(time(NULL));
	//creating window object
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Quiz 2", NULL, NULL);
	if (window == NULL) 
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//passing GLAD os-specific function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	Shader ourShader("vertexShader.glsl", "fragmentShader.glsl");
    Shader shadowShader("shadowVertexShader.glsl", "shadowFragmentShader.glsl");
    Shader sphereShader("sphereVertexShader.glsl", "sphereFragmentShader.glsl");
	
	


	//floormesh
	float meshVertices[] = {
		// positions              // normals           // texcoords
		 100.0f, -2.5f,  100.0f,  0.0f,  1.0f, 0.0f,   100.0f, 0.0f,
		-100.0f, -2.5f,  100.0f,  0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
		-100.0f, -2.5f, -100.0f,  0.0f,  1.0f, 0.0f,   0.0f, 100.0f,
		 
		 100.0f, -2.5f,  100.0f,  0.0f,  1.0f, 0.0f,   100.0f, 0.0f, 
		-100.0f, -2.5f, -100.0f,  0.0f,  1.0f, 0.0f,   0.0f, 100.0f, 
		 100.0f, -2.5f, -100.0f,  0.0f,  1.0f, 0.0f,   100.0f, 100.0f, 
	};

	unsigned int meshVBO;
	glGenVertexArrays(1, &meshVAO);
	glGenBuffers(1, &meshVBO);
	glBindVertexArray(meshVAO);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(meshVertices), meshVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//loading texture
	glGenTextures(1, &felt);
	glBindTexture(GL_TEXTURE_2D, felt);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("felt.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glGenTextures(1, &snow);
	glBindTexture(GL_TEXTURE_2D, snow);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	data = stbi_load("snow.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &carrot);
	glBindTexture(GL_TEXTURE_2D, carrot);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("carrot.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	float verticesSquare[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, 
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f  	};



	

	unsigned int VBO;

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), verticesSquare, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// configure depth map FBO
// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 1024, 768);
	//adjusting window resolution with resize
	ourShader.use();
	ourShader.setInt("diffuseTexture", 0);
	ourShader.setInt("shadowMap", 1);
	ourShader.setInt("shadowMap2", 2);


	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	glEnable(GL_DEPTH_TEST);
	//render loop

	glfwSetKeyCallback(window, keyCallbacks);
	while (!glfwWindowShouldClose(window))
	{
		

		ourShader.use();
		float currtime = glfwGetTime();
		dt = currtime - prevtime;
		prevtime = currtime;
		
		//inputs
		processInput(window);
		
		prevtime = (float)glfwGetTime();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		rotating.Position = glm::vec3(glm::cos(glfwGetTime())*15.0f, 3.0f, glm::sin(glfwGetTime())*15.0f);
		rotating.Front = glm::normalize(glm::vec3(glm::cos(glfwGetTime()) * -15.0f, 0.0f, glm::sin(glfwGetTime()) * -15.0f));
		rotating.Up = glm::vec3(0.0f,1.0f,0.0f);


		// first pass shadow

		glCullFace(GL_FRONT);
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 30.0f, far_plane = 100.0f;
		//lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, far_plane);
		lightProjection = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 180.0f );
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f, -1.0f,1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;
		shadowShader.use();
		shadowShader.setBool("firstPass", true);
		shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, crate);
		renderScene(shadowShader, benModel, binoModel, vobsModel, chloeModel);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);

		// reset viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();
		int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		if (fpsOn)
		{
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			ourShader.setMat4("projection", projection);
			glm::mat4 view = camera.GetViewMatrix();
			ourShader.setMat4("view", view);
		}
		else if (backCam) 
		{
			glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			ourShader.setMat4("projection", projection);
			glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 1.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f));
			ourShader.setMat4("view", view);
		}
		else if (frontCam)
		{
			glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			ourShader.setMat4("projection", projection);
			glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 1.0f, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			ourShader.setMat4("view", view);
		}
		else if (rotatingCam)
		{
			glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			ourShader.setMat4("projection", projection);
			glm::mat4 view = rotating.GetViewMatrix();
			ourShader.setMat4("view", view);
		}
		ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		// be sure to activate shader when setting uniforms/drawing objects
		ourShader.setVec3("light.position", lightPos);
		ourShader.setVec3("light.direction", glm::vec3(0.0f, 0.0f, -1.0f));
		ourShader.setFloat("light.cutOff", glm::cos(glm::radians(lightSwitch ? 20.0f : 0.0f)));

		// be sure to activate shader when setting uniforms/drawing objects
		ourShader.setVec3("viewPos", camera.Position);
		glm::vec3 temp = glm::normalize(glm::vec3(-rotating.Position.x, rotating.Position.y - 5.0f, -rotating.Position.z));

		ourShader.setFloat("light.constant", 1.0f);
		ourShader.setFloat("light.linear", 0.014f);
		ourShader.setFloat("light.quadratic", 0.0007f);

		
		glm::vec3 lightColor = glm::vec3(1.0f);
		

		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);


		//rendering commands begin

		float timeValue = glfwGetTime();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, felt);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glBindVertexArray(VAO);
		view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		renderScene(ourShader, benModel, binoModel, vobsModel, chloeModel);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, carrot);
		ourShader.setVec3("light.specular", 0.0f, 0.0f, 0.0f);
		ourShader.setVec3("light2.specular", 0.0f, 0.0f, 0.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		// check and call events, swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}