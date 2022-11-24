add_library(json SHARED
	${CMAKE_CURRENT_LIST_DIR}/extension.cpp
	${CMAKE_CURRENT_LIST_DIR}/detail/ext.cpp
	${CMAKE_CURRENT_LIST_DIR}/json/main.cpp
	${CMAKE_CURRENT_LIST_DIR}/json/sm_handler.cpp
	${CMAKE_CURRENT_LIST_DIR}/json/sm_interface.cpp
	${CMAKE_CURRENT_LIST_DIR}/json/sp_native.cpp
)

target_include_directories(json PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/json
	${CMAKE_CURRENT_LIST_DIR}/common
	${CMAKE_CURRENT_LIST_DIR}/detail
)

add_library(lapis_sdk INTERFACE)
target_include_directories(lapis_sdk INTERFACE
	$ENV{LPS_SDK_PATH}/sm/public
)
target_link_libraries(lapis_sdk INTERFACE amsdk)

target_link_libraries(json PUBLIC lapis_sdk fmt::fmt-header-only)

set_target_properties(json PROPERTIES PREFIX "" SUFFIX ".ext${LIB_SHARED_EXT}")

# Windows is true, Linux is not tested.
install(TARGETS json
	RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/addons/sourcemod/extensions
)

# If you have gamedata
# Uncomment to activate it
#[[ install(
	DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/gamedata/
	DESTINATION ${PROJECT_SOURCE_DIR}/addons/sourcemod/gamedata
	FILES_MATCHING PATTERN "*.games.txt"
) ]]

# If you have SP include...
# Uncomment to activate it
install(
	DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/sourcepawn/
	DESTINATION ${PROJECT_SOURCE_DIR}/addons/sourcemod/scripting/include
	FILES_MATCHING PATTERN "*.inc"
)
