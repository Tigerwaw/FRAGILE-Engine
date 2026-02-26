#pragma once

class Mesh;
class Texture;
class Material;
class DynamicVertexBuffer;
class Text;
class ParticleEmitter;
class TrailEmitter;

class Drawer
{
public:
	void RenderQuad();
	void RenderSkybox(const Mesh& aMesh, const std::shared_ptr<Texture> aTexture);
	void RenderMesh(const Mesh& aMesh, const std::vector<std::shared_ptr<Material>>& aMaterialList, float aLODHeuristic = 0.0f);
	void RenderMeshShadow(const Mesh& aMesh);
	void RenderMeshDebugPass(const Mesh& aMesh, const std::vector<std::shared_ptr<Material>>& aMaterialList, float aLODHeuristic = 0.0f);
	void RenderInstancedMesh(const Mesh& aMesh, unsigned aMeshCount, const std::vector<std::shared_ptr<Material>>& aMaterialList, DynamicVertexBuffer& aInstanceBuffer, float aLODHeuristic = 0.0f);
	void RenderInstancedMeshShadow(const Mesh& aMesh, unsigned aMeshCount, DynamicVertexBuffer& aInstanceBuffer);
	void RenderInstancedMeshDebugPass(const Mesh& aMesh, unsigned aMeshCount, const std::vector<std::shared_ptr<Material>>& aMaterialList, DynamicVertexBuffer& aInstanceBuffer, float aLODHeuristic = 0.0f);
	void RenderSprite();
	void RenderText(const Text& aText);
	void RenderDebugLines(const DynamicVertexBuffer& aDynamicBuffer, unsigned aLineAmount);
	void RenderParticleEmitter(const ParticleEmitter& aParticleEmitter);
	void RenderTrailEmitter(const TrailEmitter& aTrailEmitter);
};