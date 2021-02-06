// THIS CORNFILE IS GENERATED. DO NOT EDIT! 🌽
#ifndef __REFLECTION_INTERNALH
#define __REFLECTION_INTERNALH
#include <string>
#include <vector>
enum class ReflectTypeID {
EnumReflectTypeID = 0,
ClassReflectField = 1,
ClassReflectEnumValue = 2,
ClassReflectType = 3,
EnumReflectTypeKind = 4,
VectorOfClassReflectField = 5,
VectorOfClassReflectEnumValue = 6,
Bool = 7,
UnsignedChar = 8,
Float = 9,
Uint32 = 10,
Uint64 = 11,
Uint8 = 12,
Char = 13,
String = 14,
Double = 15,
Int32 = 16,
Int64 = 17,
Int = 18,
UnsignedInt = 19,
SizeT = 20,
VectorOfUint8 = 21,
ClassConfigurationApiServiceConfiguration = 22,
ClassDupaServiceConfiguration = 23,
};

enum class ReflectTypeKind {
Primitive = 0,
Enum = 1,
Class = 2,
Vector = 3,
Optional = 4,
};

class ReflectField {
public:
ReflectTypeID typeID;
std::string name;
size_t offset;
uint32_t protobufTag;
static constexpr ReflectTypeID _TYPE_ID = ReflectTypeID::ClassReflectField;

			ReflectField() {};
			ReflectField(ReflectTypeID typeID, std::string name, size_t offset, uint32_t protobufTag) {
				this->typeID = typeID;
				this->name = name;
				this->offset = offset;
				this->protobufTag = protobufTag;
			}
		};

class ReflectEnumValue {
public:
std::string name;
int value;
static constexpr ReflectTypeID _TYPE_ID = ReflectTypeID::ClassReflectEnumValue;

			ReflectEnumValue(){};
			ReflectEnumValue( std::string name, int value) {
				this->name = name;
				this->value = value;
			}
		};

class ReflectType {
public:
ReflectTypeID typeID;
std::string name;
ReflectTypeKind kind;
size_t size;
ReflectTypeID innerType;
std::vector<ReflectField> fields;
std::vector<ReflectEnumValue> enumValues;
static constexpr ReflectTypeID _TYPE_ID = ReflectTypeID::ClassReflectType;

		void (*_Construct)(void *mem);
		void (*_Destruct)(void *obj);
		VectorOperations vectorOps;
		OptionalOperations optionalOps;
		static ReflectType ofPrimitive(ReflectTypeID id, std::string name, size_t size) {
			ReflectType t;
			t.kind = ReflectTypeKind::Primitive;
			t.typeID = id;
			t.name = name;
			t.size = size;
			return t;
		}
		static ReflectType ofEnum(ReflectTypeID id, std::string name, std::vector<ReflectEnumValue> enumValues, size_t size) {
			ReflectType t;
			t.kind = ReflectTypeKind::Enum;
			t.typeID = id;
			t.name = name;
			t.size = size;
			t.enumValues = enumValues;
			return t;
		}
		static ReflectType ofVector(ReflectTypeID id, ReflectTypeID innerType, size_t size, 
			VectorOperations vectorOps,
			void (*_Construct)(void *mem), void (*_Destruct)(void *obj)) {
			ReflectType t;
			t.kind = ReflectTypeKind::Vector;
			t.typeID = id;
			t.innerType = innerType;
			t.size = size;
			t._Construct = _Construct;
			t._Destruct = _Destruct;
			t.vectorOps = vectorOps;
			return t;
		}
		static ReflectType ofOptional(ReflectTypeID id, ReflectTypeID innerType, size_t size, 
			OptionalOperations optionalOps,
			void (*_Construct)(void *mem), void (*_Destruct)(void *obj)) {
			ReflectType t;
			t.kind = ReflectTypeKind::Optional;
			t.typeID = id;
			t.innerType = innerType;
			t.size = size;
			t._Construct = _Construct;
			t._Destruct = _Destruct;
			t.optionalOps = optionalOps;
			return t;
		}
		static ReflectType ofClass(ReflectTypeID id, std::string name, std::vector<ReflectField> fields, size_t size, void (*_Construct)(void *mem), void (*_Destruct)(void *obj)) {
			ReflectType t;
			t.kind = ReflectTypeKind::Class;
			t.name = name;
			t.typeID = id;
			t.size = size;
			t.fields = std::move(fields);
			t._Construct = _Construct;
			t._Destruct = _Destruct;
			return t;
		}
		
		};

#endif
