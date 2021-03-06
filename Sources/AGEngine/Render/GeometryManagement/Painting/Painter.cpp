#include <Render/GeometryManagement/Painting/Painter.hh>

#include <Utils/Debug.hpp>
#include <Threads/ThreadManager.hpp>
#include <Utils/Profiler.hpp>

#include <Utils/StringID.hpp>

namespace AGE
{

	Painter::Painter(std::vector<std::pair<GLenum, StringID>>  const &types) :
		_buffer(types)
		, _isInUniqueDraw(false)
		, _isInstanciedDraw(false)
	{
		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());
	}

	Painter::Painter(Painter &&move) :
		_buffer(std::move(move._buffer)),
		_vertices(std::move(move._vertices))
		, _isInUniqueDraw(std::move(move._isInUniqueDraw))
		, _isInstanciedDraw(std::move(move._isInstanciedDraw))
	{
		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());
	}

	Key<Vertices> Painter::add_vertices(size_t nbrVertex, size_t nbrIndices)
	{
		SCOPE_profile_cpu_function("PainterTimer");

		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());

		auto offset = 0ull;
		for (auto &vertices : _vertices) {
			offset += vertices.nbr_vertex();
		}
		_vertices.emplace_back(_buffer.get_types(), nbrVertex, nbrIndices, offset);
		_buffer.insert(_vertices.back());
		return (Key<Vertices>::createKey(int(_vertices.size()) - 1));
	}

	Painter & Painter::remove_vertices(Key<Vertices> &key)
	{
		SCOPE_profile_cpu_function("PainterTimer");

		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());

		if (!key.isValid())
		{
			return (*this);
		}
		auto iterator = _vertices.begin() + key.getId();
		iterator->remove();
		key.destroy();
		_vertices.erase(iterator);
		auto offset = 0ull;
		for (auto &vertices : _vertices) {
			vertices.reset(offset);
			offset = vertices.nbr_vertex();
		}
		return (*this);
	}

	Vertices * Painter::get_vertices(Key<Vertices> const &key)
	{
		SCOPE_profile_cpu_function("PainterTimer");

		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());

		if (!key.isValid())
		{
			return (nullptr);
		}
		return (&_vertices[key.getId()]);
	}

	Painter & Painter::draw(GLenum mode, std::shared_ptr<Program> const &program, std::vector<Key<Vertices>> const &drawList)
	{
		SCOPE_profile_gpu_i("Draw");
		SCOPE_profile_cpu_function("PainterTimer");
		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());
		program->set_attributes(_buffer);
		_buffer.bind();
		_buffer.update();
		int index = 0;
		for (auto &draw_element : drawList)
		{
			if (draw_element.isValid())
			{
				program->update();
				_vertices[draw_element.getId()].draw(mode);
			}
			++index;
		}
		_buffer.unbind();

		return (*this);
	}

	void Painter::uniqueDraw(GLenum mode, std::shared_ptr<Program> const &program, const Key<Vertices> &vertice)
	{
		//@PROFILER_COMMENTED
		//SCOPE_profile_gpu_i("Unique Draw");
		//SCOPE_profile_cpu_function("PainterTimer");

		// be sure to call uniqueDrawBegin() before and uniqueDrawEnd() after
		AGE_ASSERT(_isInUniqueDraw);

		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());

		program->update();
		// TODO: Fix that properly! @Dorian
		if (vertice.getId() != std::uint32_t(-1) && vertice.getId() < _vertices.size())
			_vertices[vertice.getId()].draw(mode);
	}

	void Painter::instanciedDraw(GLenum mode, std::shared_ptr<Program> const &program, const Key<Vertices> &vertice, std::size_t count)
	{
		//@PROFILER_COMMENTED
		//SCOPE_profile_gpu_i("Instancied Draw");
		//SCOPE_profile_cpu_function("PainterTimer");

		// be sure to call uniqueDrawBegin() before and uniqueDrawEnd() after
		AGE_ASSERT(_isInstanciedDraw);

		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());

		program->update();
		// TODO: Fix that properly! @Dorian
		if (vertice.getId() != std::uint32_t(-1) && vertice.getId() < _vertices.size())
		{
			_vertices[vertice.getId()].instanciedDraw(mode, count);
		}
	}

	void Painter::uniqueDrawBegin(std::shared_ptr<Program> const &program)
	{
		AGE_ASSERT(_isInUniqueDraw == false);

		if (program)
		{
			program->set_attributes(_buffer);
		}

		_buffer.bind();
		_buffer.update();

		_isInUniqueDraw = true;
	}

	void Painter::uniqueDrawEnd()
	{
		AGE_ASSERT(_isInUniqueDraw == true);

		_buffer.unbind();

		_isInUniqueDraw = false;
	}

	void Painter::instanciedDrawBegin(std::shared_ptr<Program> const &program)
	{
		AGE_ASSERT(_isInstanciedDraw == false);

		if (program)
		{
			program->set_attributes(_buffer);
		}

		_buffer.bind();
		_buffer.update();

		_isInstanciedDraw = true;
	}

	void Painter::instanciedDrawEnd()
	{
		AGE_ASSERT(_isInstanciedDraw == true);

		_buffer.unbind();

		_isInstanciedDraw = false;
	}

	bool Painter::coherent(std::vector<std::pair<GLenum, StringID>> const &types) const
	{
		SCOPE_profile_cpu_function("PainterTimer");

		// to be sure that this function is only called in render thread
		AGE_ASSERT(CurrentThread() == (AGE::Thread*)GetRenderThread());

		auto &types_buffer = _buffer.get_types();
		if (types.size() != types_buffer.size()) {
			return (false);
		}
		for (auto index = 0ull; index < types_buffer.size(); ++index) {
			if (types[index] != types_buffer[index]) {
				return (false);
			}
		}
		return (true);
	}

	bool Painter::coherent(std::shared_ptr<Program> const &prog) const
	{
		SCOPE_profile_cpu_function("PainterTimer");

		return (prog->coherent_attributes(_buffer.get_types()));
	}
}