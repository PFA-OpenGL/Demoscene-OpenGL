#include <Utils/Profiler.hpp>
#include <Utils/StringID.hpp>
#include <Render/GeometryManagement/Buffer/BufferPrograms.hh>
#include <Render/Buffer/VertexBuffer.hh>
#include <Render/Buffer/IndexBuffer.hh>
#include <Render/ProgramResources/Types/ProgramResourcesType.hh>
#include <Render/Program.hh>
#include <memory>

namespace AGE
{
	BufferPrograms::BufferPrograms(std::vector<std::pair<GLenum, StringID>> const &types) :
		_types(types),
		_indices_buffer(StringID("indices", 0x144d56ffb313be92), std::make_unique<IndexBuffer>())
	{
		_buffers.reserve(_types.size());
		_vertex_array.bind();
		auto index = 0ull;
		for (auto &type : _types) {
			auto &iterator = available_types.find(type.first);
			if (iterator != available_types.end()) {
				auto &a = iterator->second;
				_buffers.emplace_back(std::make_shared<Buffer>(StringID(type.second), std::make_unique<VertexBuffer>()));
			}
		}
		_indices_buffer.bind();
		_vertex_array.unbind();
	}

	BufferPrograms::BufferPrograms(BufferPrograms &&move) :
		_types(std::move(move._types)),
		_buffers(std::move(move._buffers)),
		_indices_buffer(std::move(move._indices_buffer)),
		_vertex_array(std::move(move._vertex_array))
	{
	}

	bool BufferPrograms::insert(Vertices &vertices)
	{
		SCOPE_profile_cpu_function("RenderTimer");
		if (vertices.nbr_buffer() != _types.size()) {
			return (false);
		}
		for (auto index = 0ull; index < vertices.nbr_buffer(); ++index) {
			if (_types[index] != vertices.get_type(index)) {
				return (false);
			}
		}
		for (auto &buffer_targeted : _buffers) {
			vertices.set_block_memory(buffer_targeted->push_back(vertices.transfer_data(buffer_targeted->name())), buffer_targeted->name());
		}
		vertices.set_indices_block_memory(_indices_buffer.push_back(vertices.transfer_indices_data()));
		return (true);
	}

	size_t BufferPrograms::size() const
	{
		return (_buffers.size());
	}

	BufferPrograms & BufferPrograms::bind()
	{
		SCOPE_profile_cpu_function("RenderTimer");
		_vertex_array.bind();
		return (*this);
	}

	BufferPrograms & BufferPrograms::unbind()
	{
		SCOPE_profile_cpu_function("RenderTimer");
		_vertex_array.unbind();
		return (*this);
	}

	BufferPrograms & BufferPrograms::update()
	{
		SCOPE_profile_cpu_function("RenderTimer");
		for (auto &buffer : _buffers) {
			buffer->update();
		}
		_indices_buffer.update();
		return (*this);
	}

	BufferPrograms & BufferPrograms::clear()
	{
		SCOPE_profile_cpu_function("RenderTimer");
		for (auto &buffer : _buffers) {
			buffer->clear();
		}
		_indices_buffer.clear();
		return (*this);
	}

	std::vector<std::pair<GLenum, StringID>> const & BufferPrograms::get_types() const
	{
		return (_types);
	}

	std::vector<std::shared_ptr<Buffer>> const &BufferPrograms::get_buffers() const
	{
		return _buffers;
	}

}
