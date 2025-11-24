add_executable(s3store main.cpp in_memory_store.cpp)
target_compile_features(s3store PRIVATE cxx_std_23)
