#define DUCKDB_EXTENSION_MAIN

#include "lakefs_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

// OpenSSL linked through vcpkg
#include <openssl/opensslv.h>

namespace duckdb {

inline void LakefsScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Lakefs "+name.GetString()+" 🐥");
        });
}

inline void LakefsOpenSSLVersionScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Lakefs " + name.GetString() +
                                                     ", my linked OpenSSL version is " +
                                                     OPENSSL_VERSION_TEXT );
        });
}

static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto lakefs_scalar_function = ScalarFunction("lakefs", {LogicalType::VARCHAR}, LogicalType::VARCHAR, LakefsScalarFun);
    ExtensionUtil::RegisterFunction(instance, lakefs_scalar_function);

    // Register another scalar function
    auto lakefs_openssl_version_scalar_function = ScalarFunction("lakefs_openssl_version", {LogicalType::VARCHAR},
                                                LogicalType::VARCHAR, LakefsOpenSSLVersionScalarFun);
    ExtensionUtil::RegisterFunction(instance, lakefs_openssl_version_scalar_function);
}

void LakefsExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string LakefsExtension::Name() {
	return "lakefs";
}

std::string LakefsExtension::Version() const {
#ifdef EXT_VERSION_LAKEFS
	return EXT_VERSION_LAKEFS;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void lakefs_init(duckdb::DatabaseInstance &db) {
    duckdb::DuckDB db_wrapper(db);
    db_wrapper.LoadExtension<duckdb::LakefsExtension>();
}

DUCKDB_EXTENSION_API const char *lakefs_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
