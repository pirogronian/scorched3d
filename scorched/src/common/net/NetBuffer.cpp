////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#include <net/NetBufferPool.h>
#include <common/Logger.h>
#include <string.h>
#include <zlib.h>

static const unsigned startSize = 1024 * 10;

NetBuffer::NetBuffer() : 
	buffer_(0),
	usedSize_(0),
	bufferSize_(0)
{
}

NetBuffer::NetBuffer(const NetBuffer &other) :
	buffer_(0),
	usedSize_(0),
	bufferSize_(0)	
{
	allocate(((NetBuffer &) other).getBufferUsed());
	addToBuffer(((NetBuffer &)other).getBuffer());
}

NetBuffer::NetBuffer(unsigned startSize, void *startBuffer)
{
	allocate(startSize);
	if (startBuffer)
	{
		addDataToBuffer(startBuffer, startSize);
	}
}

NetBuffer::~NetBuffer()
{
	delete [] buffer_;
	buffer_ = 0;
}

void NetBuffer::allocate(unsigned size)
{
	if (bufferSize_<size)
	{
		delete [] buffer_;
		buffer_ = 0;
		buffer_ = new char[size];
		bufferSize_ = size;
	}

	usedSize_ = 0;
}

void NetBuffer::reset()
{
	usedSize_ = 0;
}

void NetBuffer::clear()
{
	delete [] buffer_;
	usedSize_ = 0;
	buffer_ = 0;
	bufferSize_ = 0;
}

bool NetBuffer::compressBuffer()
{
	NetBuffer *newBuffer = NetBufferPool::instance()->getFromPool();

	unsigned long destLen = getBufferUsed() * 2;
	unsigned long srcLen = getBufferUsed();

	// Compress the message into the new buffer
	newBuffer->allocate(destLen);
	newBuffer->reset();
	newBuffer->addToBuffer((unsigned int) srcLen); // First 4 bytes are for uncompressed size
	if (compress2((unsigned char *) (newBuffer->getBuffer() + 4), &destLen, 
				   (unsigned char *) getBuffer(), srcLen, 
				   6) == Z_OK)
	{
		newBuffer->setBufferUsed(destLen + 4);

		// Copy the new buffer into the current buffer
		reset();
		addDataToBuffer(newBuffer->getBuffer(), newBuffer->getBufferUsed());
	}

	NetBufferPool::instance()->addToPool(newBuffer);

	return true;
}

bool NetBuffer::uncompressBuffer()
{
	// Get the size of the uncompressed data
	NetBufferReader reader(*this);
	unsigned int dLen = 0;
	if (!reader.getFromBuffer(dLen)) return false;

	// Create a new buffer for the uncompressed data
	unsigned long srcLen = getBufferUsed() - 4;
	unsigned long destLen = dLen;

#ifdef S3D_SERVER
	if (destLen > 500000)
	{
		if (destLen > 2400000)
		{
			Logger::log(S3D::formatStringBuffer("ERROR: Asked to allocated %u bytes", destLen));
			return false;
		}
		else
		{
			Logger::log(S3D::formatStringBuffer("Warning: Asked to allocated %u bytes", destLen));
		}
	}
#endif

	NetBuffer *newBuffer = NetBufferPool::instance()->getFromPool();
	newBuffer->allocate(destLen);
	newBuffer->setBufferUsed(destLen);

	// Uncompress the data
	if (uncompress((unsigned char *) newBuffer->getBuffer(), &destLen, 
				   (unsigned char *) (getBuffer() + 4), srcLen) == Z_OK)
	{
		// Copy the new buffer into the current buffer
		reset();
		addDataToBuffer(newBuffer->getBuffer(), newBuffer->getBufferUsed());
	}
	NetBufferPool::instance()->addToPool(newBuffer);

	return true;
}

void NetBuffer::resize(unsigned newBufferSize)
{
	if (newBufferSize < startSize) newBufferSize = startSize;
	char *newBuffer = new char[newBufferSize];
	if (buffer_)
	{
		memcpy(newBuffer, buffer_, usedSize_);
		delete [] buffer_;
	}
	buffer_ = newBuffer;
	bufferSize_ = newBufferSize;
}

void NetBuffer::addDataToBuffer(const void *add, unsigned len)
{
	unsigned bufferLeft = bufferSize_ - usedSize_;
	if (!buffer_ || (bufferLeft < len))
	{
		unsigned sizeNeeded = len + usedSize_;
		unsigned newBufferSize = sizeNeeded * 2;

		resize(newBufferSize);
	}

	memcpy(&buffer_[usedSize_], add, len);
	usedSize_ += len;
}

void NetBuffer::addToBuffer(Vector &add)
{
	addToBuffer(add[0]);
	addToBuffer(add[1]);
	addToBuffer(add[2]);
}

void NetBuffer::addToBuffer(FixedVector &add)
{
	addToBuffer(add[0]);
	addToBuffer(add[1]);
	addToBuffer(add[2]);
}

void NetBuffer::addToBuffer(FixedVector4 &add)
{
	addToBuffer(add[0]);
	addToBuffer(add[1]);
	addToBuffer(add[2]);
	addToBuffer(add[3]);
}

void NetBuffer::addToBuffer(const fixed add)
{
	Uint32 value = 0;
	SDLNet_Write32(fixed(add).getInternal(), &value);
	addDataToBuffer(&value, sizeof(Uint32));
}

void NetBuffer::addToBuffer(const int add)
{
	Uint32 value = 0;
	SDLNet_Write32(add, &value);
	addDataToBuffer(&value, sizeof(Uint32));
}

void NetBuffer::addToBuffer(const float addf)
{
	Uint32 value = 0;
	Uint32 add = 0;
	memcpy(&add, &addf, sizeof(Uint32));
	SDLNet_Write32(add, &value);
	addDataToBuffer(&value, sizeof(Uint32));
}

void NetBuffer::addToBuffer(const bool add)
{
	char c = (add?'1':'0');
	addDataToBuffer(&c, sizeof(char));
}

void NetBuffer::addToBuffer(const char add)
{
	addDataToBuffer(&add, sizeof(char));
}

void NetBuffer::addToBuffer(const unsigned int add)
{
	Uint32 value = 0;
	SDLNet_Write32(add, &value);
	addDataToBuffer(&value, sizeof(Uint32));
}

void NetBuffer::addToBuffer(const char *add)
{
	addDataToBuffer(add, (unsigned) strlen(add)+1);
}

void NetBuffer::addToBuffer(const std::string &string)
{
	addToBuffer(string.c_str());
}

void NetBuffer::addToBuffer(std::string &string)
{
	addToBuffer(string.c_str());
}

void NetBuffer::addToBuffer(NetBuffer &add)
{
	addToBuffer(add.getBufferUsed());
	addDataToBuffer(add.getBuffer(), add.getBufferUsed());
}

NetBufferReader::NetBufferReader() :
	buffer_(0),
	bufferSize_(0),
	readSize_(0)
{
}

NetBufferReader::NetBufferReader(NetBuffer &buffer) :
	buffer_(buffer.getBuffer()),
	bufferSize_(buffer.getBufferUsed()),
	readSize_(0)
{
}

NetBufferReader::~NetBufferReader()
{
}

void NetBufferReader::reset()
{
	readSize_ = 0;
}

bool NetBufferReader::getFromBuffer(Vector &result)
{
	if (!getFromBuffer(result[0])) return false;
	if (!getFromBuffer(result[1])) return false;
	if (!getFromBuffer(result[2])) return false;
	return true;
}

bool NetBufferReader::getFromBuffer(FixedVector &result)
{
	if (!getFromBuffer(result[0])) return false;
	if (!getFromBuffer(result[1])) return false;
	if (!getFromBuffer(result[2])) return false;
	return true;
}

bool NetBufferReader::getFromBuffer(FixedVector4 &result)
{
	if (!getFromBuffer(result[0])) return false;
	if (!getFromBuffer(result[1])) return false;
	if (!getFromBuffer(result[2])) return false;
	if (!getFromBuffer(result[3])) return false;
	return true;
}

bool NetBufferReader::getFromBuffer(int &result)
{
	Uint32 value = 0;
	if (!getDataFromBuffer(&value, sizeof(value))) return false;
	result = SDLNet_Read32(&value);
	return true;
}

bool NetBufferReader::getFromBuffer(fixed &result)
{
	Uint32 value = 0;
	if (!getDataFromBuffer(&value, sizeof(value))) return false;
	result = fixed(true, SDLNet_Read32(&value));
	return true;
}

bool NetBufferReader::getFromBuffer(float &resultf)
{
	Uint32 value = 0;
	if (!getDataFromBuffer(&value, sizeof(value))) return false;
	Uint32 result = SDLNet_Read32(&value);
	memcpy(&resultf, &result, sizeof(Uint32));
	return true;
}

bool NetBufferReader::getFromBuffer(bool &result)
{
	char c = 0;
	if (!getDataFromBuffer(&c, sizeof(c))) return false;
	result = (c=='1'?true:false);
	return true;
}

bool NetBufferReader::getFromBuffer(char &result)
{
	if (!getDataFromBuffer(&result, sizeof(result))) return false;
	return true;
}

bool NetBufferReader::getFromBuffer(unsigned int &result)
{
	Uint32 value = 0;
	if (!getDataFromBuffer(&value, sizeof(value))) return false;
	result = SDLNet_Read32(&value);
	return true;
}

bool NetBufferReader::getFromBuffer(std::string &result, bool safe)
{
	int i;
	for (i=0; buffer_[readSize_ + i]; i++);
	i++;

	char *value = new char[i+1]; // Another one just in case
	if (getDataFromBuffer(value, i))
	{
		if (safe)
		{
			for (int j=0; j<i; j++)
			{
				if (value[j] == '%') value[j] = ' ';
			}
		}

		result = value;
		delete [] value;

		return true;
	}

	delete [] value;
	return false;
}

bool NetBufferReader::getFromBuffer(NetBuffer &buffer)
{
	buffer.reset();
	unsigned int bufferSize = 0;
	if (!getFromBuffer(bufferSize)) return false;
	buffer.resize(bufferSize);
	buffer.setBufferUsed(bufferSize);
	return getDataFromBuffer(buffer.getBuffer(), bufferSize);
}

bool NetBufferReader::getDataFromBuffer(void *dest, int len)
{
	unsigned bufferLeft = bufferSize_ - readSize_;
	if (bufferLeft < (unsigned) len) return false;

	memcpy(dest, &buffer_[readSize_], len);
	readSize_ += len;
	return true;
}
