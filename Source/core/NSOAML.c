#include "NSOACPI.h"
#include "NSOAllocator.h"
#include "NSOBochs.h"
#include "NSOCoreUtils.h"
#include "NSOStringUtils.h"

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
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////
static struct kernel_ACPIScope* _kernel_createScope(const char*);
static void _kernel_createOperationRegion(struct _kernel_StreamBuffer*, struct kernel_ACPIScope*);
static void _kernel_appendToChildScope(struct kernel_ACPIScope*, struct kernel_ACPIScope*);
static struct kernel_ACPIScope* _kernel_getLastScope(struct kernel_ACPIScope*);
static u8 _kernel_isRootNamespace(struct kernel_ACPIScope*);
static struct kernel_ACPIScope* _kernel_loadStringToObject(struct kernel_ACPIScope*, const char*);
static struct kernel_ACPIScope* _kernel_loadIntegerToObject(struct kernel_ACPIScope*, u64);
static void _kernel_debugACPIObject(struct kernel_ACPIScope*);
static u32 _kernel_extractPkgLength(struct _kernel_StreamBuffer*);
static char* _kernel_extractNamepath(struct _kernel_StreamBuffer*);
//////////////////////////////////////////////
static void _kernel_parseTermList(struct kernel_ACPIScope*, struct _kernel_StreamBuffer*);
static void _kernel_parseTerm(struct kernel_ACPIScope*, struct _kernel_StreamBuffer*);

///////////////////////////////////////////////////
u8 kernel_loadAML(struct kernel_ACPIScope* rootNamespace, void* aml){	
	struct kernel_ACPIHeader* header = (struct kernel_ACPIHeader*)aml;

	if (kernel_calculateChecksum(header, header->length) != 0) return 0;

	struct _kernel_StreamBuffer r_buffer = {.source = aml, .position = sizeof(struct kernel_ACPIHeader), .size = header->length};
	struct _kernel_StreamBuffer* buffer = &r_buffer;

	_kernel_parseTermList(rootNamespace, aml);
	return 1;
}

void kernel_preloadACPIRoot(struct kernel_ACPIScope* rootNamespace){
	_kernel_appendToChildScope(rootNamespace, _kernel_createScope("_GPE"));
	_kernel_appendToChildScope(rootNamespace, _kernel_createScope("_PR_"));
	_kernel_appendToChildScope(rootNamespace, _kernel_createScope("_SB_"));
	_kernel_appendToChildScope(rootNamespace, _kernel_createScope("_SI_"));
	_kernel_appendToChildScope(rootNamespace, _kernel_createScope("_TZ_"));

	//predefined objects
	_kernel_appendToChildScope(rootNamespace, _kernel_loadStringToObject(_kernel_createScope("_OS_"), KERNEL_OSNAME));
	_kernel_appendToChildScope(rootNamespace, _kernel_loadIntegerToObject(_kernel_createScope("_REV"), kernel_getACPIRevision()));

}

struct kernel_ACPIScope* kernel_searchScope(struct kernel_ACPIScope* namespace, const char* path){
	if (namespace == NULL) return NULL;
	char* tpath = (char*)path;
	char* end = tpath + kernel_stringLength(tpath);

	struct kernel_ACPIScope* currentNamespace = namespace, *childNamepaces = NULL, *temp = NULL;

	if (*tpath == '\\'){
		while (!_kernel_isRootNamespace(currentNamespace))
			currentNamespace = currentNamespace->parentScope;
		tpath++;
	}else{
		while (*tpath == '^'){
			currentNamespace = currentNamespace->parentScope;
			tpath++;
		}
	}

	while (tpath != end){
		
		childNamepaces = currentNamespace->childScope;
		temp = childNamepaces;

		while (temp != NULL){
			if (kernel_stringCompareRAW(temp->name, tpath, 4)){
				currentNamespace = temp;
				tpath += 4;
				break;
			}
			temp = temp->nextScope;
		}

		if (temp == NULL)
			return NULL;
	}
	return currentNamespace;
}

static void _kernel_parseTermList(struct kernel_ACPIScope* scope, struct _kernel_StreamBuffer* buffer){
	while (!_kernel_isBufferDone(buffer)){
		_kernel_parseTerm(scope, buffer);
	}
}

static void _kernel_parseTerm(struct kernel_ACPIScope* scope, struct _kernel_StreamBuffer* buffer){
	u8 starter = _kernel_readBuffer8(buffer, 0);

	switch (starter){
		case 0x10:
			//_kernel_parseScope(scope, buffer);
			break;
		case 0x06:
			//_kernel_parseAlias(scope, buffer);
			break;
	}
}

static void _kernel_parseScope(struct kernel_ACPIScope* scope, struct _kernel_StreamBuffer* buffer){
	_kernel_advanceBuffer(buffer, 1);
	u32 length = _kernel_extractPkgLength(buffer);
	
	char* namepath = _kernel_extract
	
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
}

static char* _kernel_extractNamepath(struct _kernel_StreamBuffer* buffer){
	u32 savedPos = buffer->position;
	if (_kernel_checkBuffer8(buffer, 0x5C))
		_kernel_advanceBuffer(buffer, 1);
	else if (_kernel_checkBuffer(buffer, 0x5E)){
		_kernel_advanceBuffer(buffer, 1);
		while (_kernel_checkBuffer(buffer, 0x5E))
			_kernel_advanceBuffer(buffer, 1);
	}
	
	if (_kernel_checkBuffer(buffer, 0x2E)){
		_kernel_advanceBuffer(buffer, 9);
	else if (_kernel_checkBuffer(buffer, 0x2F)){
		_kernel_advanceBuffer(buffer, 1);
		
		
}

static struct kernel_ACPIScope* _kernel_createScope(const char* name){
	struct kernel_ACPIScope* namespace = kernel_malloc(sizeof(struct kernel_ACPIScope), 4);

	if (namespace != NULL){
		kernel_memset(namespace, 0, sizeof(struct kernel_ACPIScope));
		kernel_memcpy(namespace->name, name, 4);
		namespace->type = KERNEL_AML_UNINITALIZED;
	}

	return namespace;
}

static struct kernel_ACPIScope* _kernel_loadStringToObject(struct kernel_ACPIScope* scope, const char* string){
	scope->type = KERNEL_AML_STRING;
	scope->data.string = string;
	return scope;
}
static struct kernel_ACPIScope* _kernel_loadIntegerToObject(struct kernel_ACPIScope* scope, u64 integer){
	scope->type = KERNEL_AML_INTEGER;
	scope->data.integer = integer;
	return scope;
}



void kernel_debugACPITree(struct kernel_ACPIScope* scope){
	struct kernel_ACPIScope* temp = scope, *childTemp = scope->childScope;
	char tempStr[5] = {0};
	
	kernel_memcpy(tempStr, scope->name, 4);
	kernel_printfBOCHS("{\n\tName: %s\n\tType: %x\n\tValue: ", tempStr, temp->type);
	_kernel_debugACPIObject(scope);
	kernel_printStringBOCHS("\n}\n");
	while (childTemp != NULL){
		kernel_printfBOCHS("\nchild of \"%s\":\n", tempStr);
		//kernel_printfBOCHS("child scope: %x\n", (u32)childTemp);
		kernel_debugACPITree(childTemp);
		childTemp = childTemp->nextScope;
	}
}


static void _kernel_appendToChildScope(struct kernel_ACPIScope* parent, struct kernel_ACPIScope* child){
	child->parentScope = parent;
	child->SDT = parent->SDT;

	if (parent->childScope == NULL)
		parent->childScope = child;
	else { 
		struct kernel_ACPIScope* lastScope = _kernel_getLastScope(parent->childScope);
		lastScope->nextScope = child;
	}
}

static void _kernel_debugACPIObject(struct kernel_ACPIScope* scope){
	switch (scope->type){
		case KERNEL_AML_STRING:
			kernel_printStringBOCHS(scope->data.string);
			break;
		case KERNEL_AML_INTEGER:
			kernel_printUNumberBOCHS((u32)scope->data.integer);
			break;
		default:
			break;
	}
}

static struct kernel_ACPIScope* _kernel_getLastScope(struct kernel_ACPIScope* scope){
	struct kernel_ACPIScope* temp = scope;

	while (temp->nextScope != NULL)
		temp = temp->nextScope;

	return temp;
}

static inline u8 _kernel_isRootNamespace(struct kernel_ACPIScope* namespace){
	return namespace->parentScope == NULL;
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

