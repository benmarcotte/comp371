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
unsigned int skybox;
bool lightSwitch = true;

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
glm::mat4 benModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
glm::mat4 binoModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
glm::mat4 vobsModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
glm::mat4 chloeModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
glm::vec3 baseModelPos = glm::vec3(1.0f);

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
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && fpsOn)
		camera.ProcessKeyboard(FORWARD, dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && fpsOn)
		camera.ProcessKeyboard(BACKWARD, dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && fpsOn)
		camera.ProcessKeyboard(LEFT, dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && fpsOn)
		camera.ProcessKeyboard(RIGHT, dt);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		benModel = glm::rotate(baseModel, 0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		benModel = glm::rotate(baseModel, -0.8f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
}

void keyCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_L && action != GLFW_RELEASE)
		lightSwitch = !lightSwitch;
	if (key == GLFW_KEY_M && action != GLFW_RELEASE)
	{
		if (frontCam)
		{
			frontCam = false;
			fpsOn = !fpsOn;
		}
		else 
		{
			if (backCam)
			{
				backCam = false;
				frontCam = true;
			}
			else if (rotatingCam)
			{
				rotatingCam = false;
				frontCam = true;
			}
			else
			{
				fpsOn = !fpsOn;
				frontCam = true;
			}
		}
	}
		
	if (key == GLFW_KEY_B && action != GLFW_RELEASE)
		if (backCam)
		{
			backCam = false;
			fpsOn = !fpsOn;
		}
		else
		{
			if (frontCam)
			{
				frontCam = false;
				backCam = true;
			}
			else if (rotatingCam)
			{
				rotatingCam = false;
				backCam = true;
			}
			else
			{
				fpsOn = !fpsOn;
				backCam = true;
			}
		}

	if (key == GLFW_KEY_N && action != GLFW_RELEASE)
		if (rotatingCam)
		{
			rotatingCam = false;
			fpsOn = !fpsOn;
		}
		else
		{
			if (frontCam)
			{
				frontCam = false;
				rotatingCam = true;
			}
			else if (backCam)
			{
				backCam = false;
				rotatingCam = true;
			}
			else
			{
				fpsOn = !fpsOn;
				rotatingCam = true;
			}
		}

	if (key == GLFW_KEY_R && action != GLFW_RELEASE)
	{
		baseModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		frontCam = false;
		backCam = false;
		rotatingCam = false;
		fpsOn = true;
		lightSwitch = true;
		camera = Camera(glm::vec3(0.0f, 1.0f, 8.0f));
		rotating = Camera(glm::vec3(0.0f, 3.0f, 10.0f));
	}
}

void renderBen(glm::mat4 baseBpos, Shader shader) 
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);
	glm::mat4 b1 = baseBpos;
	glm::mat4 b2 = baseBpos;
	glm::mat4 b3 = baseBpos;
	glm::mat4 b4 = baseBpos;
	glm::mat4 b5 = baseBpos;
	glm::mat4 b6 = baseBpos;
	b1 = glm::translate(b1, glm::vec3(0.0f, 3.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(5.0f, 1.0f, 1.0f));
	b2 = glm::translate(b2, glm::vec3(0.0f, -3.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(5.0f, 1.0f, 1.0f));
	b3 = glm::translate(b3, glm::vec3(-2.0f, 0.0f, 0.0f)) *glm::scale(glm::mat4(1.0),glm::vec3(1.0f, 6.0f, 1.0f));
	b4 = glm::translate(b4, glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(3.0f, 1.0f, 1.0f));
	b5 = glm::translate(b5, glm::vec3(2.0f, 1.75f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 2.5f, 1.0f));
	b6 = glm::translate(b6, glm::vec3(2.0f, -1.75f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 2.5f, 1.0f));

	shader.setMat4("model", b1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", b2);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", b3);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", b4);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", b5);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", b6);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void renderBino(glm::mat4 baseEpos, Shader shader) 
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);
	glm::mat4 e1 = baseEpos;
	glm::mat4 e2 = baseEpos;
	glm::mat4 e3 = baseEpos;
	glm::mat4 e4 = baseEpos;
	glm::mat4 e5 = baseEpos;
	glm::mat4 e6 = baseEpos;
	e1 = glm::translate(e1, glm::vec3(0.5f, 3.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(4.0f, 1.0f, 1.0f));
	e2 = glm::translate(e2, glm::vec3(0.5f, -3.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(4.0f, 1.0f, 1.0f));
	e3 = glm::translate(e3, glm::vec3(-2.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 8.0f, 1.0f));
	e4 = glm::translate(e4, glm::vec3(0.5f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(4.0f, 1.0f, 1.0f));

	shader.setMat4("model", e1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", e2);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", e3);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", e4);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void renderVobs(glm::mat4 base9pos, Shader shader) 
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);
	glm::mat4 n1 = base9pos;
	glm::mat4 n2 = base9pos;
	glm::mat4 n3 = base9pos;
	glm::mat4 n4 = base9pos;
	glm::mat4 n5 = base9pos;
	n1 = glm::translate(n1, glm::vec3(0.0f, 3.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(3.0f, 1.0f, 1.0f));
	n2 = glm::translate(n2, glm::vec3(0.0f, -3.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(3.0f, 1.0f, 1.0f));
	n3 = glm::translate(n3, glm::vec3(2.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 8.0f, 1.0f));
	n4 = glm::translate(n4, glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(3.0f, 1.0f, 1.0f));
	n5 = glm::translate(n5, glm::vec3(-2.0f, 1.75f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 2.5f, 1.0f));

	shader.setMat4("model", n1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", n2);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", n3);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", n4);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", n5);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void renderChloe(glm::mat4 base7pos, Shader shader) 
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);
	glm::mat4 s1 = base7pos;
	glm::mat4 s2 = base7pos;
	glm::mat4 s3 = base7pos;
	s1 = glm::translate(s1, glm::vec3(0.0f, 3.5f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(3.0f, 1.0f, 1.0f));
	s2 = glm::translate(s2, glm::vec3(-2.0f, 2.25f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 3.5f, 1.0f));
	s3 = glm::translate(s3, glm::vec3(2.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 8.0f, 1.0f));

	shader.setMat4("model", s1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", s2);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", s3);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void renderSkateboard(glm::mat4 baseSkateboard, Shader shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);
	shader.setVec3("light.ambient", 0.25f, 0.20725f, 0.20725f);
	shader.setVec3("light.diffuse", 1.0f, 0.829f, 0.829f);
	shader.setVec3("light.specular", 0.296648f, 0.296648f, 0.296648f);
	shader.setFloat("light.shininess", 32.0f);
	//* glm::scale(baseSkateboard, glm::vec3(0.75, 0.75, 0.75))
	glm::mat4 w1 = glm::translate(baseSkateboard, glm::vec3(-7.0f, -0.5f, 0.5f)) ;
	glm::mat4 w2 = glm::translate(baseSkateboard, glm::vec3(-7.0f, -0.5f, -0.5f));
	glm::mat4 w3 = glm::translate(baseSkateboard, glm::vec3(7.0f, -0.5f, 0.5f));
	glm::mat4 w4 = glm::translate(baseSkateboard, glm::vec3(7.0f, -0.5f, -0.5f));
	w1 = glm::scale(w1, glm::vec3(0.75, 0.75, 0.75));
	w2 = glm::scale(w2, glm::vec3(0.75, 0.75, 0.75));
	w3 = glm::scale(w3, glm::vec3(0.75, 0.75, 0.75));
	w4 = glm::scale(w4, glm::vec3(0.75, 0.75, 0.75));

	shader.setMat4("model", w1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", w2);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", w3);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.setMat4("model", w4);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, felt);
	shader.setVec3("light.ambient", 0.19225f, 0.19225f, 0.19225f);
	shader.setVec3("light.diffuse", 0.50754f, 0.50754f, 0.50754f);
	shader.setVec3("light.specular", 2.0f, 2.0f, 2.0f);
	shader.setFloat("light.shininess",128.0f);
	baseSkateboard = glm::scale(baseSkateboard, glm::vec3(15.0f, 0.5f, 2.0f));
	//glBindVertexArray(VAO);

	shader.setMat4("model", baseSkateboard);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	

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

	glGenTextures(1, &skybox);
	glBindTexture(GL_TEXTURE_2D, skybox);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
		lightProjection = glm::perspective(glm::cos(glm::radians(70.0f)), 1.0f, 0.1f, 180.0f);
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
		glBindTexture(GL_TEXTURE_2D, skybox);
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