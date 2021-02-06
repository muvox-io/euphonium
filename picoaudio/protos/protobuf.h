// THIS CORNFILE IS GENERATED. DO NOT EDIT! 🌽
#ifndef _PROTOBUFH
#define _PROTOBUFH
#include <utility>
#include <optional>
#include <vector>
#include <type_traits>
#include <string>

		class AnyRef;
		struct VectorOperations {
			void (*push_back)(AnyRef &vec, AnyRef &val);
			AnyRef (*at)(AnyRef &vec, size_t index);
			size_t (*size)(AnyRef &vec);
			void (*emplace_back)(AnyRef &vec);
			void (*clear)(AnyRef &vec);
			void (*reserve)(AnyRef &vec, size_t n);
		};
		
	

		class AnyRef;
		struct OptionalOperations {
			AnyRef (*get)(AnyRef &opt);
			bool (*has_value)(AnyRef &opt);
			void (*set)(AnyRef &opt, AnyRef &val);
			void (*reset)(AnyRef &opt);
			void (*emplaceEmpty)(AnyRef &opt);
		};
		
	
#include "ReflectionInternal.h"
#include "configuration_api_service.h"
#include "dupa_service.h"

		template<class T>
		void __reflectConstruct(void *mem) {
			new(mem) T;
		}
		template<class T>
		void __reflectDestruct(void *obj) {
			((T*) obj)->~T();
		}
		
	
class ReflectField;
class ReflectEnumValue;
class ReflectType;
class ConfigurationApiServiceConfiguration;
class DupaServiceConfiguration;


	
	class AnyRef {
		public:
				ReflectTypeID typeID;
				AnyRef() {};
				AnyRef(ReflectTypeID typeID, void *obj) {
					this->typeID = typeID;
					this->value.voidptr = obj;
				}
				template<typename T>
				T *as() {
					// if(T::_TYPE_ID != this->typeID) {
					// 	throw "invalid as call";
					// }
					return (T*) this->value.voidptr;
				}

				template<typename T>
				bool is() {
					if constexpr(std::is_same<T, ReflectTypeID>::value) {
						return ReflectTypeID::EnumReflectTypeID == this->typeID;
					} else 
					if constexpr(std::is_same<T, ReflectTypeKind>::value) {
						return ReflectTypeID::EnumReflectTypeKind == this->typeID;
					} else 
					if constexpr(std::is_same<T, std::vector<ReflectField>>::value) {
						return ReflectTypeID::VectorOfClassReflectField == this->typeID;
					} else 
					if constexpr(std::is_same<T, std::vector<ReflectEnumValue>>::value) {
						return ReflectTypeID::VectorOfClassReflectEnumValue == this->typeID;
					} else 
					if constexpr(std::is_same<T, bool>::value) {
						return ReflectTypeID::Bool == this->typeID;
					} else 
					if constexpr(std::is_same<T, unsigned char>::value) {
						return ReflectTypeID::UnsignedChar == this->typeID;
					} else 
					if constexpr(std::is_same<T, float>::value) {
						return ReflectTypeID::Float == this->typeID;
					} else 
					if constexpr(std::is_same<T, uint32_t>::value) {
						return ReflectTypeID::Uint32 == this->typeID;
					} else 
					if constexpr(std::is_same<T, uint64_t>::value) {
						return ReflectTypeID::Uint64 == this->typeID;
					} else 
					if constexpr(std::is_same<T, uint8_t>::value) {
						return ReflectTypeID::Uint8 == this->typeID;
					} else 
					if constexpr(std::is_same<T, char>::value) {
						return ReflectTypeID::Char == this->typeID;
					} else 
					if constexpr(std::is_same<T, std::string>::value) {
						return ReflectTypeID::String == this->typeID;
					} else 
					if constexpr(std::is_same<T, double>::value) {
						return ReflectTypeID::Double == this->typeID;
					} else 
					if constexpr(std::is_same<T, int32_t>::value) {
						return ReflectTypeID::Int32 == this->typeID;
					} else 
					if constexpr(std::is_same<T, int64_t>::value) {
						return ReflectTypeID::Int64 == this->typeID;
					} else 
					if constexpr(std::is_same<T, int>::value) {
						return ReflectTypeID::Int == this->typeID;
					} else 
					if constexpr(std::is_same<T, unsigned int>::value) {
						return ReflectTypeID::UnsignedInt == this->typeID;
					} else 
					if constexpr(std::is_same<T, size_t>::value) {
						return ReflectTypeID::SizeT == this->typeID;
					} else 
					if constexpr(std::is_same<T, std::vector<uint8_t>>::value) {
						return ReflectTypeID::VectorOfUint8 == this->typeID;
					} else 
					 {
						return T::_TYPE_ID == this->typeID;
					}
				}
				

				ReflectType *reflectType();
				AnyRef getField(int i);
				template <typename T>
				static AnyRef of(T *obj)
				{
					ReflectTypeID typeID;
					if constexpr(std::is_same<T, ReflectTypeID>::value) {
						typeID = ReflectTypeID::EnumReflectTypeID;
					} else 
					if constexpr(std::is_same<T, ReflectTypeKind>::value) {
						typeID = ReflectTypeID::EnumReflectTypeKind;
					} else 
					if constexpr(std::is_same<T, std::vector<ReflectField>>::value) {
						typeID = ReflectTypeID::VectorOfClassReflectField;
					} else 
					if constexpr(std::is_same<T, std::vector<ReflectEnumValue>>::value) {
						typeID = ReflectTypeID::VectorOfClassReflectEnumValue;
					} else 
					if constexpr(std::is_same<T, bool>::value) {
						typeID = ReflectTypeID::Bool;
					} else 
					if constexpr(std::is_same<T, unsigned char>::value) {
						typeID = ReflectTypeID::UnsignedChar;
					} else 
					if constexpr(std::is_same<T, float>::value) {
						typeID = ReflectTypeID::Float;
					} else 
					if constexpr(std::is_same<T, uint32_t>::value) {
						typeID = ReflectTypeID::Uint32;
					} else 
					if constexpr(std::is_same<T, uint64_t>::value) {
						typeID = ReflectTypeID::Uint64;
					} else 
					if constexpr(std::is_same<T, uint8_t>::value) {
						typeID = ReflectTypeID::Uint8;
					} else 
					if constexpr(std::is_same<T, char>::value) {
						typeID = ReflectTypeID::Char;
					} else 
					if constexpr(std::is_same<T, std::string>::value) {
						typeID = ReflectTypeID::String;
					} else 
					if constexpr(std::is_same<T, double>::value) {
						typeID = ReflectTypeID::Double;
					} else 
					if constexpr(std::is_same<T, int32_t>::value) {
						typeID = ReflectTypeID::Int32;
					} else 
					if constexpr(std::is_same<T, int64_t>::value) {
						typeID = ReflectTypeID::Int64;
					} else 
					if constexpr(std::is_same<T, int>::value) {
						typeID = ReflectTypeID::Int;
					} else 
					if constexpr(std::is_same<T, unsigned int>::value) {
						typeID = ReflectTypeID::UnsignedInt;
					} else 
					if constexpr(std::is_same<T, size_t>::value) {
						typeID = ReflectTypeID::SizeT;
					} else 
					if constexpr(std::is_same<T, std::vector<uint8_t>>::value) {
						typeID = ReflectTypeID::VectorOfUint8;
					} else 
					 {
						typeID = T::_TYPE_ID;
					}
					AnyRef a;
					a.typeID = typeID;
					a.value.voidptr = (void*) obj;
					return a;
				}
			
				union ReflectedTypes {
					void *voidptr;
					ReflectTypeID *u_EnumReflectTypeID;
					ReflectField *u_ClassReflectField;
					ReflectEnumValue *u_ClassReflectEnumValue;
					ReflectType *u_ClassReflectType;
					ReflectTypeKind *u_EnumReflectTypeKind;
					std::vector<ReflectField> *u_VectorOfClassReflectField;
					std::vector<ReflectEnumValue> *u_VectorOfClassReflectEnumValue;
					bool *u_Bool;
					unsigned char *u_UnsignedChar;
					float *u_Float;
					uint32_t *u_Uint32;
					uint64_t *u_Uint64;
					uint8_t *u_Uint8;
					char *u_Char;
					std::string *u_String;
					double *u_Double;
					int32_t *u_Int32;
					int64_t *u_Int64;
					int *u_Int;
					unsigned int *u_UnsignedInt;
					size_t *u_SizeT;
					std::vector<uint8_t> *u_VectorOfUint8;
					ConfigurationApiServiceConfiguration *u_ClassConfigurationApiServiceConfiguration;
					DupaServiceConfiguration *u_ClassDupaServiceConfiguration;
					
				} value;
				private:
		
	};
	
	
	
	template<class T>
	class __VectorManipulator {
		public:
			static void push_back(AnyRef &vec, AnyRef &val) {
				auto theVector = reinterpret_cast<std::vector<T>*>(vec.value.voidptr);
				auto theValue = *reinterpret_cast<T*>(val.value.voidptr);
				theVector->push_back(theValue);
			};
			static AnyRef at(AnyRef &vec, size_t index) {
				auto theVector = reinterpret_cast<std::vector<T>*>(vec.value.voidptr);
				return AnyRef::of<T>(&(*theVector)[index]);
			};
			static size_t size(AnyRef &vec) {
				auto theVector = reinterpret_cast<std::vector<T>*>(vec.value.voidptr);
				return theVector->size();
			};
			static void emplace_back(AnyRef &vec) {
				auto theVector = reinterpret_cast<std::vector<T>*>(vec.value.voidptr);
				theVector->emplace_back();
			};
			static void clear(AnyRef &vec) {
				auto theVector = reinterpret_cast<std::vector<T>*>(vec.value.voidptr);
				theVector->clear();
			};
			static void reserve(AnyRef &vec, size_t n) {
				auto theVector = reinterpret_cast<std::vector<T>*>(vec.value.voidptr);
				theVector->reserve(n);
			};
	};
	

	template<class T>
	class __OptionalManipulator {
		public:
			static AnyRef get(AnyRef &opt) {
				auto theOptional = reinterpret_cast<std::optional<T>*>(opt.value.voidptr);
				return AnyRef::of<T>(&**theOptional);
			}
			static bool has_value(AnyRef &opt) {
				auto theOptional = reinterpret_cast<std::optional<T>*>(opt.value.voidptr);
				return theOptional->has_value();
			}
			static void set(AnyRef &opt, AnyRef &val) {
				auto theOptional = reinterpret_cast<std::optional<T>*>(opt.value.voidptr);
				auto theValue = reinterpret_cast<T*>(val.value.voidptr);
				*theOptional = *theValue;
			}

			static void reset(AnyRef &opt) {
				auto theOptional = reinterpret_cast<std::optional<T>*>(opt.value.voidptr);
				theOptional->reset();
			}

			static void emplaceEmpty(AnyRef &opt) {
				auto theOptional = reinterpret_cast<std::optional<T>*>(opt.value.voidptr);
				theOptional->emplace();
			}
	};
		
	
extern ReflectType reflectTypeInfo[24];
	

	class UniqueAny: public AnyRef {
		public:
			UniqueAny() {
				this->value.voidptr = nullptr;
			};
			UniqueAny(ReflectTypeID typeID) {
				this->typeID = typeID;
				auto typeInfo = &reflectTypeInfo[static_cast<int>(typeID)];
				AnyRef a;
				this->value.voidptr = new unsigned char[typeInfo->size];
				typeInfo->_Construct(this->value.voidptr);
			};
			~UniqueAny() {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(typeID)];
				typeInfo->_Destruct(this->value.voidptr);
				delete reinterpret_cast<char *>(this->value.voidptr);
			};
	};

	class AnyVectorRef {
		public:
			AnyRef ref;
			AnyVectorRef(AnyRef r): ref(r) {}
			void push_back(AnyRef &v) {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				typeInfo->vectorOps.push_back(ref, v);
			}
			size_t size() {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				return typeInfo->vectorOps.size(ref);
			}

			void emplace_back() {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				typeInfo->vectorOps.emplace_back(ref);
			}

			void clear() {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				typeInfo->vectorOps.clear(ref);
			}

			void reserve(size_t n) {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				typeInfo->vectorOps.reserve(ref, n);
			}


			AnyRef at(size_t index) {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				return typeInfo->vectorOps.at(ref, index);
			}
	};

	class AnyOptionalRef {
		public:
			AnyRef ref;
			AnyOptionalRef(AnyRef r): ref(r) {}
			
			AnyRef get() {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				return typeInfo->optionalOps.get(ref);
			}

			bool has_value() {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				return typeInfo->optionalOps.has_value(ref);
			}
			void set(AnyRef &o) {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				typeInfo->optionalOps.set(ref, o);
			}
			void reset() {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				typeInfo->optionalOps.reset(ref);
			}

			void emplaceEmpty() {
				auto typeInfo = &reflectTypeInfo[static_cast<int>(this->ref.typeID)];
				typeInfo->optionalOps.emplaceEmpty(ref);
			}

	};

	#endif
