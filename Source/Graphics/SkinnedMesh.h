#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>
#include "cereal/archives/binary.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/set.hpp"
#include "cereal/types/unordered_map.hpp"

namespace DirectX
{
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}


	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

struct scene
{
	struct node
	{
		uint64_t unique_id{ 0 };
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parent_index{ -1 };

		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, attribute, parent_index);
		}

	};
	std::vector<node> nodes;
	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const node& node : nodes)
		{
			if (node.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}
	template<class T>
	void serialize(T& archive)
	{
		archive(nodes);
	}
};

struct skeleton
{
	struct bone
	{
		uint64_t unique_id{ 0 };
		std::string name;
		// 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
		int64_t parent_index{ -1 }; // -1 : the bone is orphan
		// 'node_index' is an index that refers to the node array of the scene.
		int64_t node_index{ 0 };

		// 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
		DirectX::XMFLOAT4X4 offset_transform{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

		bool is_orphan() const { return parent_index < 0; }

		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, parent_index, node_index, offset_transform);
		}
	};

	std::vector<bone> bones;
	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const bone& bone : bones)
		{
			if (bone.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

	template<class T>
	void serialize(T& archive)
	{
		archive(bones);
	}
};

struct animation
{
	std::string name;
	float sampling_rate{ 0 };

	struct keyframe
	{
		struct node
		{
			//�{�[���̍s��
			DirectX::XMFLOAT4X4 global_transform{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
			//�v���O�������łځ[��̒l��ύX���邽�߂̕ϐ����`����
			DirectX::XMFLOAT3 scaling{ 1,1,1 };
			DirectX::XMFLOAT4 rotation{ 0,0,0,1 };//�N�I�[�^�j�I��
			DirectX::XMFLOAT3 translation{ 0,0,0 };

			template<class T>
			void serialize(T& archive)
			{
				archive(global_transform, scaling, rotation, translation);
			}
		};

		std::vector<node> nodes;

		template<class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}
	};
	std::vector<keyframe> sequence;
	template<class T>
	void serialize(T& archive)
	{
		archive(name, sampling_rate, sequence);
	}
};



class SkinnedMesh
{
public:
	static const int MAX_BONE_INFLUENCES{ 4 };
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal{ 0,1,0 };
		DirectX::XMFLOAT4 tangent{ 1,0,0,1 };
		DirectX::XMFLOAT2 texcoord{ 0,0 };
		float bone_weights[MAX_BONE_INFLUENCES]{ 1,0,0,0 };
		uint32_t bone_indices[MAX_BONE_INFLUENCES]{};

		template<class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord, bone_weights, bone_indices);
		}
	};

	static const int MAX_BONES{ 256 };
	struct constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1} };

		template<class T>
		void serialize(T& archive)
		{
			archive(world, material_color, bone_transforms);
		}
	};

	struct material
	{
		uint64_t unique_id{ 0 };
		std::string name;

		DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };

		std::string texture_filenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];

		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, Ka, Kd, Ks, texture_filenames);
		}
	};

	std::unordered_map<uint64_t, material> materials;

	struct mesh
	{
		uint64_t unique_id{ 0 };
		std::string name;
		// 'node_index' �́A�V�[���̃m�[�h�z����Q�Ƃ���C���f�b�N�X�ł��B
		int64_t node_index{ 0 };

		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;

		DirectX::XMFLOAT4X4 default_global_transform{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

		struct subset
		{
			uint64_t material_unique_id{ 0 };
			std::string material_name;

			uint32_t start_index_location{ 0 };
			uint32_t index_count{ 0 };

			material* material = nullptr;

			template<class T>
			void serialize(T& archive)
			{
				archive(material_unique_id, material_name, start_index_location, index_count);
			}
		};
		std::vector<subset> subsets;

		skeleton bind_pose;

		//���E�{�b�N�X
		DirectX::XMFLOAT3 bounding_box[2]
		{
			{+D3D11_FLOAT32_MAX,+D3D11_FLOAT32_MAX,+D3D11_FLOAT32_MAX},
			{-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
		};

		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, node_index, subsets, default_global_transform,
				bind_pose, bounding_box, vertices, indices);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		friend class SkinnedMesh;
	};
	std::vector<mesh> meshes;

	//���f���\�z
	void BulidModel(ID3D11Device* device,const char* dirname);


	//�e�N�X�`���ǂݍ���
	HRESULT LoadTexture(ID3D11Device* device, const char* filename, const char* suffix, bool dummy, ID3D11ShaderResourceView** srv, UINT dummy_color = 0xFFFFFFFF);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
public:
	SkinnedMesh(ID3D11Device* device, const char* fbx_filename, bool triangulate = false);
	virtual ~SkinnedMesh() = default;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="immediate_context">�f�o�C�X�R���e�L�X�g</param>
	/// <param name="world">���[���h�s��</param>
	/// <param name="material_color">���f���S�̂̐F</param>
	/// <param name="keyframe">1�t���[���̃��f���̑S�{�[���̎p���s��</param>
	void render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
		const animation::keyframe* keyframe);

	/// �X�V�A�j���[�V�����֐����`
	void update_animation(animation::keyframe& keyframe);

	bool append_animation(const char* animation_finename, float sampling_rate);

	void blend_animations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe);

	//�{�[���̔ԍ�����
	int findBoneIndex(const char* name);
	int findBoneIndex(const char* name, int meshIndex);

	//���b�V���̔ԍ�����
	int findMeshIndex(const char* name);

	//�{�[���̃|�W�V�����̎擾
	DirectX::XMFLOAT3 getBonePosition(int meshIndex, int boneIndex, animation::keyframe& key,DirectX::XMFLOAT4X4& transform);

	std::vector<animation> animation_clips;

protected:
	scene scene_view;

	/// <summary>
	/// ���b�V�����̒��o�i���b�V�����̎��o���j
	/// </summary>
	/// <param name="fbx_scene">���f���̃f�[�^</param>
	/// <param name="meshes">���o�������b�V�����i�o�͗p�j</param>
	void fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);

	/// <summary>
	/// �}�e���A�����̒��o
	/// </summary>
	/// <param name="fbx_scene">���f���̃f�[�^</param>
	/// <param name="materials">���o�����}�e���A�����i�o�͗p�j</param>
	void fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);

	/// <summary>
	/// �o�C���h�|�[�Y�̒��o
	/// </summary>
	/// <param name="fbx_mesh">���f���f�[�^</param>
	/// <param name="bind_pose">���o�����o�C���h�|�[�Y�i�I�t�Z�b�g�s��j</param>
	void fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose);

	/// <summary>
	/// �A�j���[�V�����f�[�^�̒��o
	/// </summary>
	/// <param name="fbx_scene">���f���f�[�^</param>
	/// <param name="animation_clips">���o�����A�j���[�V�����f�[�^</param>
	/// <param name="sampling_rate">�T���v�����O���[�g�i�O���f�t�H���g�j</param>
	void fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips,
		float sampling_rate);

	/// <summary>
	/// COM �I�u�W�F�N�g�̐���
	/// </summary>
	void create_com_objects(ID3D11Device* device, const char* fbx_filename);

private:



};