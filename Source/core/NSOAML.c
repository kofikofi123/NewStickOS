#include "NSOACPI.h"
#include "NSOAllocator.h"
#include "NSOBochs.h"
#include "NSOCoreUtils.h"

//static void _kernel_resetBuffer(struct _kernel_StreamBuffer*, void*);

struct _kernel_StreamBuffer {
	void* source;
	u32 position;
	u32 size;
};


static u8 _kernel_readBuffer8(struct _kernel_StreamBuffer*, u32);
static u16 _kernel_readBuffer16(struct _kernel_StreamBuffer*, u32);
static u32 _kernel_readBuffer32(struct _kernel_StreamBuffer*, u32);
static void _kernel_advanceBuffer(struct _kernel_StreamBuffer*, u32);
static u8 _kernel_checkBuffer8(struct _kernel_StreamBuffer*, u8, u32);
static u8 _kernel_checkBuffer16(struct _kernel_StreamBuffer*, u16, u32);
static u8 _kernel_checkBuffer32(struct _kernel_StreamBuffer*, u32, u32);
static u8 _kernel_isBufferDone(struct _kernel_StreamBuffer*);

static u8 _kernel_parseTermlist(struct _kernel_StreamBuffer*, struct kernel_ACPIObject*);
static u8 _kernel_parseTerm(struct _kernel_StreamBuffer*, struct kernel_ACPIObject*);
static void _kernel_createScopeObject(struct _kernel_StreamBuffer*, struct kernel_ACPIObject*);
static u32 _kernel_extractPkgLength(struct _kernel_StreamBuffer*);
static const char* _kernel_extractNamestring(struct _kernel_StreamBuffer*);



u8 kernel_loadAML(struct kernel_ACPIObject* rootNamepace, void* aml){	
	struct kernel_ACPIHeader* header = (struct kernel_ACPIHeader*)aml;

	if (kernel_calculateChecksum(header, header->length) != 0) return 0;

	struct _kernel_StreamBuffer r_buffer = {.source = aml, .position = sizeof(struct kernel_ACPIHeader), .size = header->length};
	struct _kernel_StreamBuffer* buffer = &r_buffer;

	_kernel_parseTermlist(buffer, rootNamepace);

	return 1;
}

static u8 _kernel_parseTermlist(struct _kernel_StreamBuffer* buffer, struct kernel_ACPIObject* namespace){

	if (!_kernel_parseTerm(buffer, namespace)){
		if (_kernel_isBufferDone(buffer)) return 1;
	}
	return 0;
}

static u8 _kernel_parseTerm(struct _kernel_StreamBuffer* buffer, struct kernel_ACPIObject* namespace){
	u8 byte = _kernel_readBuffer8(buffer, 0);

	if (byte == 0x10){
		_kernel_createScopeObject(buffer, namespace);
		return 1;
	}

	return 0;
}

static void _kernel_createScopeObject(struct _kernel_StreamBuffer* buffer, struct kernel_ACPIObject* namespace){
	_kernel_advanceBuffer(buffer, 1);

	u32 length = _kernel_extractPkgLength(buffer);
	u32 temp = buffer->size;
	buffer->size = length;
	/////////////////////////
	const char* namestring = _kernel_extractNamestring(buffer);
	kernel_printfBOCHS("Name: %s\n", namestring);

	if (kernel_stringCompare(namestring, "\\")){
		kernel_printfBOCHS("Is namespace root\n");
	}

	////////////////////////
	buffer->size = temp;

}


static const char* _kernel_extractNamestring(struct _kernel_StreamBuffer* buffer){
	u32 sizeToAlloc = 1;
	u32 numChars = 0;

	char prefix = 0;

	u32 numOfPrefix = 0;

	if (_kernel_checkBuffer8(buffer, 0x5C, 0)){
		sizeToAlloc++;
		numOfPrefix++;
		prefix = 0x5C;
		_kernel_advanceBuffer(buffer, 1);
	}else if (_kernel_checkBuffer8(buffer, 0x5E, 0)){
		prefix = 0x5E;
		sizeToAlloc++;
		numOfPrefix++;
		_kernel_advanceBuffer(buffer, 1);
		while (_kernel_checkBuffer8(buffer, 0x5E, 0)){
			sizeToAlloc++;
			numOfPrefix++;
			_kernel_advanceBuffer(buffer, 1);
		}
	}


	if (_kernel_checkBuffer8(buffer, 0x2E, 0)){
		numChars += 10;
		_kernel_advanceBuffer(buffer, 1);
	}else if (_kernel_checkBuffer8(buffer, 0x2F, 0)){
		_kernel_advanceBuffer(buffer, 1);
		numChars += (_kernel_readBuffer8(buffer, 0) * 5);
		_kernel_advanceBuffer(buffer, 1);
	}

	if (!_kernel_checkBuffer8(buffer, 0, 0)){
		numChars += 4;
	}

	sizeToAlloc += numChars;

	char* path = kernel_malloc(sizeToAlloc, 1);
	char* tempPath = path;
	kernel_memset(path, 0, sizeToAlloc);
	kernel_memset(path, prefix, numOfPrefix);

	u32 ftemp = 0;

	if (numChars > 0){
		for (u32 i = 0; i < numOfPrefix; i += 4, *tempPath++ = '.'){
			ftemp = _kernel_readBuffer32(buffer, 0);
			kernel_memcpy(tempPath, &ftemp, 4);
			ftemp += 4;
		}
	}
	return path;
}


static u32 _kernel_extractPkgLength(struct _kernel_StreamBuffer* buffer){
	u32 length = 0;
	u8 temp = _kernel_readBuffer8(buffer, 0);
	u8 numBytes = (temp >> 6);
	_kernel_advanceBuffer(buffer, 1);

	if (numBytes == 0)
		length = temp & 0x3F;
	else {
		length = temp & 0x0F;
		for (u8 i = 0; i < numBytes; i++){
			length <<= 8;
			length |= _kernel_readBuffer8(buffer, 0);
			_kernel_advanceBuffer(buffer, 1);
		}
	}

	length -= (numBytes + 1);

	return length;
}


static inline u8 _kernel_readBuffer8(struct _kernel_StreamBuffer* buffer, u32 lookahead){
	return *(u8*)(buffer->source + buffer->position + lookahead);
}

static inline u16 _kernel_readBuffer16(struct _kernel_StreamBuffer* buffer, u32 lookahead){
	return *(u16*)(buffer->source + buffer->position + lookahead);
}

static inline u32 _kernel_readBuffer32(struct _kernel_StreamBuffer* buffer, u32 lookahead){
	return *(u32*)(buffer->source + buffer->position + lookahead);
}

static void _kernel_advanceBuffer(struct _kernel_StreamBuffer* buffer, u32 size){
	buffer->position += size;
	buffer->size -= size;
}

static u8 _kernel_checkBuffer8(struct _kernel_StreamBuffer* buffer, u8 value, u32 lookahead){
	return _kernel_isBufferDone(buffer) ? 0 : (_kernel_readBuffer8(buffer, lookahead) == value);
}

static u8 _kernel_checkBuffer16(struct _kernel_StreamBuffer* buffer, u16 value, u32 lookahead){
	return _kernel_isBufferDone(buffer) ? 0 : (_kernel_readBuffer16(buffer, lookahead) == value);
}

static u8 _kernel_checkBuffer32(struct _kernel_StreamBuffer* buffer, u32 value, u32 lookahead){
	return _kernel_isBufferDone(buffer) ? 0 : (_kernel_readBuffer32(buffer, lookahead) == value);
}

static u8 _kernel_isBufferDone(struct _kernel_StreamBuffer* buffer){
	return (buffer->size == 0);
}