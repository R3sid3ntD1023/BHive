#include "MeshImporter.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/material.h>

#include "math/Math.h"
#include "importers/TextureImporter.h"
#include "gfx/Texture.h"

namespace BHive
{
	struct ModelProgress : public Assimp::ProgressHandler
	{
		virtual bool Update(float percentage = -1.f)
		{
			bool finished = percentage == 1.f;
			auto str = std::format("\rLoading... {:.2f}%", percentage * 100.0f);
			std::cout << str << (!finished ? "" : "\n");

			return finished;
		}
	};

	namespace utils
	{

		glm::mat4 make_mat4(const aiMatrix4x4 &mat)
		{
			return glm::transpose(glm::make_mat4(&mat.a1));
		}

		glm::vec4 make_vec4(const aiColor4D &vec)
		{
			return {vec.r, vec.g, vec.b, vec.a};
		}

		glm::vec3 make_vec3(const aiVector3D &vec)
		{
			return {vec.x, vec.y, vec.z};
		}

		glm::vec2 make_vec2(const aiVector3D &vec)
		{
			return {vec.x, vec.y};
		}

		glm::quat make_quat(const aiQuaternion &quat)
		{
			return glm::quat(quat.w, quat.x, quat.y, quat.z);
		}

		void SetVertexBoneData(FVertex &vertex, int id, float weight)
		{
			for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
			{
				if (vertex.mBoneID[i] < 0)
				{
					vertex.mBoneID[i] = id;
					vertex.mWeights[i] = weight;
					break;
				}
			}
		}

		void ExtractBoneWeightsForVertices(Bones &bones, std::vector<FVertex> &vertices, const aiMesh *mesh)
		{
			int bone_counter = 0;
			for (unsigned bone_index = 0; bone_index < mesh->mNumBones; bone_index++)
			{
				int bone_id = -1;
				std::string bone_name = mesh->mBones[bone_index]->mName.C_Str();
				if (!bones.contains(bone_name))
				{
					Bone bone{.mName = bone_name, .mID = bone_counter, .mOffset = make_mat4(mesh->mBones[bone_index]->mOffsetMatrix)};
					bones[bone_name] = bone;
					bone_id = bone_counter;
					bone_counter++;
				}
				else
				{
					bone_id = bones.at(bone_name).mID;
				}

				ASSERT(bone_id != -1);
				auto weights = mesh->mBones[bone_index]->mWeights;
				auto numWeights = mesh->mBones[bone_index]->mNumWeights;

				for (unsigned weightIndex = 0; weightIndex < numWeights; weightIndex++)
				{
					int vertex_id = weights[weightIndex].mVertexId;
					float weight = weights[weightIndex].mWeight;
					ASSERT(vertex_id <= vertices.size());
					SetVertexBoneData(vertices[vertex_id], bone_id, weight);
				}
			}
		}

		FSubMesh ParseMesh(const aiScene *scene, const aiMesh *mesh, FMeshImportData &import_data)
		{
			FMeshData &data = import_data.mMeshData;

			FSubMesh sub_mesh{};
			sub_mesh.mStartVertex = (uint32_t)data.mVertices.size();
			sub_mesh.mVertexCount = mesh->mNumVertices;
			sub_mesh.mStartIndex = (int32_t)data.mIndices.size();
			sub_mesh.mIndexCount = mesh->mNumFaces * 3;

			aiMatrix4x4 root_transfrom = scene->mRootNode->mTransformation;

			std::vector<FVertex> vertices(mesh->mNumVertices);
			std::vector<uint32_t> indices(mesh->mNumFaces * 3);
			

			if (mesh->mMaterialIndex >= 0)
				sub_mesh.mMaterialIndex = mesh->mMaterialIndex;

			for (unsigned v = 0; v < mesh->mNumVertices; v++)
			{

				glm::vec3 position = glm::vec4(make_vec3(mesh->mVertices[v]), 1.0);
				glm::vec2 texcoord = {0.0f, 0.0f};
				glm::vec3 normal = {0.0f, 0.0f, 0.0f};
				glm::vec3 tangent = {0.0f, 0.0f, 0.0f};
				glm::vec3 bitangent = {0.0f, 0.0f, 0.0f};
				glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

				if (mesh->HasTextureCoords(0))
				{
					texcoord = make_vec2(mesh->mTextureCoords[0][v]);
				}

				if (mesh->HasNormals())
				{
					normal = make_vec3(mesh->mNormals[v]);
				}

				if (mesh->mTangents)
				{
					tangent = make_vec3(mesh->mTangents[v]);
				}

				if (mesh->mBitangents)
				{
					bitangent = make_vec3(mesh->mBitangents[v]);
				}

				if (mesh->HasVertexColors(0))
				{
					color = make_vec4(mesh->mColors[0][v]);
				}

				FVertex vertex{};
				vertex.Position = position;
				vertex.TexCoord = texcoord;
				vertex.Normal = normal;
				vertex.BiNormal = bitangent;
				vertex.Tangent = tangent;
				vertex.TexCoord = texcoord;

				for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
				{
					vertex.mBoneID[i] = -1;
					vertex.mWeights[i] = 0.f;
				}
				vertices[v] = vertex;

				data.mBoundingBox.Min = glm::min(position, data.mBoundingBox.Min);
				data.mBoundingBox.Max = glm::max(position, data.mBoundingBox.Max);
			}

			// process indices
			for (unsigned i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace &face = mesh->mFaces[i];
				for (unsigned j = 0; j < 3; j++)
				{
					indices[(i * 3) + j] = face.mIndices[j];
				}
			}

			// proccess bones
			if (mesh->HasBones())
			{
				// LOG_TRACE("Mesh has Bones {}", mesh->mName.C_Str());
				ExtractBoneWeightsForVertices(import_data.mBoneData, vertices, mesh);
			}

			data.mVertices.insert(data.mVertices.end(), vertices.begin(), vertices.end());
			data.mIndices.insert(data.mIndices.end(), indices.begin(), indices.end());

			return sub_mesh;
		}


		void ProcessMeshes(const aiScene* scene, FMeshImportData &data)
		{
			auto count = scene->mNumMeshes;
			data.mMeshData.mSubMeshes.resize(count);

			for (unsigned i = 0; i < count; i++)
			{
				aiMesh* mesh = scene->mMeshes[i];
				auto submesh = ParseMesh(scene, mesh, data);
				data.mMeshData.mSubMeshes[i] = submesh;
			}
		}

		void ParseAnimationData(aiAnimation *animation, aiNodeAnim *channel, FrameData &data)
		{
			unsigned num_positions = channel->mNumPositionKeys;
			unsigned num_rotations = channel->mNumRotationKeys;
			unsigned num_scales = channel->mNumScalingKeys;

			data.mPositions.resize(num_positions);
			data.mRotations.resize(num_rotations);
			data.mScales.resize(num_scales);

			for (unsigned p = 0; p < num_positions; p++)
			{
				TKeyFrame<glm::vec3> position;

				position.mValue = make_vec3(channel->mPositionKeys[p].mValue);
				position.mTimeStamp = (float)channel->mPositionKeys[p].mTime;

				data.mPositions[p] = (position);
			}

			for (unsigned p = 0; p < num_rotations; p++)
			{
				TKeyFrame<glm::quat> rotation;

				rotation.mValue = make_quat(channel->mRotationKeys[p].mValue);
				rotation.mTimeStamp = (float)channel->mRotationKeys[p].mTime;

				data.mRotations[p] = (rotation);
			}

			for (unsigned p = 0; p < num_scales; p++)
			{
				TKeyFrame<glm::vec3> scale;

				scale.mValue = make_vec3(channel->mScalingKeys[p].mValue);
				scale.mTimeStamp = (float)channel->mScalingKeys[p].mTime;

				data.mScales[p] = (scale);
			}
		}

		void GetNodeHeiracrchy(aiNode *node, SkeletalNode &data)
		{
			data.mName = node->mName.C_Str();
			data.mTransformation = make_mat4(node->mTransformation);

			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				SkeletalNode child_data;
				GetNodeHeiracrchy(node->mChildren[i], child_data);
				data.mChildren.push_back(child_data);
			}
		}

		void GetAnimationData(const aiScene *scene, std::vector<FImportedAnimationData> &data, Bones &bones)
		{
			aiMatrix4x4 global_inverse_matrix = scene->mRootNode->mTransformation;
			global_inverse_matrix = global_inverse_matrix.Inverse();

			int32_t bone_count = (int32_t)bones.size();

			for (unsigned int i = 0; i < scene->mNumAnimations; i++)
			{
				aiAnimation *animation = scene->mAnimations[i];

				std::map<std::string, FrameData> frames;
				for (unsigned j = 0; j < animation->mNumChannels; j++)
				{

					auto channel = animation->mChannels[j];
					std::string bone_name = channel->mNodeName.data;
					// remove_extension(bone_name);

					// find missing bones
					if (!bones.contains(bone_name))
					{
						bones[bone_name].mID = bone_count;
						bones[bone_name].mName = bone_name;
						bone_count++;
						// LOG_TRACE("Added Missing Bone {}", bone_name);
					}

					// get animation keys
					FrameData frame_data;
					ParseAnimationData(animation, channel, frame_data);
					frames.emplace(bone_name, frame_data);

					// LOG_TRACE("Added Bone to animation {}", bone_name);
				}

				FImportedAnimationData animation_data;
				animation_data.mName = animation->mName.C_Str();
				animation_data.mDuration = (float)animation->mDuration;
				animation_data.TicksPerSecond = (float)animation->mTicksPerSecond;
				animation_data.mFrames = frames;
				animation_data.mGlobalInverseMatrix = make_mat4(global_inverse_matrix);

				data.push_back(animation_data);
			}
		}

		void GetTextureData(const aiScene *scene, aiString &str, FTextureData &data)
		{
			data.mPath = str.C_Str();

			if (auto embedded_texture = scene->GetEmbeddedTexture(str.C_Str()))
			{
				auto size = embedded_texture->mWidth + embedded_texture->mHeight;

				auto pixels = new uint8_t[size];
				memcpy(pixels, embedded_texture->pcData, size);

				data.mEmbeddedData = pixels;
				data.mEmbeddedDataSize = size;
			}
		}

		void GetTextureType(aiTextureType aiType, FTextureData::EType& type)
		{
			switch (aiType)
			{
			case aiTextureType_NONE:
				break;
			case aiTextureType_DIFFUSE:
				type = FTextureData::Type_ALBEDO;
				break;
			case aiTextureType_SPECULAR:
				type = FTextureData::Type_ROUGHNESS;
				break;
			case aiTextureType_AMBIENT:
				break;
			case aiTextureType_EMISSIVE:
				type = FTextureData::Type_EMISSION;
				break;
			case aiTextureType_HEIGHT:
				type = FTextureData::Type_DISPLACEMENT;
				break;
			case aiTextureType_NORMALS:
				type = FTextureData::Type_NORMAL;
				break;
			case aiTextureType_SHININESS:
				type = FTextureData::Type_METALLIC;
				break;
			case aiTextureType_OPACITY:
				type = FTextureData::Type_OPACITY;
				break;
			case aiTextureType_DISPLACEMENT:
				type = FTextureData::Type_DISPLACEMENT;
				break;
			case aiTextureType_LIGHTMAP:
				break;
			case aiTextureType_REFLECTION:
				break;
			case aiTextureType_BASE_COLOR:
				type = FTextureData::Type_ALBEDO;
				break;
			case aiTextureType_NORMAL_CAMERA:
				break;
			case aiTextureType_EMISSION_COLOR:
				break;
			case aiTextureType_METALNESS:
				type = FTextureData::Type_METALLIC;
				break;
			case aiTextureType_DIFFUSE_ROUGHNESS:
				type = FTextureData::Type_ROUGHNESS;
				break;
			case aiTextureType_AMBIENT_OCCLUSION:
				break;
			case aiTextureType_UNKNOWN:
				break;
			case aiTextureType_SHEEN:
				break;
			case aiTextureType_CLEARCOAT:
				break;
			case aiTextureType_TRANSMISSION:
				break;
			case aiTextureType_MAYA_BASE:
				break;
			case aiTextureType_MAYA_SPECULAR:
				break;
			case aiTextureType_MAYA_SPECULAR_COLOR:
				break;
			case aiTextureType_MAYA_SPECULAR_ROUGHNESS:
				break;
			case _aiTextureType_Force32Bit:
				break;
			default:
				break;
			}
		}

		void GetMaterialData(const aiScene *scene, std::vector<FMaterialData> &materials)
		{
				static aiTextureType supported_textures[] =
				{

					aiTextureType_DIFFUSE,
					aiTextureType_EMISSIVE,
					aiTextureType_NORMALS,
					aiTextureType_OPACITY,
					aiTextureType_BASE_COLOR,
					aiTextureType_METALNESS,
					aiTextureType_SPECULAR,
					aiTextureType_DIFFUSE_ROUGHNESS,
				};

			auto count = scene->mNumMaterials;
			aiString str;

			materials.resize(count);

			for (unsigned i = 0; i < count; i++)
			{

				auto loaded_material = scene->mMaterials[i];
				auto &material = materials[i];

				material.mName = loaded_material->GetName().C_Str();

				for (unsigned j = 1; j < 9; j++)
				{
					auto texture_count = loaded_material->GetTextureCount(supported_textures[j]);

					if (loaded_material->GetTexture(supported_textures[j], 0, &str) == aiReturn_SUCCESS)
					{
						if (!material.mLoadedTextureNames.contains(str.C_Str()))
						{
							FTextureData texture_data;
							GetTextureType(supported_textures[j], texture_data.mType);
							GetTextureData(scene, str, texture_data);

							material.mTextureData.push_back(texture_data);
							material.mLoadedTextureNames.insert(str.C_Str());
						}
					}
				}

				aiColor4D albedo = {.5f, .5f, .5f, 1.f};
				float metallic = 0.f;
				float roughness = 1.0f;

				if (aiGetMaterialColor(loaded_material, AI_MATKEY_BASE_COLOR, &albedo) == aiReturn_SUCCESS)
					material.mAlbedo = {albedo.r, albedo.g, albedo.b, albedo.a};
				if (aiGetMaterialFloat(loaded_material, AI_MATKEY_METALLIC_FACTOR, &metallic) == aiReturn_SUCCESS)
					material.mMetallic = metallic;
				if (aiGetMaterialFloat(loaded_material, AI_MATKEY_METALLIC_FACTOR, &roughness) == aiReturn_SUCCESS)
					material.mRoughness = roughness;
			}
		}

		void ProcessScene(const aiScene *scene, FMeshImportData &data)
		{
			utils::ProcessMeshes(scene, data);

			utils::GetMaterialData(scene, data.mMaterialData);
			utils::GetAnimationData(scene, data.mAnimationData, data.mBoneData);

			if (data.mBoneData.size())
			{
				utils::GetNodeHeiracrchy(scene->mRootNode, data.mSkeletonHeirarchyData);
			}
		}

	}

	FMeshImportData MeshImporter::Import(const std::filesystem::path &path)
	{
		Assimp::Importer importer;
		importer.SetProgressHandler(new ModelProgress());
		int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_RemoveRedundantMaterials;
		const aiScene *scene = importer.ReadFile(path.string().c_str(), (unsigned)flags);

		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			LOG_ERROR("MeshImporter::Error - {0}", importer.GetErrorString());
			return {};
		}

		FMeshImportData data;
		utils::ProcessScene(scene, data);

		return data;
	}
}