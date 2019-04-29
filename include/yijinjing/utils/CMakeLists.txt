
set(source_dir ${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB_INCLUDE_FILES  
	${source_dir}/Hash.hpp 
	${source_dir}/json.hpp 
	${source_dir}/constants.h
	${source_dir}/sys_messages.h  
	${source_dir}/YJJ_DECLARE.h 
	)


INSTALL(FILES ${LIB_INCLUDE_FILES} DESTINATION "include/yijinjing/utils/")


