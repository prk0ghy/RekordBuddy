//
//  Rekord Buddy - The future proof music collection tool made by DJs for DJs.
//  Copyright (C) 2020-2021 Didier Malenfant (didier@rekordbuddy.org)
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

// -- Describes the tables and generates appropriate queries. Manages type mapping between constants and the types of the schema.
#define FORWARD_DECLS(x, parent) class Persistent##x;
ALL_TYPES(FORWARD_DECLS)
#undef FORWARD_DECLS

#define CALL_ON_TYPE(x, parent) callable(NxA::RekordBuddy::CollectionImplementation::Persistent##x{RekordBuddy::CollectionImplementation::RBSchema::Type::x, nullptr});
#define CONSTRUCT_DYNAMIC(x, parent)                                                                                \
    case Type::x: {                                                                                                 \
        auto object = std::make_shared<Persistent##x>(id, std::const_pointer_cast<PersistentContext<RBSchema>>(c)); \
        return {std::static_pointer_cast<PersistentObject>(object)};                                                \
    }

// We assign an enumeration constant for each type (strong enum, so scoped name)
#define ENUM_DECL(x, parent) x,
enum class Type : integer32 {
    Undefined_,
    ALL_TYPES(ENUM_DECL) PersistentObject,
    Last_, // special: not part of the chain but still a valid type in the schema.
    First_ = Undefined_ + 1,
};
#undef ENUM_DECL

namespace {

// We can find the constant for any type using a specialization: TypeEnumeration<Foo> == Type::Foo. For other types, returns Undefined_.
template <typename T>
struct TypeEnumeration
{
    constexpr static Type value = Type::Undefined_;
};

#define ENUM_FROM_TYPE_SPEC(x, parent)         \
    template <>                                \
    struct TypeEnumeration<Persistent##x>      \
    {                                          \
        constexpr static Type value = Type::x; \
    };
ALL_TYPES(ENUM_FROM_TYPE_SPEC);
#undef ENUM_FROM_TYPE_SPEC

// We can find the constant for any type's parent
template <Type T>
struct ParentForType
{
    constexpr static Type value = Type::Undefined_;
};

#define ENUM_PARENT_FROM_TYPE_SPEC(x, parent)       \
    template <>                                     \
    struct ParentForType<Type::x>                   \
    {                                               \
        constexpr static Type value = Type::parent; \
    };
ALL_TYPES(ENUM_PARENT_FROM_TYPE_SPEC);
#undef ENUM_PARENT_FROM_TYPE_SPEC

// Given any compile-time-known enum, we can get the actual type for that enum: is_equal<TypeFromEnum<Type::Foo>::Type, Foo>
// This is usually not as useful as TypeEnumeration, unless one needs to do some template metaprogramming with Type as a tag.
template <Type t>
struct TypeFromEnum
{
    using Type = void;
};

#define TYPE_FROM_ENUM_SPEC(x, parent) \
    template <>                        \
    struct TypeFromEnum<Type::x>       \
    {                                  \
        using Type = Persistent##x;    \
    };
ALL_TYPES(TYPE_FROM_ENUM_SPEC);
#undef TYPE_FROM_ENUM_SPEC
}

// Used strictly to carry various information about the schema to places that require types.
// Generally users should not be directly accessing the schema
struct RBSchema
{
    using Version = uinteger16;
    static constexpr Version schemaVersion()
    {
        return version;
    }
    static constexpr Version schemaReadableSinceVersion()
    {
        return readableSinceVersion;
    }
    using Index = uinteger64;
    using Store = SqlLiteStore<RBSchema>;
    using ObjectID = PersistentObjectID<RBSchema>;
    using PersistentObject = GenericPersistentObject<RBSchema>;
    using RootContext = std::shared_ptr<PersistentContext<RBSchema>>;

    template <typename T>
    using TypeEnumeration = TypeEnumeration<T>;

    template <Type T>
    using TypeFromEnum = TypeFromEnum<T>;

    template <Type T>
    using ParentForType = ParentForType<T>;

    using Type = Type;

    enum class QueryOperation { Equals, StartsWith, EndsWith, Contains };

    constexpr static int numTypes = static_cast<integer32>(Type::Last_) - static_cast<integer32>(Type::First_);

#define STR_CASE(x, parent) \
    case Type::x:           \
        return #x;
    constexpr static char const* typeToString(Type t) noexcept
    {
        switch (t) {
            ALL_TYPES(STR_CASE)
            case Type::PersistentObject:
                return "PersistentObject";
            default:
                break;
        }
        return "Undefined";
    }
#undef STR_CASE

#define TYPE_CASE_INHERIT(x, p)        \
    case Type::x:                      \
        if (b == Type::p) {            \
            return true;               \
        }                              \
        else {                         \
            return typeIs(Type::p, b); \
        }
    constexpr inline static boolean typeIs(Type a, Type b) noexcept
    {
        if (a == b)
            return true;
        if (a == Type::PersistentObject)
            return false;
        switch (a) {
            ALL_TYPES(TYPE_CASE_INHERIT)
            default:
                return false;
        }
    }
#undef TYPE_CASE_INHERIT

#define TYPE_CASE_PARENT(x, p) \
    case Type::x:              \
        return Type::p;

    constexpr static Type parentTypeOf(Type a) noexcept
    {
        if (a == Type::PersistentObject)
            return Type::Undefined_;
        switch (a) {
            ALL_TYPES(TYPE_CASE_PARENT)
            default:
                return Type::Undefined_;
        }
    }
#undef TYPE_CASE_PARENT

    static inline boolean typeIs(ObjectID a, Type b) noexcept
    {
        return typeIs(a.objectType(), b);
    }

    template <typename T>
    static inline boolean isObjectOneOf(ObjectID object) noexcept
    {
        return object.isValid() ? typeIs(object, TypeEnumeration<T>::value) : false;
    }

    template <typename T1, typename T2, typename... Ts>
    static inline boolean isObjectOneOf(ObjectID object) noexcept
    {
        return isObjectOneOf<T1>(object) || isObjectOneOf<T2, Ts...>(object);
    }

    static Optional<std::shared_ptr<GenericPersistentObject<RBSchema>>> constructDynamic(const ObjectID& id,
                                                                                         std::shared_ptr<PersistentContext<RBSchema>> c);

    // Given some string, will find the corresponding type enumeration, if any, or undefined Otherwise
    static Type stringToType(const NxA::String& str) noexcept
    {
        for (Type i = Type::First_; i != Type::Last_; i = static_cast<Type>(static_cast<integer32>(i) + 1)) {
            if (str.compare(String{typeToString(i)}) == 0) {
                return i;
            }
        }
        return Type::Undefined_;
    }

    using MigrateFunctionT = void(const std::shared_ptr<NxA::SqlLiteStore<RBSchema>>&);
    using SourceVersion = NxA::uinteger16;
    using DestinationVersion = NxA::uinteger16;

    template <typename F>
    static void applyToTypes(F callable);

    static void emptyDatabaseToCurrentMigration(const std::shared_ptr<NxA::SqlLiteStore<RBSchema>>&);
    static void version9ToCurrentMigration(const std::shared_ptr<NxA::SqlLiteStore<RBSchema>>&);

    // -- When we start up, we'll check this data structure for functions to run to migrate up to a new version.
    static const count migrationLength = 2;
    constexpr static std::tuple<SourceVersion, DestinationVersion, MigrateFunctionT*> schemaMigrations[] = {
        std::make_tuple(0, version, &emptyDatabaseToCurrentMigration),
        std::make_tuple(9, version, &version9ToCurrentMigration),
    };

    RBSchema() = delete;
    RBSchema(const RBSchema&) = delete;
    RBSchema& operator=(const RBSchema&) = delete;
};

// These aliases make using the library a little easier
using ObjectID = RBSchema::ObjectID;
using PersistentObject = RBSchema::PersistentObject;
using RootContext = RBSchema::RootContext;

namespace {
template <>
struct TypeEnumeration<PersistentObject>
{
    constexpr static Type value = Type::PersistentObject;
};

template <>
struct TypeFromEnum<Type::PersistentObject>
{
    using Type = PersistentObject;
};
}

template <typename Stream>
Stream& operator<<(Stream& os, RBSchema::Type dt)
{
    os << RBSchema::typeToString(dt);
    return os;
}
