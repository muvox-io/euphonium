// THIS CORNFILE IS GENERATED. DO NOT EDIT! 🌽
#include <vector>
#include "protobuf.h"
ReflectType reflectTypeInfo[24] = {
ReflectType::ofEnum(/* mine id */ ReflectTypeID::EnumReflectTypeID, /* name */ "ReflectTypeID", /* enum values */ std::move(std::vector<ReflectEnumValue>{   ReflectEnumValue("EnumReflectTypeID", 0),
   ReflectEnumValue("ClassReflectField", 1),
   ReflectEnumValue("ClassReflectEnumValue", 2),
   ReflectEnumValue("ClassReflectType", 3),
   ReflectEnumValue("EnumReflectTypeKind", 4),
   ReflectEnumValue("VectorOfClassReflectField", 5),
   ReflectEnumValue("VectorOfClassReflectEnumValue", 6),
   ReflectEnumValue("Bool", 7),
   ReflectEnumValue("UnsignedChar", 8),
   ReflectEnumValue("Float", 9),
   ReflectEnumValue("Uint32", 10),
   ReflectEnumValue("Uint64", 11),
   ReflectEnumValue("Uint8", 12),
   ReflectEnumValue("Char", 13),
   ReflectEnumValue("String", 14),
   ReflectEnumValue("Double", 15),
   ReflectEnumValue("Int32", 16),
   ReflectEnumValue("Int64", 17),
   ReflectEnumValue("Int", 18),
   ReflectEnumValue("UnsignedInt", 19),
   ReflectEnumValue("SizeT", 20),
   ReflectEnumValue("VectorOfUint8", 21),
   ReflectEnumValue("ClassConfigurationApiServiceConfiguration", 22),
   ReflectEnumValue("ClassDupaServiceConfiguration", 23),
}), /* size */ sizeof(ReflectTypeID)),
ReflectType::ofClass(
	/* mine type id */ ReflectTypeID::ClassReflectField, 
	/* name */ "ReflectField", 
	/* fields */ std::move(std::vector<ReflectField>{ReflectField( /* typeID */ ReflectTypeID::EnumReflectTypeID, /* name */ "typeID", /* offset */ offsetof(ReflectField, typeID), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::String, /* name */ "name", /* offset */ offsetof(ReflectField, name), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::SizeT, /* name */ "offset", /* offset */ offsetof(ReflectField, offset), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::Uint32, /* name */ "protobufTag", /* offset */ offsetof(ReflectField, protobufTag), /* protobuf tag */ 0),
}), 
	/* size */ sizeof(ReflectField), 
	__reflectConstruct<ReflectField>,
	__reflectDestruct<ReflectField>),
ReflectType::ofClass(
	/* mine type id */ ReflectTypeID::ClassReflectEnumValue, 
	/* name */ "ReflectEnumValue", 
	/* fields */ std::move(std::vector<ReflectField>{ReflectField( /* typeID */ ReflectTypeID::String, /* name */ "name", /* offset */ offsetof(ReflectEnumValue, name), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::Int, /* name */ "value", /* offset */ offsetof(ReflectEnumValue, value), /* protobuf tag */ 0),
}), 
	/* size */ sizeof(ReflectEnumValue), 
	__reflectConstruct<ReflectEnumValue>,
	__reflectDestruct<ReflectEnumValue>),
ReflectType::ofClass(
	/* mine type id */ ReflectTypeID::ClassReflectType, 
	/* name */ "ReflectType", 
	/* fields */ std::move(std::vector<ReflectField>{ReflectField( /* typeID */ ReflectTypeID::EnumReflectTypeID, /* name */ "typeID", /* offset */ offsetof(ReflectType, typeID), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::String, /* name */ "name", /* offset */ offsetof(ReflectType, name), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::EnumReflectTypeKind, /* name */ "kind", /* offset */ offsetof(ReflectType, kind), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::SizeT, /* name */ "size", /* offset */ offsetof(ReflectType, size), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::EnumReflectTypeID, /* name */ "innerType", /* offset */ offsetof(ReflectType, innerType), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::VectorOfClassReflectField, /* name */ "fields", /* offset */ offsetof(ReflectType, fields), /* protobuf tag */ 0),
ReflectField( /* typeID */ ReflectTypeID::VectorOfClassReflectEnumValue, /* name */ "enumValues", /* offset */ offsetof(ReflectType, enumValues), /* protobuf tag */ 0),
}), 
	/* size */ sizeof(ReflectType), 
	__reflectConstruct<ReflectType>,
	__reflectDestruct<ReflectType>),
ReflectType::ofEnum(/* mine id */ ReflectTypeID::EnumReflectTypeKind, /* name */ "ReflectTypeKind", /* enum values */ std::move(std::vector<ReflectEnumValue>{   ReflectEnumValue("Primitive", 0),
   ReflectEnumValue("Enum", 1),
   ReflectEnumValue("Class", 2),
   ReflectEnumValue("Vector", 3),
   ReflectEnumValue("Optional", 4),
}), /* size */ sizeof(ReflectTypeKind)),

	ReflectType::ofVector(
		/* mine typeId */ ReflectTypeID::VectorOfClassReflectField,
		/* inner type id */  ReflectTypeID::ClassReflectField,
		/* size */ sizeof(std::vector<ReflectField>),
		VectorOperations{
			.push_back = __VectorManipulator<ReflectField>::push_back,
			.at = __VectorManipulator<ReflectField>::at,
			.size = __VectorManipulator<ReflectField>::size,
			.emplace_back =  __VectorManipulator<ReflectField>::emplace_back,
			.clear = __VectorManipulator<ReflectField>::clear,
			.reserve = __VectorManipulator<ReflectField>::reserve,
		},
		__reflectConstruct<std::vector<ReflectField>>,
		__reflectDestruct<std::vector<ReflectField>>
	)
	
	
	
	,

	ReflectType::ofVector(
		/* mine typeId */ ReflectTypeID::VectorOfClassReflectEnumValue,
		/* inner type id */  ReflectTypeID::ClassReflectEnumValue,
		/* size */ sizeof(std::vector<ReflectEnumValue>),
		VectorOperations{
			.push_back = __VectorManipulator<ReflectEnumValue>::push_back,
			.at = __VectorManipulator<ReflectEnumValue>::at,
			.size = __VectorManipulator<ReflectEnumValue>::size,
			.emplace_back =  __VectorManipulator<ReflectEnumValue>::emplace_back,
			.clear = __VectorManipulator<ReflectEnumValue>::clear,
			.reserve = __VectorManipulator<ReflectEnumValue>::reserve,
		},
		__reflectConstruct<std::vector<ReflectEnumValue>>,
		__reflectDestruct<std::vector<ReflectEnumValue>>
	)
	
	
	
	,
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Bool, /* name */ "bool", /* size */ sizeof(bool)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::UnsignedChar, /* name */ "unsigned char", /* size */ sizeof(unsigned char)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Float, /* name */ "float", /* size */ sizeof(float)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Uint32, /* name */ "uint32_t", /* size */ sizeof(uint32_t)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Uint64, /* name */ "uint64_t", /* size */ sizeof(uint64_t)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Uint8, /* name */ "uint8_t", /* size */ sizeof(uint8_t)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Char, /* name */ "char", /* size */ sizeof(char)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::String, /* name */ "std::string", /* size */ sizeof(std::string)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Double, /* name */ "double", /* size */ sizeof(double)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Int32, /* name */ "int32_t", /* size */ sizeof(int32_t)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Int64, /* name */ "int64_t", /* size */ sizeof(int64_t)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::Int, /* name */ "int", /* size */ sizeof(int)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::UnsignedInt, /* name */ "unsigned int", /* size */ sizeof(unsigned int)),
ReflectType::ofPrimitive(/* type id */ ReflectTypeID::SizeT, /* name */ "size_t", /* size */ sizeof(size_t)),

	ReflectType::ofVector(
		/* mine typeId */ ReflectTypeID::VectorOfUint8,
		/* inner type id */  ReflectTypeID::Uint8,
		/* size */ sizeof(std::vector<uint8_t>),
		VectorOperations{
			.push_back = __VectorManipulator<uint8_t>::push_back,
			.at = __VectorManipulator<uint8_t>::at,
			.size = __VectorManipulator<uint8_t>::size,
			.emplace_back =  __VectorManipulator<uint8_t>::emplace_back,
			.clear = __VectorManipulator<uint8_t>::clear,
			.reserve = __VectorManipulator<uint8_t>::reserve,
		},
		__reflectConstruct<std::vector<uint8_t>>,
		__reflectDestruct<std::vector<uint8_t>>
	)
	
	
	
	,
ReflectType::ofClass(
	/* mine type id */ ReflectTypeID::ClassConfigurationApiServiceConfiguration, 
	/* name */ "ConfigurationApiServiceConfiguration", 
	/* fields */ std::move(std::vector<ReflectField>{ReflectField( /* typeID */ ReflectTypeID::String, /* name */ "dupaValue", /* offset */ offsetof(ConfigurationApiServiceConfiguration, dupaValue), /* protobuf tag */ 1),
}), 
	/* size */ sizeof(ConfigurationApiServiceConfiguration), 
	__reflectConstruct<ConfigurationApiServiceConfiguration>,
	__reflectDestruct<ConfigurationApiServiceConfiguration>),
ReflectType::ofClass(
	/* mine type id */ ReflectTypeID::ClassDupaServiceConfiguration, 
	/* name */ "DupaServiceConfiguration", 
	/* fields */ std::move(std::vector<ReflectField>{ReflectField( /* typeID */ ReflectTypeID::String, /* name */ "dupaValue", /* offset */ offsetof(DupaServiceConfiguration, dupaValue), /* protobuf tag */ 1),
ReflectField( /* typeID */ ReflectTypeID::String, /* name */ "dupaValue2", /* offset */ offsetof(DupaServiceConfiguration, dupaValue2), /* protobuf tag */ 2),
}), 
	/* size */ sizeof(DupaServiceConfiguration), 
	__reflectConstruct<DupaServiceConfiguration>,
	__reflectDestruct<DupaServiceConfiguration>),
};

