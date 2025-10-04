#include <memory>
#include <vector>

class Disk;
using DiskPtr = std::shared_ptr<Disk>;

#ifndef DISK_H
#define DISK_H

#include "shape.h"
#include "state.h"

class Disk: public Shape
{
	unsigned int m_vao;
	std::vector<float> coord = {};
	std::vector<unsigned char> color = {};

protected:
	Disk(int num_segments);
public:
	static DiskPtr Make(int num_segments);
	virtual ~Disk();
	virtual void Draw(StatePtr state);

};

#endif // DISK_H

