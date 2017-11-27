include_directories("../include")
include_directories("common")

file(GLOB common_sources "common/*.h" "common/*.cc")

include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/contrib/SDL-mirror/include")

# Setup MSVC parallelized builds
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

#bot_simple
add_executable(ex_bot_simple "bot_simple.cc")
set_target_properties(ex_bot_simple PROPERTIES FOLDER examples)
target_link_libraries(ex_bot_simple sc2api sc2lib sc2utils)

#echo_actions
add_executable(ex_echo_actions "echo_actions.cc")
set_target_properties(ex_echo_actions PROPERTIES FOLDER examples)
target_link_libraries(ex_echo_actions sc2api sc2utils)

#bot_micro
add_executable(ex_bot_micro "bot_micro.cc" ${common_sources})
set_target_properties(ex_bot_micro PROPERTIES FOLDER examples)
target_link_libraries(ex_bot_micro sc2api sc2lib sc2utils)

#replay
add_executable(ex_replay "replay.cc")
set_target_properties(ex_replay PROPERTIES FOLDER examples)
target_link_libraries(ex_replay sc2api sc2utils)

#Tutorial
add_executable(ex_tutorial "tutorial.cc")
set_target_properties(ex_tutorial PROPERTIES FOLDER examples)
target_link_libraries(ex_tutorial sc2api sc2utils)

#bot_mp
add_executable(ex_bot_mp "bot_mp.cc" ${common_sources})
set_target_properties(ex_bot_mp PROPERTIES FOLDER examples)
target_link_libraries(ex_bot_mp sc2api sc2lib sc2utils)


# Exclude SDL and related projects to work around linker issue.
if (NOT APPLE)
	#feature layers
	add_executable(ex_feature_layers "feature_layers.cc")
	set_target_properties(ex_feature_layers PROPERTIES FOLDER examples)
	target_link_libraries(ex_feature_layers sc2api sc2utils sc2renderer)
endif ()

#Intermediate bot
add_executable(ex_intermediate_bot "intermediate_bot.cc" ${common_sources})
set_target_properties(ex_intermediate_bot PROPERTIES FOLDER examples)
target_link_libraries(ex_intermediate_bot sc2api sc2lib sc2utils)

#annoying_helper
add_executable(ex_annoying_helper "annoying_helper.cc")
set_target_properties(ex_annoying_helper PROPERTIES FOLDER examples)
target_link_libraries(ex_annoying_helper sc2api sc2lib sc2utils)
