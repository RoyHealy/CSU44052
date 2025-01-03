#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// #include <stb/stb_image.h>
// GLTF model loader
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <render/shader.h>

#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>
// #include "camera.cpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// static void convertGLBFile(std::string glbFilename, std::string filename);

// using namespace tinygltf;

// static GLFWwindow *window;
// static int windowWidth = 1920, halfWidth = windowWidth/2;
// static int windowHeight = 1024, halfHeight = windowHeight/2;
// Model model;
// TinyGLTF loader;
// std::string err;
// std::string warn;
// static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
// static void cursor_callback(GLFWwindow *window, double xpos, double ypos);
// static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
// static float sensitivity = 1.5f;
// // OpenGL camera view parameters
// static float cameraSpeed = 2.f;
// static glm::vec3 eye_center(0.0f, 0.0f, 0.0f);
// static float viewAzimuth = 0.f;
// static float viewPolar = M_PI_2;
// static glm::vec3 lookdirection(1.0f, 0.0f, 0.0f);
// static glm::vec3 up(0.0f, 1.0f, 0.0f);
// static float FoV = 45.0f;
// static float zNear = 1.0f;
// static float zFar = 1500.0f; 

// Lighting  
static glm::vec3 lightIntensity(5e6f, 5e6f, 5e6f);
static glm::vec3 lightPosition(-275.0f, 500.0f, 800.0f);

// Animation 
static bool playAnimation = true;
static float playbackSpeed = 2.0f;

struct MyAsset {
// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint jointMatricesID;
	GLuint lightPositionID;
	GLuint lightIntensityID;
	GLuint baseColorFactorID;
	GLuint textureID;
	GLuint programID;

	tinygltf::Model model;

	// Each VAO corresponds to each mesh primitive in the GLTF model
	struct PrimitiveObject {
		GLuint vao;
		std::map<int, GLuint> vbos;
	};
	std::vector<PrimitiveObject> primitiveObjects;

	// Skinning 
	struct SkinObject {
		// Transforms the geometry into the space of the respective joint
		std::vector<glm::mat4> inverseBindMatrices;  

		// Transforms the geometry following the movement of the joints
		std::vector<glm::mat4> globalJointTransforms;

		// Combined transforms
		std::vector<glm::mat4> jointMatrices;
	};
	std::vector<SkinObject> skinObjects;

	// Animation 
	struct SamplerObject {
		std::vector<float> input;
		std::vector<glm::vec4> output;
		int interpolation;
	};
	struct ChannelObject {
		int sampler;
		std::string targetPath;
		int targetNode;
	}; 
	struct AnimationObject {
		std::vector<SamplerObject> samplers;	// Animation data
	};
	std::vector<AnimationObject> animationObjects;

	glm::mat4 getNodeTransform(const tinygltf::Node& node) {
		glm::mat4 transform(1.0f); 

		if (node.matrix.size() == 16) {
			transform = glm::make_mat4(node.matrix.data());
		} else {
			if (node.translation.size() == 3) {
				transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
			}
			if (node.rotation.size() == 4) {
				glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
				transform *= glm::mat4_cast(q);
			}
			if (node.scale.size() == 3) {
				transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
			}
		}
		return transform;
	}

	void computeLocalNodeTransform(const tinygltf::Model& model, 
		int nodeIndex, 
		std::vector<glm::mat4> &localTransforms)
	{
		// ---------------------------------------
		// TODO: your code here
		tinygltf::Node localNode = model.nodes[nodeIndex];
		localTransforms[nodeIndex] = getNodeTransform(localNode);
		// printf("local %d done\n", nodeIndex);
		for (int childIndex : localNode.children) {
			computeLocalNodeTransform(model, childIndex, localTransforms);
		}
		// ---------------------------------------
	}

	void computeGlobalNodeTransform(const tinygltf::Model& model, 
		const std::vector<glm::mat4> &localTransforms,
		int nodeIndex, const glm::mat4& parentTransform, 
		std::vector<glm::mat4> &globalTransforms)
	{
		// ----------------------------------------
		// TODO: your code here
		tinygltf::Node localNode = model.nodes[nodeIndex];
		glm::mat4 globalTrans = parentTransform * localTransforms[nodeIndex];
		globalTransforms[nodeIndex] = globalTrans;
		// printf("global %d done\n", nodeIndex);
		for (int childIndex : localNode.children) {
			computeGlobalNodeTransform(model, localTransforms, childIndex, globalTrans, globalTransforms);
		}
		// ----------------------------------------
	}

	std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model) {
		std::vector<SkinObject> skinObjects;

		// In our Blender exporter, the default number of joints that may influence a vertex is set to 4, just for convenient implementation in shaders.

		for (size_t i = 0; i < model.skins.size(); i++) {
			SkinObject skinObject;

			const tinygltf::Skin &skin = model.skins[i];

			// Read inverseBindMatrices
			const tinygltf::Accessor &accessor = model.accessors[skin.inverseBindMatrices];
			assert(accessor.type == TINYGLTF_TYPE_MAT4);
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			const float *ptr = reinterpret_cast<const float *>(
            	buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);
			
			skinObject.inverseBindMatrices.resize(accessor.count);
			for (size_t j = 0; j < accessor.count; j++) {
				float m[16];
				memcpy(m, ptr + j * 16, 16 * sizeof(float));
				skinObject.inverseBindMatrices[j] = glm::make_mat4(m);
			}

			assert(skin.joints.size() == accessor.count);

			skinObject.globalJointTransforms.resize(skin.joints.size());
			skinObject.jointMatrices.resize(skin.joints.size());

			// ----------------------------------------------
			// TODO: your code here to compute joint matrices
			// ----------------------------------------------

			// Compute local transforms at each node
			int rootNodeIndex = skin.joints[0];
			std::vector<glm::mat4> localNodeTransforms(skin.joints.size());
			computeLocalNodeTransform(model, rootNodeIndex, localNodeTransforms);

			// Compute global transforms at each node
			glm::mat4 parentTransform(1.0f);
			// std::vector<glm::mat4> globalNodeTransforms(skin.joints.size());
			computeGlobalNodeTransform(model, localNodeTransforms, rootNodeIndex, parentTransform, skinObject.globalJointTransforms);
			for (int j = 0; j < skin.joints.size(); j++) {
				int globalJointIndex = skin.joints[j];
				skinObject.jointMatrices[j] = skinObject.globalJointTransforms[globalJointIndex] * skinObject.inverseBindMatrices[j];
			}
			// ----------------------------------------------

			skinObjects.push_back(skinObject);
		}
		return skinObjects;
	}

	int findKeyframeIndex(const std::vector<float>& times, float animationTime) 
	{
		int left = 0;
		int right = times.size() - 1;

		while (left <= right) {
			int mid = (left + right) / 2;

			if (mid + 1 < times.size() && times[mid] <= animationTime && animationTime < times[mid + 1]) {
				return mid;
			}
			else if (times[mid] > animationTime) {
				right = mid - 1;
			}
			else { // animationTime >= times[mid + 1]
				left = mid + 1;
			}
		}

		// Target not found
		return times.size() - 2;
	}

	std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model) 
	{
		std::vector<AnimationObject> animationObjects;
		for (const auto &anim : model.animations) {
			AnimationObject animationObject;
			
			for (const auto &sampler : anim.samplers) {
				SamplerObject samplerObject;

				const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
				const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

				assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

				// Input (time) values
				samplerObject.input.resize(inputAccessor.count);

				const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
				const float *inputBuf = reinterpret_cast<const float*>(inputPtr);

				// Read input (time) values
				int stride = inputAccessor.ByteStride(inputBufferView);
				for (size_t i = 0; i < inputAccessor.count; ++i) {
					samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
				}
				
				const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
				const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
				const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

				assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
				const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

				int outputStride = outputAccessor.ByteStride(outputBufferView);
				
				// Output values
				samplerObject.output.resize(outputAccessor.count);
				
				for (size_t i = 0; i < outputAccessor.count; ++i) {

					if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
						memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
					} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
						memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
					} else {
						std::cout << "Unsupport accessor type ..." << std::endl;
					}

				}

				animationObject.samplers.push_back(samplerObject);			
			}

			animationObjects.push_back(animationObject);
		}
		return animationObjects;
	}

	void updateAnimation(
		const tinygltf::Model &model, 
		const tinygltf::Animation &anim, 
		const AnimationObject &animationObject, 
		float time,
		std::vector<glm::mat4> &nodeTransforms) 
	{
		// There are many channels so we have to accumulate the transforms 
		for (const auto &channel : anim.channels) {
			
			int targetNodeIndex = channel.target_node;
			const auto &sampler = anim.samplers[channel.sampler];
			
			// Access output (value) data for the channel
			const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
			const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

			// Calculate current animation time (wrap if necessary)
			const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
			float animationTime = fmod(time, times.back());
			
			// ----------------------------------------------------------
			// TODO: Find a keyframe for getting animation data 
			// ----------------------------------------------------------
			int keyframeIndex = findKeyframeIndex(times, animationTime); 

			const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
			const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

			// -----------------------------------------------------------
			// TODO: Add interpolation for smooth interpolation
			// -----------------------------------------------------------
			
			float interdistance = 0; // linear interpolation
			if (keyframeIndex>=0 && keyframeIndex < times.size()-2) {
				interdistance = (animationTime - times[keyframeIndex])/(times[keyframeIndex+1]-times[keyframeIndex]);
			}
			if (channel.target_path == "translation") {
				glm::vec3 translation0, translation1;
				memcpy(&translation0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));
				memcpy(&translation1, outputPtr + (keyframeIndex+1) * 3 * sizeof(float), 3 * sizeof(float));
				glm::vec3 translation = translation0 + interdistance*(translation1-translation0); // if using quaternions, glm::lerp(translation0, translation1, interdistance)
				nodeTransforms[targetNodeIndex] = glm::translate(nodeTransforms[targetNodeIndex], translation);
			} else if (channel.target_path == "rotation") {
				glm::quat rotation0, rotation1;
				memcpy(&rotation0, outputPtr + keyframeIndex * 4 * sizeof(float), 4 * sizeof(float));
				memcpy(&rotation1, outputPtr + (keyframeIndex+1) * 4 * sizeof(float), 4 * sizeof(float));
				glm::quat rotation = glm::slerp(rotation0, rotation1, interdistance);//rotation0;  + interdistance*(rotation1-rotation0);
				nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation);
			} else if (channel.target_path == "scale") {
				glm::vec3 scale0, scale1;
				memcpy(&scale0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));
				memcpy(&scale0, outputPtr + (keyframeIndex+1) * 3 * sizeof(float), 3 * sizeof(float));
				glm::vec3 scale = scale0 + interdistance*(scale1-scale0);
				nodeTransforms[targetNodeIndex] = glm::scale(nodeTransforms[targetNodeIndex], scale);
			}
		}
	}

	void updateSkinning(const std::vector<glm::mat4> &nodeTransforms) {

		// -------------------------------------------------
		// TODO: Recompute joint matrices 
		// -------------------------------------------------
		for (int i = 0; i < model.skins.size(); i++) {
			SkinObject &skinObject = skinObjects[i];
			const tinygltf::Skin &skin = model.skins[i];
			// Compute global transforms at each node
			glm::mat4 parentTransform(1.0f);
			int rootNodeIndex = skin.joints[0];
			computeGlobalNodeTransform(model, nodeTransforms, rootNodeIndex, parentTransform, skinObject.globalJointTransforms);
			
			for (int j = 0; j < skin.joints.size(); j++) {
				int globalJointIndex = skin.joints[j];
				skinObject.jointMatrices[j] = skinObject.globalJointTransforms[globalJointIndex] * skinObject.inverseBindMatrices[j];
			}
		}
	}

	void update(float time) {

		// -------------------------------------------------
		// TODO: your code here 
		// -------------------------------------------------
		
		// maybe need for each skin
		// for (int i = 0; i < model.skins.size(); i++) {// supposedly theres only 1 skin, if there is multiple we should only render 1 of them
		if (model.animations.size() > 0) {
			const tinygltf::Animation &animation = model.animations[0];
			const AnimationObject &animationObject = animationObjects[0];

			const tinygltf::Skin &skin = model.skins[0];// was model.skin[0]
			std::vector<glm::mat4> nodeTransforms(skin.joints.size());
			for (size_t i = 0; i < nodeTransforms.size(); ++i) {
				nodeTransforms[i] = glm::mat4(1.0);
			}

			updateAnimation(model, animation, animationObject, time, nodeTransforms);
			
			updateSkinning(nodeTransforms);
		}
		// }
	}

	bool loadModel(tinygltf::Model &model, const char *filename) {
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
		if (!warn.empty()) {
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			std::cout << "ERR: " << err << std::endl;
		}

		if (!res)
			std::cout << "Failed to load glTF: " << filename << std::endl;
		else
			std::cout << "Loaded glTF: " << filename << std::endl;

		return res;
	}

	void initialize(const char* filename) {
		// Modify your path if needed
		if (!loadModel(model, filename)) {
			return;
		}
		// for (auto& buf : model.buffers) {
			
		// 	std::cout << buf.data.size() << std::endl;
		// }

		// Prepare buffers for rendering 
		primitiveObjects = bindModel(model);

		// Prepare joint matrices
		skinObjects = prepareSkinning(model);

		// Prepare animation data 
		animationObjects = prepareAnimation(model);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../final-proj/shader/assets.vert", "../final-proj/shader/assets.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}
		// Get a handle for GLSL variables
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
		// jointMatricesID = glGetUniformLocation(programID, "u_jointMat");
		// textureID = glGetUniformLocation(programID, "textureSampler");
		baseColorFactorID = glGetUniformLocation(programID, "baseColorFactor");
		lightPositionID = glGetUniformLocation(programID, "lightPosition");
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
	}

	void bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
				tinygltf::Model &model, tinygltf::Mesh &mesh) {

		std::map<int, GLuint> vbos;
		for (size_t i = 0; i < model.bufferViews.size(); ++i) {
			const tinygltf::BufferView &bufferView = model.bufferViews[i];

			int target = bufferView.target;
			
			if (bufferView.target == 0) { 
				// The bufferView with target == 0 in our model refers to 
				// the skinning weights, for 25 joints, each 4x4 matrix (16 floats), totaling to 400 floats or 1600 bytes. 
				// So it is considered safe to skip the warning.
				//std::cout << "WARN: bufferView.target is zero" << std::endl;
				continue;
			}

			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(target, vbo);
			glBufferData(target, bufferView.byteLength,
						&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
			
			vbos[i] = vbo;
		}

		// Each mesh can contain several primitives (or parts), each we need to 
		// bind to an OpenGL vertex array object
		for (size_t i = 0; i < mesh.primitives.size(); ++i) {

			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			for (auto &attrib : primitive.attributes) {
				tinygltf::Accessor accessor = model.accessors[attrib.second];
				int byteStride =
					accessor.ByteStride(model.bufferViews[accessor.bufferView]);
				glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

				int size = 1;
				if (accessor.type != TINYGLTF_TYPE_SCALAR) {
					size = accessor.type;
				}

				int vaa = -1;
				if (attrib.first.compare("POSITION") == 0) vaa = 0;
				if (attrib.first.compare("NORMAL") == 0) vaa = 1;
				if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
				if (attrib.first.compare("JOINTS_0") == 0) vaa = 3;
				if (attrib.first.compare("WEIGHTS_0") == 0) vaa = 4;
				if (vaa > -1) {
					glEnableVertexAttribArray(vaa);
					glVertexAttribPointer(vaa, size, accessor.componentType,
										accessor.normalized ? GL_TRUE : GL_FALSE,
										byteStride, BUFFER_OFFSET(accessor.byteOffset));
				} else {
					std::cout << "vaa missing: " << attrib.first << std::endl;
				}
			}

			// Record VAO for later use
			PrimitiveObject primitiveObject;
			primitiveObject.vao = vao;
			primitiveObject.vbos = vbos;
			primitiveObjects.push_back(primitiveObject);

			glBindVertexArray(0);
		}
	}

	void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects, 
						tinygltf::Model &model,
						tinygltf::Node &node) {
		// Bind buffers for the current mesh at the node
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}

		// Recursive into children nodes
		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}

	std::vector<PrimitiveObject> bindModel(tinygltf::Model &model) {
		std::vector<PrimitiveObject> primitiveObjects;

		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}

		return primitiveObjects;
	}

	void drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
				tinygltf::Model &model, tinygltf::Mesh &mesh) {
		
		for (size_t i = 0; i < mesh.primitives.size(); ++i) 
		{
			GLuint vao = primitiveObjects[i].vao;
			std::map<int, GLuint> vbos = primitiveObjects[i].vbos;

			glBindVertexArray(vao);

			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			std::vector<double> color = model.materials[primitive.material].pbrMetallicRoughness.baseColorFactor;
			std::vector<float> k;
			for (double d : color) k.push_back(d); 
			// std::cout << color[0] << " " << k[0] << std::endl;
			glUniform4fv(baseColorFactorID, 1, &k[0]);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));
			glDrawElements(primitive.mode, indexAccessor.count,
						indexAccessor.componentType,
						BUFFER_OFFSET(indexAccessor.byteOffset));
			glBindVertexArray(0);
		}
	}

	void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
						tinygltf::Model &model, tinygltf::Node &node) {
		// Draw the mesh at the node, and recursively do so for children nodes
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			drawMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}
		for (size_t i = 0; i < node.children.size(); i++) {
			drawModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}
	void drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
				tinygltf::Model &model) {
		// Draw all nodes
		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			drawModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}
	}

	void render(glm::mat4 cameraMatrix) {
		glUseProgram(programID);
		// Set camera
		glm::mat4 mvp = cameraMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
		// -----------------------------------------------------------------
		// TODO: Set animation data for linear blend skinning in shader
		// -----------------------------------------------------------------
		// glUniformMatrix4fv(jointMatricesID, 25, GL_FALSE, &skinObjects[0].jointMatrices[0][0][0]);
		// -----------------------------------------------------------------
		// Set light data 
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);
		// Draw the GLTF model
		drawModel(primitiveObjects, model);
		const tinygltf::Skin &skin = model.skins[0];
	}

	void cleanup() {
		glDeleteProgram(programID);
	}
}; 

// int main(void) {
//     // Initialise GLFW
// 	if (!glfwInit())
// 	{
// 		std::cerr << "Failed to initialize GLFW." << std::endl;
// 		return -1;
// 	}

// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
// 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// 	// Open a window and create its OpenGL context
// 	GLFWwindow *window = glfwCreateWindow(1920, 1024, "proj", NULL, NULL);
// 	if (window == NULL)
// 	{
// 		std::cerr << "Failed to open a GLFW window." << std::endl;
// 		glfwTerminate();
// 		return -1;
// 	}
// 	glfwMakeContextCurrent(window);

// // Ensure we can capture the escape key being pressed below
// 	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
// 	glfwSetKeyCallback(window, key_callback);

// 	glfwSetCursorPosCallback(window, cursor_callback);
// 	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
// 	int version = gladLoadGL(glfwGetProcAddress);
// 	if (version == 0)
// 	{
// 		std::cerr << "Failed to initialize OpenGL context." << std::endl;
// 		return -1;
// 	}

// 	// Background
// 	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

// 	glEnable(GL_DEPTH_TEST);
// 	glEnable(GL_CULL_FACE);

// 	std::string filename = "skyscraperB";
// 	std::string filepath = "../final-proj/models/" + filename + ".gltf";

// 	// Our 3D asset
//     MyAsset buildingA;
//     buildingA.initialize(filepath.c_str());

// 	// Camera setup
//     glm::mat4 viewMatrix, projectionMatrix;
// 	projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);

// 	// Time and frame rate tracking
// 	static double lastTime = glfwGetTime();
// 	float time = 0.0f;			// Animation time 
// 	float fTime = 0.0f;			// Time for measuring fps
// 	unsigned long frames = 0;

// 	// Main loop
// 	do
// 	{
// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
// 		// Update states for animation
//         double currentTime = glfwGetTime();
//         float deltaTime = float(currentTime - lastTime);
// 		lastTime = currentTime;
// 		// Rendering
// 		viewMatrix = glm::lookAt(eye_center, eye_center+lookdirection, up);
// 		glm::mat4 vp = projectionMatrix * viewMatrix;
// 		buildingA.render(vp);
// 		// FPS tracking 
// 		// Count number of frames over a few seconds and take average
// 		frames++;
// 		fTime += deltaTime;
// 		if (fTime > 2.0f) {		
// 			float fps = frames / fTime;
// 			frames = 0;
// 			fTime = 0;
			
// 			std::stringstream stream;
// 			stream << std::fixed << std::setprecision(2) << "Lab 4 | Frames per second (FPS): " << fps;
// 			glfwSetWindowTitle(window, stream.str().c_str());
// 		}

// 		// Swap buffers
// 		glfwSwapBuffers(window);
// 		glfwPollEvents();

// 	} // Check if the ESC key was pressed or the window was closed
// 	while (!glfwWindowShouldClose(window));

//     buildingA.cleanup();

// 	// Close OpenGL window and terminate GLFW
// 	glfwTerminate();

// 	return 0;

//     // convertGLBFile("../final-proj/models-glb/skyscraperA.glb", "../final-proj/models/skyscraperA");
// }

// static void convertGLBFile(std::string glbFilename, std::string filename) {

//     std::ifstream binFile = std::ifstream(glbFilename, std::ios::binary); 

//     binFile.seekg(12); //Skip past the 12 byte header, to the json header
//     uint32_t jsonLength;
//     binFile.read((char*)&jsonLength, sizeof(uint32_t)); //Read the length of the json file from it's header

//     std::string jsonStr;
//     jsonStr.resize(jsonLength);
//     binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
//     binFile.read((char*)jsonStr.data(), jsonLength); // Read out the json string
//     // Parse the json
//     // json scene = json::parse(jsonStr);
//     // if(!scene) //jsonStr))
//     //     std::cerr << "Problem parsing assetData: " << std::endl;

//     // After reading from the json, the file cusor will automatically be at the start of the binary header

//     uint32_t binLength;
//     binFile.read((char*)&binLength, sizeof(binLength)); // Read out the bin length from it's header
//     binFile.seekg(sizeof(uint32_t), std::ios_base::cur); // skip chunk type
//     std::vector<unsigned char> bin(binLength);
//     binFile.read((char*)bin.data(), binLength);

//     // Now that we have the files read out, let's actually do something with them
//     // This code prints out all the vertex positions for the first primitive
//     std::ofstream binary(filename + ".bin", std::ios::out|std::ios::binary);
//     std::copy(bin.cbegin(), bin.cend(), std::ostream_iterator<unsigned char>(binary)); 
//     binary.close();

//     std::ofstream gltf = std::ofstream(filename + ".gltf");
//     gltf << jsonStr;
//     gltf.close();
// }


// Is called whenever a key is pressed/released via GLFW
// static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
// {
// 	if (key == GLFW_KEY_R && action == GLFW_PRESS)
// 	{
// 		eye_center = glm::vec3(0.0f, 0.0f, 0.0f);
// 		// lightPosition = glm::vec3(-275.0f, 500.0f, -275.0f);

// 	}
// 	glm::vec3 forwardSpeed = cameraSpeed*lookdirection;
// 	glm::vec3 sideSpeed = cameraSpeed*glm::cross(up, lookdirection);

// 	if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && (action == GLFW_REPEAT || action == GLFW_PRESS))
// 	{
// 		eye_center += forwardSpeed;
// 	}

// 	if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && (action == GLFW_REPEAT || action == GLFW_PRESS))
// 	{
// 		eye_center -= forwardSpeed;
// 	}

// 	if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && (action == GLFW_REPEAT || action == GLFW_PRESS))
// 	{
// 		eye_center += sideSpeed;
// 	}

// 	if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && (action == GLFW_REPEAT || action == GLFW_PRESS))
// 	{
// 		eye_center -= sideSpeed;
// 	}
	    
// 	if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
//     {
//         eye_center.y += cameraSpeed;
//     }
// 	if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && (action == GLFW_REPEAT || action == GLFW_PRESS))
// 	{
// 		eye_center.y -= cameraSpeed;
// 	}
// 	if (key == GLFW_KEY_LEFT_CONTROL) {
// 		if (action == GLFW_PRESS) {
// 			cameraSpeed = 5.f;
// 		}
// 		if (action == GLFW_RELEASE) {
// 			cameraSpeed = 2.f;
// 		}
// 	}
// 	if (eye_center.y < 1) eye_center.y = 1;

// 	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
// 		glfwSetWindowShouldClose(window, GL_TRUE);
// }

// static bool mouseOutsideView = true, LMB_HELD = false;
// static void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
// 	int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
// 	if (xpos < 0 || xpos >= windowWidth || ypos < 0 || ypos > windowHeight || !focused) {
// 		mouseOutsideView = true;
// 		return;
// 	}	

// 	// Normalize to [0, 1] 
// 	float x = xpos / windowWidth;
// 	float y = ypos / windowHeight;

// 	// To [-1, 1] and flip y up 
// 	x = (x * 2.0f - 1.0f)*sensitivity;
// 	y = (1.0f - y * 2.0f)*sensitivity;

// 	if (mouseOutsideView) {
// 		x = y = 0;
// 		mouseOutsideView = false;
// 	}

// 	viewPolar += -y;
// 	viewAzimuth += -x;
// 	if (viewPolar < 0) viewPolar += M_PI*2;
// 	if (viewPolar > M_PI*2) viewPolar -= M_PI*2;
// 	if (viewPolar < 0.1) viewPolar = 0.1;
// 	if (viewPolar > M_PI - 0.1) viewPolar = M_PI - 0.1;

// 	// glm::vec3 sideAxis = glm::normalize(glm::cross(up, lookdirection));
// 	lookdirection = glm::vec3(sin(viewPolar)*cos(viewAzimuth),cos(viewPolar),-sin(viewPolar)*sin(viewAzimuth));
	
// 	glfwSetCursorPos(window, halfWidth,halfHeight);
// }

// static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
// {
//     if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
//         if (action == GLFW_PRESS) 
// 			LMB_HELD = true;
// 		if (action == GLFW_RELEASE)
// 			LMB_HELD = false;
// }