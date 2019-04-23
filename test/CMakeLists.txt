
add_executable(writer writer.cpp)
TARGET_LINK_LIBRARIES(writer paged journal)

add_executable(reader reader.cpp)
TARGET_LINK_LIBRARIES(reader paged journal)

               
           
