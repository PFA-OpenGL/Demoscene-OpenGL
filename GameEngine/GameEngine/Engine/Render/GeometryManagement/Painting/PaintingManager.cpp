#include <Render/GeometryManagement/Painting/PaintingManager.hh>
#include <Render/GeometryManagement/Painting/Painter.hh>

PaintingManager::PaintingManager()
{

}

PaintingManager::PaintingManager(PaintingManager &&move) :
_painters(std::move(move._painters))
{

}



Key<Painter> PaintingManager::add_painter(std::vector<GLenum> &&types)
{
	_painters.emplace_back(types);
	return (Key<Painter>::createKey(_painters.size() - 1));
}

std::shared_ptr<Painter> const & PaintingManager::get_painter(Key<Painter> const &key) const
{
	return (_painters[key.getId()]);
}

Key<Painter> PaintingManager::get_painter(std::vector<GLenum> const &types) const
{
	for (auto index = 0ull; index < _painters.size(); ++index) {
		if (_painters[index]->coherent(types)) {
			return (Key<Painter>::createKey(index));
		}
	}
	return (Key<Painter>::createKey(-1));
}

bool PaintingManager::has_painter(std::vector<GLenum> const types) const
{
	for (auto &painter : _painters) {
		if (painter->coherent(types)) {
			return (true);
		}
	}
	return (false);
}



