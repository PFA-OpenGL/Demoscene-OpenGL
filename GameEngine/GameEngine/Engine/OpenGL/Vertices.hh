#ifndef VERTICE_HH_
# define VERTICE_HH_

#define WARNING_MESSAGE_INDEX(in) \
	std::cerr << "Warning: the index to get something into " << in << " is out of range." << std::endl;\

#include <stdint.h>
#include <OpenGL/VerticesManager.hh>
#include <OpenGL/Pool.hh>

namespace gl
{
	class MemoryBlocksGPU;
	class Vertices;
	template<typename TYPE> class Key;

	//!\file VerticesManager.hh
	//!\author Dorian Pinaud
	//!\version v1.0
	//!\class Vertices
	//!\brief contain vertices information
	class Vertices
	{
	public:
		Vertices();
		Vertices(size_t nbrVertices, uint8_t nbrBuffers, size_t *sizeBuffers, void **buffers);
		~Vertices();
		Vertices(Vertices const &copy);
		Vertices &operator=(Vertices const &v);

		void const *getBuffer(uint8_t index) const;
		size_t getSizeBuffer(uint8_t index) const;
		uint8_t getNbrBuffers() const;
		size_t getNbrVertices() const;

		Vertices &setPool(VerticesPool const *pool);
		VerticesPool const *getPool() const;

	private:
		void **_buffers;
		size_t *_sizeBuffers;
		size_t _nbrVertices;
		uint8_t _nbrBuffers;
		
		size_t _indexOnPool;
		VerticesPool *_pool;

		METHODE_ACCESS
	};

}

#endif /* !VERTICE_HH_ */