#pragma once
#include "Objects/Vertices/ParticleVertex.h"
#include "ParticleEmitterSettings.h"

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

	protected:
		virtual void InitParticle(ParticleVertex& aParticle, size_t aIndex);
		virtual void UpdateParticle(ParticleVertex& aParticle, float aDeltaTime);
		virtual void InitInternal();
		std::vector<ParticleVertex> myParticles;
	private:
		std::shared_ptr<DynamicVertexBuffer> myVertexBuffer;
		std::shared_ptr<Material> myMaterial;
		ParticleEmitterSettings mySettings;
};