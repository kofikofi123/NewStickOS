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

static u8 _kernel_readBuffer8(struct _kernel_StreamBuffer*);
static u16 _kernel_readBuffer16(struct _kernel_StreamBuffer*);
static u32 _kernel_readBuffer32(struct _kernel_StreamBuffer*);
static void _kernel_advanceBuffer(struct _kernel_StreamBuffer*, u32);
static u8 _kernel_checkBuffer8(struct _kernel_StreamBuffer*, u8);
static u8 _kernel_checkBuffer16(struct _kernel_StreamBuffer*, u16);
static u8 _kernel_checkBuffer32(struct _kernel_StreamBuffer*, u32);
static u8 _kernel_isBufferDone(struct _kernel_StreamBuffer*);

static u8 _kernel_parseTermlist(struct _kernel_StreamBuffer*, struct kernel_ACPINamespace*);
static u8 _kernel_parseTerm(struct _kernel_StreamBuffer*, struct kernel_ACPINamespace*);
static void _kernel_createScopeObject(struct _kernel_StreamBuffer*, struct kernel_ACPINamespace*);
static u32 _kernel_extractPkgLength(struct _kernel_StreamBuffer*);

u8 kernel_loadAML(struct kernel_ACPINamespace* rootNamepace, void* aml){	
	struct kernel_ACPIHeader* header = (struct kernel_ACPIHeader*)aml;

	if (kernel_calculateChecksum(header, header->length) != 0) return 0;

	struct _kernel_StreamBuffer r_buffer = {.source = aml, .position = sizeof(struct kernel_ACPIHeader), .size = header->length};
	struct _kernel_StreamBuffer* buffer = &r_buffer;

	_kernel_parseTermlist(buffer, rootNamepace);
}

static u8 _kernel_parseTermlist(struct _kernel_StreamBuffer* buffer, struct kernel_ACPINamespace* namespace){

	if (!_kernel_parseTerm(buffer, namespace)){
		if (_kernel_isBufferDone(buffer)) return 1;
	}
	return 0;
}

static u8 _kernel_parseTerm(struct _kernel_StreamBuffer* buffer, struct kernel_ACPINamespace* namespace){
	u8 byte = _kernel_readBuffer8(buffer);

	if (byte == 0x10){
		_kernel_createScopeObject(buffer, namespace);
		return 1;
	}
}

static void _kernel_createScopeObject(struct _kernel_StreamBuffer* buffer, struct kernel_ACPINamespace* namespace){
	_kernel_advanceBuffer(buffer, 1);

	u32 length = _kernel_extractPkgLength(buffer);
	u32 temp = buffer->size;
	buffer->size = length;
	/////////////////////////
	const char* namestring = _kernel_extractNamestring(buffer);
	////////////////////////
	buffer->size = temp;

}

static const char* _kernel_extractNamestring(struct _kernel_StreamBuffer* buffer){
	u32 tempPos = buffer->position;

	char mainName[4] = {0};
	
	if (_kernel_checkBuffer8(buffer, 0x5C)){

	}
}

static u32 _kernel_extractPkgLength(struct _kernel_StreamBuffer* buffer){
	u32 length = 0;
	u8 temp = _kernel_readBuffer8(buffer);
	u8 numBytes = (temp >> 6);
	_kernel_advanceBuffer(buffer, 1);

	if (numBytes == 0)
		length = temp & 0x3F;
	else {
		length = temp & 0x0F;
		for (u8 i = 0; i < numBytes; i++){
			length <<= 8;
			length |= _kernel_readBuffer8(buffer);
			_kernel_advanceBuffer(buffer, 1);
		}
	}

	length -= (numBytes + 1);

	return length;
}

static inline u8 _kernel_readBuffer8(struct _kernel_StreamBuffer* buffer){
	return *(u8*)(buffer->source + buffer->position);
}

static inline u16 _kernel_readBuffer16(struct _kernel_StreamBuffer* buffer){
	return *(u16*)(buffer->source + buffer->position);
}

static inline u32 _kernel_readBuffer32(struct _kernel_StreamBuffer* buffer){
	return *(u32*)(buffer->source + buffer->position);
}

static void _kernel_advanceBuffer(struct _kernel_StreamBuffer* buffer, u32 size){
	buffer->position += size;
	buffer->size -= size;
}

static u8 _kernel_checkBuffer8(struct _kernel_StreamBuffer* buffer, u8 value){
	return _kernel_isBufferDone(buffer) ? (_kernel_readBuffer8(buffer) == value) : 0;
}

static u8 _kernel_checkBuffer16(struct _kernel_StreamBuffer* buffer, u16 value){
	return _kernel_isBufferDone(buffer) ? (_kernel_readBuffer16(buffer) == value) : 0;
}

static u8 _kernel_checkBuffer32(struct _kernel_StreamBuffer* buffer, u32 value){
	return _kernel_isBufferDone(buffer) ? (_kernel_readBuffer32(buffer) == value) : 0;
}

static u8 _kernel_isBufferDone(struct _kernel_StreamBuffer* buffer){
	return (buffer->size == 0);
}