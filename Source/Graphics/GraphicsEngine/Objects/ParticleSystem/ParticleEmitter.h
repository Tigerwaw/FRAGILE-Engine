#pragma once
#include "Objects/Vertices/ParticleVertex.h"
#include "ParticleEmitterSettings.h"

#include "Math/AABB3D.hpp"

class Material;
class DynamicVertexBuffer;
struct ParticleVertex;

class ParticleEmitter
{
	friend class Drawer;
	friend class ParticleSystem;

	public:
		ParticleEmitter();
		virtual ~ParticleEmitter();
		void Update(float aDeltaTime);

		void SetMaterial(std::shared_ptr<Material> aMaterial) { myMaterial = aMaterial; }
		const std::shared_ptr<Material> GetMaterial() const { return myMaterial; }

		const ParticleEmitterSettings& GetEmitterSettings() const { return mySettings; }

		void ResetParticles();

		const Math::AABB3D<float>& GetBoundingBox() const { return myBoundingBox; }

	protected:
		virtual void InitParticle(ParticleVertex& aParticle, size_t aIndex);
		virtual void UpdateParticle(ParticleVertex& aParticle, float aDeltaTime);
		virtual void InitInternal();
		std::vector<ParticleVertex> myParticles;
	private:
		void UpdateBoundingBox(const Math::Vector3f& aPosition);

		std::shared_ptr<DynamicVertexBuffer> myVertexBuffer;
		std::shared_ptr<Material> myMaterial;
		ParticleEmitterSettings mySettings;

		Math::AABB3D<float> myBoundingBox;
};