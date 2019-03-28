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


static u8 _kernel_readBuffer8(struct _kernel_StreamBuffer*);
static u16 _kernel_readBuffer16(struct _kernel_StreamBuffer*);
static u32 _kernel_readBuffer32(struct _kernel_StreamBuffer*);
static u64 _kernel_readBuffer64(struct _kernel_StreamBuffer*);
static void _kernel_advanceBuffer(struct _kernel_StreamBuffer*, u32);
static u8 _kernel_checkBuffer8(struct _kernel_StreamBuffer*, u8);
static u8 _kernel_checkBuffer16(struct _kernel_StreamBuffer*, u16);
static u8 _kernel_checkBuffer32(struct _kernel_StreamBuffer*, u32);
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
static struct kernel_ACPIScope* _kernel_loadAliasRefToObject(struct kernel_ACPIScope*, struct kernel_ACPIScope*);
static struct kernel_ACPIScope* _kernel_loadOperationRegionToObject(struct kernel_ACPIScope*, u8, u64, u64);

static void _kernel_debugACPIObject(struct kernel_ACPIScope*);
/////////////////////////////////////////////////////////////////////
static u32 _kernel_extractPkgLength(struct _kernel_StreamBuffer*);
static char* _kernel_extractNamestring(struct _kernel_StreamBuffer*);
static u64 _kernel_extractTermArgInteger(struct _kernel_StreamBuffer*,  struct kernel_ACPIHeader*);
static char* _kernel_getNameFromPath(char*);
//////////////////////////////////////////////
static void _kernel_parseTermList(struct kernel_ACPIScope*, struct _kernel_StreamBuffer*);
static void _kernel_parseTerm(struct kernel_ACPIScope*, struct _kernel_StreamBuffer*);
static void _kernel_parseScope(struct kernel_ACPIScope*, struct _kernel_StreamBuffer*);
static void _kernel_parseAlias(struct kernel_ACPIScope*, struct _kernel_StreamBuffer*);
static void _kernel_parseOperationRegion(struct kernel_ACPIScope*, struct _kernel_StreamBuffer*);
static void _kernel_parseOpcode(struct kernel_ACPIScope*, struct _kernel_StreamBuffer*);
static u8 _kernel_getObjectType(struct kernel_ACPIScope*);

///////////////////////////////////////////////////
u8 kernel_loadAML(struct kernel_ACPIScope* rootNamespace, void* aml){	
	struct kernel_ACPIHeader* header = (struct kernel_ACPIHeader*)aml;

	if (kernel_calculateChecksum(header, header->length) != 0) return 0;

	struct _kernel_StreamBuffer r_buffer = {.source = aml, .position = sizeof(struct kernel_ACPIHeader), .size = header->length};
	struct _kernel_StreamBuffer* buffer = &r_buffer;


	_kernel_parseTermList(rootNamespace, buffer);
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
		kernel_printfBOCHS("[Tree check]\n");
		kernel_debugACPITree(scope);
	}
}

static void _kernel_parseTerm(struct kernel_ACPIScope* scope, struct _kernel_StreamBuffer* buffer){
	u8 starter = _kernel_readBuffer8(buffer);


	switch (starter){
		case 0x10:
			kernel_printfBOCHS("Parsing scope entry\n");
			_kernel_parseScope(scope, buffer);
			break;
		case 0x06:
			_kernel_parseAlias(scope, buffer);
			break;
		case 0x5B:
			_kernel_parseOpcode(scope, buffer);
			break;
		default: 
			kernel_printfBOCHS("Unknown opcode: %x\n", starter);
			kernel_panic("stop here");
			break;
	}
}

static void _kernel_parseOpcode(struct kernel_ACPIScope* scope, struct _kernel_StreamBuffer* buffer){
	kernel_printfBOCHS("Parsing opcode\n");
	_kernel_advanceBuffer(buffer, 1);

	u8 opcode = _kernel_readBuffer8(buffer);

	switch (opcode){
		case 0x80:
			_kernel_parseOperationRegion(scope, buffer);
			break;
		default:
			kernel_panic("stop here 2");
			break;
	}
}


static void _kernel_parseOperationRegion(struct kernel_ACPIScope* scope, struct _kernel_StreamBuffer* buffer){
	_kernel_advanceBuffer(buffer, 1);

	struct kernel_ACPIHeader* sdt = scope->SDT;
	char* namestring = _kernel_extractNamestring(buffer);
	u8 regionType = _kernel_readBuffer8(buffer);
	_kernel_advanceBuffer(buffer, 1);
	u64 regionOffset = _kernel_extractTermArgInteger(buffer, sdt);
	u64 regionLength = _kernel_extractTermArgInteger(buffer, sdt);

	_kernel_appendToChildScope(scope, _kernel_loadOperationRegionToObject(_kernel_createScope(namestring), regionType, regionOffset, regionLength));
}



static void _kernel_parseAlias(struct kernel_ACPIScope* scope, struct _kernel_StreamBuffer* buffer){
	_kernel_advanceBuffer(buffer, 1);
	char* alias = _kernel_extractNamestring(buffer);
	char* objpath = _kernel_extractNamestring(buffer);

	_kernel_appendToChildScope(scope, _kernel_loadAliasRefToObject(_kernel_createScope(alias), kernel_searchScope(scope, objpath)));
}

static void _kernel_parseScope(struct kernel_ACPIScope* scope, struct _kernel_StreamBuffer* buffer){
	_kernel_advanceBuffer(buffer, 1);
	u32 length = _kernel_extractPkgLength(buffer);
	
	char* namepath = _kernel_extractNamestring(buffer);
		
	struct kernel_ACPIScope* newScope = kernel_searchScope(scope, namepath);
	
	//I am guessing this part is not needed because they will be defined in other places, but I will leave for now
	if (newScope == NULL){
		char* name = _kernel_getNameFromPath(namepath);

		newScope = _kernel_createScope(name);
	}
	
	return _kernel_parseTermList(newScope, buffer);
}

static char* _kernel_getNameFromPath(char* path){
	char* tempA = path;
	if (*tempA == '\\')
		tempA++;
	else if (*tempA == '^'){
		tempA++;
		while (*tempA == '^') tempA++;
	}

	if (*tempA == 0) return NULL;
	char* tempB = tempA;
	while (*path != 0){
		tempA = tempB;
		tempB += 4;
		if (*path == '.') path++;
	}
	return tempA;
}

static u64 _kernel_extractTermArgInteger(struct _kernel_StreamBuffer* buffer,  struct kernel_ACPIHeader* header){
	u64 integer = 0;
	u8 incoming = _kernel_readBuffer8(buffer);
	_kernel_advanceBuffer(buffer, 1);

	switch (incoming){
		case 0x0A: {
			integer = _kernel_readBuffer8(buffer);
			_kernel_advanceBuffer(buffer, 1);
			break;
		}
		case 0x0B: {
			integer = _kernel_readBuffer16(buffer);
			_kernel_advanceBuffer(buffer, 2);
			break;
		}
		case 0x0C: {
			integer = _kernel_readBuffer32(buffer);
			_kernel_advanceBuffer(buffer, 4);
			break;
		}
		case 0x0E: {
			integer = _kernel_readBuffer64(buffer);
			_kernel_advanceBuffer(buffer, 8);
			break;
		}
		default:
			kernel_panic("Unknown ACPI opcode\n");
			break;
	}


	return integer;
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
}

static char* _kernel_extractNamestring(struct _kernel_StreamBuffer* buffer){
	u32 savedPos = buffer->position;
	if (_kernel_checkBuffer8(buffer, 0x5C))
		_kernel_advanceBuffer(buffer, 1);
	else if (_kernel_checkBuffer8(buffer, 0x5E)){
		_kernel_advanceBuffer(buffer, 1);
		while (_kernel_checkBuffer8(buffer, 0x5E))
			_kernel_advanceBuffer(buffer, 1);
	}
	
	if (_kernel_checkBuffer8(buffer, 0x2E))
		_kernel_advanceBuffer(buffer, 9);
	else if (_kernel_checkBuffer8(buffer, 0x2F)){
		_kernel_advanceBuffer(buffer, 1);
		u8 segCount = _kernel_readBuffer8(buffer);
		_kernel_advanceBuffer(buffer, segCount + 1);
	}else if (!_kernel_checkBuffer8(buffer, 0))
		_kernel_advanceBuffer(buffer, 4);


	u32 length = buffer->position - savedPos + 1;
	if (_kernel_checkBuffer8(buffer, 0)){
		_kernel_advanceBuffer(buffer, 1);
	}

	char* str = kernel_malloc(length, 1);
	kernel_memset(str, 0, length);
	kernel_memcpy(str, buffer->source + savedPos, length - 1);
	return str;	
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

static struct kernel_ACPIScope* _kernel_loadAliasRefToObject(struct kernel_ACPIScope* refScope, struct kernel_ACPIScope* ref){
	refScope->type = KERNEL_AML_ALIASREF;
	refScope->data.aliasReference = ref;
	return refScope;
}

static struct kernel_ACPIScope* _kernel_loadOperationRegionToObject(struct kernel_ACPIScope* scope, u8 rT, u64 rO, u64 rL){
	scope->type = KERNEL_AML_OPERATIONREGION;
	scope->data.operationRegion.regionType = rT;
	scope->data.operationRegion.regionOffset = rO;
	scope->data.operationRegion.regionLength = rL;
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
		
		
static u8 _kernel_getObjectType(struct kernel_ACPIScope* scope){
	u8 type = KERNEL_AML_UNINITALIZED;
	
	switch (scope->type){
		case KERNEL_AML_ALIASREF:
			type = _kernel_getObjectType(scope->data.aliasReference);
			break;
		default:
			type = scope->type;
			break;
	}
	
	return type;
}

static void _kernel_debugACPIObject(struct kernel_ACPIScope* scope){
	switch (scope->type){
		case KERNEL_AML_STRING:
			kernel_printStringBOCHS(scope->data.string);
			break;
		case KERNEL_AML_INTEGER:
			kernel_printUNumberBOCHS((u32)scope->data.integer);
			break;
		case KERNEL_AML_ALIASREF:
			_kernel_debugACPIObject(scope->data.aliasReference);
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


static inline u8 _kernel_readBuffer8(struct _kernel_StreamBuffer* buffer){
	return *(u8*)(buffer->source + buffer->position);
}

static inline u16 _kernel_readBuffer16(struct _kernel_StreamBuffer* buffer){
	return *(u16*)(buffer->source + buffer->position);
}

static inline u32 _kernel_readBuffer32(struct _kernel_StreamBuffer* buffer){
	return *(u32*)(buffer->source + buffer->position);
}

static u64 _kernel_readBuffer64(struct _kernel_StreamBuffer* buffer){
	return *(u64*)(buffer->source + buffer->position);
}

static void _kernel_advanceBuffer(struct _kernel_StreamBuffer* buffer, u32 size){
	buffer->position += size;
	buffer->size -= size;
}

static u8 _kernel_checkBuffer8(struct _kernel_StreamBuffer* buffer, u8 value){
	return _kernel_isBufferDone(buffer) ? 0 : (_kernel_readBuffer8(buffer) == value);
}

static u8 _kernel_checkBuffer16(struct _kernel_StreamBuffer* buffer, u16 value){
	return _kernel_isBufferDone(buffer) ? 0 : (_kernel_readBuffer16(buffer) == value);
}

static u8 _kernel_checkBuffer32(struct _kernel_StreamBuffer* buffer, u32 value){
	return _kernel_isBufferDone(buffer) ? 0 : (_kernel_readBuffer32(buffer) == value);
}

static u8 _kernel_isBufferDone(struct _kernel_StreamBuffer* buffer){
	return (buffer->size == 0);
}

