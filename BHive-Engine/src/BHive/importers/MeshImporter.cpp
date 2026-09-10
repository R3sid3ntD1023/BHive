#include "MeshImporter.h"
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "TextureImporter.h"
#include "gfx/Texture.h"
#include "gfx/mesh/MeshData.h"

namespace BHive
{
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
		std::string GetTextureType(aiTextureType aiType)
		{
			switch (aiType)
			{
			case aiTextureType_DIFFUSE:
				return "Albedo";
			case aiTextureType_SPECULAR:
				return "Roughness";
			case aiTextureType_AMBIENT:
				return "Ambient";
			case aiTextureType_EMISSIVE:
				return "Emission";
			case aiTextureType_HEIGHT:
				return "Height";
			case aiTextureType_NORMALS:
				return "Normal";
			case aiTextureType_SHININESS:
				return "Shininess";
			case aiTextureType_OPACITY:
				return "Opacity";
			case aiTextureType_DISPLACEMENT:
				return "Displacement";
			case aiTextureType_LIGHTMAP:
				return "Lightmap";
			case aiTextureType_REFLECTION:
				return "Reflection";
			case aiTextureType_BASE_COLOR:
				return "Albedo";
			case aiTextureType_NORMAL_CAMERA:
				return "NormalCamera";
			case aiTextureType_EMISSION_COLOR:
				return "EmissionColor";
			case aiTextureType_METALNESS:
				return "Metallic";
			case aiTextureType_DIFFUSE_ROUGHNESS:
				return "DiffuseRoughness";
			case aiTextureType_AMBIENT_OCCLUSION:
				return "AmbientOcclusion";
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

			return "";
		}
	} // namespace utils

	class AssimpParser
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

		void SetVertexBoneData(FVertex &vertex, int id, float weight)
		{
			for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
			{
				if (vertex.BoneIDs[i] < 0)
				{
					vertex.BoneIDs[i] = id;
					vertex.Weights[i] = weight;
					break;
				}
			}
		}

		void ExtractBoneWeightsForVertices(std::vector<FVertex> &vertices, const aiMesh *mesh)
		{
			auto &bones = mDecodedMesh.Bones.Bones;
			auto &boneNames = mDecodedMesh.Bones.BoneNames;

			for (unsigned bone_index = 0; bone_index < mesh->mNumBones; bone_index++)
			{
				int bone_id = -1;
				auto bone = mesh->mBones[bone_index];

				std::string bone_name = bone->mName.C_Str();
				uint64_t name_hash = std::hash<std::string>()(bone_name);

				if (!bones.contains(name_hash))
				{
					bones[name_hash] = {name_hash, mBoneCounter, utils::make_mat4(bone->mOffsetMatrix)};
					boneNames[bone_name] = name_hash;
					bone_id = mBoneCounter;
					mBoneCounter++;
				}
				else
				{
					bone_id = bones.at(name_hash).ID;
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

		FSubMesh ParseMesh(const aiMatrix4x4 &matrix, const aiMesh *mesh)
		{
			FMeshData &data = mDecodedMesh.MeshData;

#if 1
	#define IMPORT_SCALE glm::scale(glm::mat4(1.0f), glm::vec3(mImportScale)) *
#else
	#define IMPORT_SCALE
#endif
			auto node_matrix = IMPORT_SCALE utils::make_mat4(matrix);

			FSubMesh sub_mesh{};
			sub_mesh.StartVertex = (uint32_t)data.Vertices.size();
			sub_mesh.StartIndex = (uint32_t)data.Indices.size();
			sub_mesh.IndexCount = mesh->mNumFaces * 3;
			sub_mesh.Transformation = node_matrix;
			sub_mesh.MaterialIndex = mesh->mMaterialIndex;

			std::vector<FVertex> vertices(mesh->mNumVertices);
			std::vector<uint32_t> indices(mesh->mNumFaces * 3);

			for (unsigned v = 0; v < mesh->mNumVertices; v++)
			{

				glm::vec3 position = utils::make_vec3(mesh->mVertices[v]);
				glm::vec2 texcoord = {0.0f, 0.0f};
				glm::vec3 normal = {0.0f, 0.0f, 0.0f};
				glm::vec3 tangent = {0.0f, 0.0f, 0.0f};
				glm::vec3 bitangent = {0.0f, 0.0f, 0.0f};
				glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

				if (mesh->HasTextureCoords(0))
				{
					texcoord = utils::make_vec2(mesh->mTextureCoords[0][v]);
				}

				if (mesh->HasNormals())
				{
					normal = utils::make_vec3(mesh->mNormals[v]);
				}

				if (mesh->mTangents)
				{
					tangent = utils::make_vec3(mesh->mTangents[v]);
				}

				if (mesh->mBitangents)
				{
					bitangent = utils::make_vec3(mesh->mBitangents[v]);
				}

				if (mesh->HasVertexColors(0))
				{
					color = utils::make_vec4(mesh->mColors[0][v]);
				}

				FVertex vertex{};
				vertex.Position = position;
				vertex.TexCoord = texcoord;
				vertex.Normal = normal;
				vertex.BiNormal = bitangent;
				vertex.Tangent = tangent;
				vertex.TexCoord = texcoord;
				vertices[v] = vertex;

				sub_mesh.Bounds.Min = glm::min(position, sub_mesh.Bounds.Min);
				sub_mesh.Bounds.Max = glm::max(position, sub_mesh.Bounds.Max);

				data.Bounds.Min = glm::min(position, data.Bounds.Min);
				data.Bounds.Max = glm::max(position, data.Bounds.Max);
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
				ExtractBoneWeightsForVertices(vertices, mesh);
			}

			data.Vertices.insert(data.Vertices.end(), vertices.begin(), vertices.end());
			data.Indices.insert(data.Indices.end(), indices.begin(), indices.end());

			return sub_mesh;
		}

		void ProcessNode(const aiNode *node, const aiMatrix4x4 &parent)
		{

			for (unsigned i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh *mesh = mScene->mMeshes[node->mMeshes[i]];
				auto submesh = ParseMesh(parent * node->mTransformation, mesh);
				mDecodedMesh.MeshData.SubMeshes.emplace_back(submesh);
			}

			for (unsigned i = 0; i < node->mNumChildren; i++)
			{
				ProcessNode(node->mChildren[i], parent * node->mTransformation);
			}
		}

		void GetNodeHeiracrchy(aiNode *node, SkeletalNode &out)
		{
			out.NameHash = std::hash<std::string>()(node->mName.C_Str());
			out.Transformation = utils::make_mat4(node->mTransformation);

			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				SkeletalNode child_data;
				GetNodeHeiracrchy(node->mChildren[i], child_data);
				out.Children.push_back(child_data);
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

				position.mValue = utils::make_vec3(channel->mPositionKeys[p].mValue);
				position.mTimeStamp = (float)channel->mPositionKeys[p].mTime;

				data.mPositions[p] = (position);
			}

			for (unsigned p = 0; p < num_rotations; p++)
			{
				TKeyFrame<glm::quat> rotation;

				rotation.mValue = utils::make_quat(channel->mRotationKeys[p].mValue);
				rotation.mTimeStamp = (float)channel->mRotationKeys[p].mTime;

				data.mRotations[p] = (rotation);
			}

			for (unsigned p = 0; p < num_scales; p++)
			{
				TKeyFrame<glm::vec3> scale;

				scale.mValue = utils::make_vec3(channel->mScalingKeys[p].mValue);
				scale.mTimeStamp = (float)channel->mScalingKeys[p].mTime;

				data.mScales[p] = (scale);
			}
		}

		void ReadMissingBones(aiAnimation *animation)
		{
			auto &bones = mDecodedMesh.Bones.Bones;
			auto &boneNames = mDecodedMesh.Bones.BoneNames;

			int32_t bone_count = (int32_t)bones.size();

			for (unsigned j = 0; j < animation->mNumChannels; j++)
			{

				auto channel = animation->mChannels[j];
				std::string bone_name = channel->mNodeName.data;
				uint64_t name_hash = std::hash<std::string>()(bone_name);

				// find missing bones
				if (!bones.contains(name_hash))
				{
					bones[name_hash] = {name_hash, bone_count};
					boneNames[bone_name] = name_hash;
					bone_count++;
				}
			}
		}

		void GetAnimationData()
		{
			for (unsigned int i = 0; i < mScene->mNumAnimations; i++)
			{
				aiAnimation *animation = mScene->mAnimations[i];
				ReadMissingBones(animation);

				DecodedAnimation anim{};
				anim.Name = animation->mName.C_Str();
				anim.Duration = (float)animation->mDuration;
				anim.TicksPerSecond = (float)animation->mTicksPerSecond;

				for (unsigned j = 0; j < animation->mNumChannels; j++)
				{
					auto channel = animation->mChannels[j];
					uint64_t bone_name = std::hash<std::string>()(channel->mNodeName.data);

					// get animation keys
					FrameData frame_data;
					ParseAnimationData(animation, channel, frame_data);
					anim.Frames.emplace(bone_name, frame_data);
				}

				mDecodedMesh.Animations.emplace_back(anim);
			}
		}

		EmbeddedTexture GetTextureData(const aiScene *scene, aiString &str)
		{
			EmbeddedTexture out{};
			out.Path = str.C_Str();

			if (auto embedded = scene->GetEmbeddedTexture(str.C_Str()))
			{
				auto size = embedded->mWidth + embedded->mHeight;
				out.EmbeddedData.Allocate(embedded->pcData, size);
				out.Type = EmbeddedTexture::Embedded;
			}

			return out;
		}

		void GetMaterialData()
		{
			static aiTextureType supported_textures[] = {

				aiTextureType_DIFFUSE,	  aiTextureType_EMISSIVE,  aiTextureType_NORMALS,  aiTextureType_OPACITY,
				aiTextureType_BASE_COLOR, aiTextureType_METALNESS, aiTextureType_SPECULAR, aiTextureType_DIFFUSE_ROUGHNESS,
			};

			auto &materials = mDecodedMesh.Materials;
			auto count = mScene->mNumMaterials;
			aiString str;

			materials.resize(count);
			mDecodedMesh.MeshData.MaterialCount = count;

			for (unsigned i = 0; i < count; i++)
			{

				auto loaded_material = mScene->mMaterials[i];
				auto name = loaded_material->GetName().C_Str();

				auto &material = materials[i];
				material.Name = name;
				auto &textures = material.Textures;

				for (unsigned j = 1; j < 8; j++)
				{
					if (loaded_material->GetTexture(supported_textures[j], 0, &str) == aiReturn_SUCCESS)
					{
						auto texName = str.C_Str();
						auto it = std::find_if(textures.begin(), textures.end(), [texName](const auto &t) { return t.GetName() == texName; });
						if (it == textures.end())
						{
							EmbeddedTexture texture = GetTextureData(mScene, str);
							texture.Type = utils::GetTextureType(supported_textures[j]);
							material.Textures.emplace_back(texture);
						}
					}
				}

				aiColor4D albedo = {.5f, .5f, .5f, 1.f};
				float metallic = 0.f;
				float roughness = 1.0f;

				if (aiGetMaterialColor(loaded_material, AI_MATKEY_BASE_COLOR, &albedo) == aiReturn_SUCCESS)
					material.Albedo = {albedo.r, albedo.g, albedo.b, albedo.a};
				if (aiGetMaterialFloat(loaded_material, AI_MATKEY_METALLIC_FACTOR, &metallic) == aiReturn_SUCCESS)
					material.Metallic = metallic;
				if (aiGetMaterialFloat(loaded_material, AI_MATKEY_ROUGHNESS_FACTOR, &roughness) == aiReturn_SUCCESS)
					material.Roughness = roughness;
			}
		}

		void ProcessScene()
		{
			mScene->mRootNode->mTransformation = aiMatrix4x4({1.f, 1.f, 1.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f});
			ProcessNode(mScene->mRootNode, mScene->mRootNode->mTransformation);
			GetNodeHeiracrchy(mScene->mRootNode, mDecodedMesh.BoneHeirarchy);
			GetMaterialData();
			GetAnimationData();
		}

	public:
		AssimpParser(const std::filesystem::path &path, float importScale)
			: mImportScale(importScale)
		{
			Assimp::Importer importer;
			importer.SetProgressHandler(new ModelProgress());
			int flags = aiProcessPreset_TargetRealtime_Fast;
			mScene = importer.ReadFile(path.string().c_str(), (unsigned)flags);

			if (!mScene || !mScene->mRootNode)
			{
				LOG_ERROR("AssimpParser::Error - {}", importer.GetErrorString());
			}
			else
			{
				ProcessScene();
			}

			mDecodedMesh.Path = path;
		}

		DecodedMesh GetResult() const { return mDecodedMesh; }

	private:
		const aiScene *mScene = nullptr;
		DecodedMesh mDecodedMesh;
		float mImportScale = 1.0f;
		int32_t mBoneCounter = 0;
	};

	DecodedMesh MeshImporter::Import(const std::filesystem::path &path, float importScale)
	{
		AssimpParser parser(path, importScale);
		return parser.GetResult();
	}
} // namespace BHive