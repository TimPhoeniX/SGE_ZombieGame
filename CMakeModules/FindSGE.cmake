## FindSGE version 0.1 -- this is not the final bersion of this module. Temporary solution.

if (NOT SGE_ROOT)
    message(ERROR "SGE_ROOT not specified!")
endif()

set(SGE_INCLUDE_DIR "${SGE_ROOT}/include")
set(SGE_LIBRARY_DIR "${SGE_ROOT}/lib")
set(SGE_FOUND ON)

find_library(SGE_ACTION_LIB NAMES Action PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_ACTION_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_ACTION_HANDLER_LIB NAMES ActionHandler PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_ACTION_HANDLER_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_BOX2D_LIB NAMES Box2D PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_BOX2D_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_GAME_LIB NAMES Game PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_GAME_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_IO_LIB NAMES IO PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_IO_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_LEVEL_PATH NAMES Level PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_LEVEL_PATH)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_LOGIC_LIB NAMES Logic PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_LOGIC_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_OBJECT_LIB NAMES Object PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_OBJECT_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_PicoPNG_LIB NAMES PicoPNG PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_PicoPNG_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_RENDERER_LIB NAMES Renderer PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_RENDERER_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_SCENE_LIB NAMES Scene PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_SCENE_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

find_library(SGE_UTILS_LIB NAMES Utils PATHS ${SGE_LIBRARY_DIR})
if (NOT SGE_UTILS_LIB)
    message(WARNING "Cannot find SGE lib!")
    set(SGE_FOUND OFF)
endif()

if (SGE_FOUND)
    set(SGE_LIBRARIES "${SGE_ACTION_LIB};${SGE_ACTION_HANDLER_LIB};${SGE_BOX2D_LIB};${SGE_GAME_LIB};${SGE_IO_LIB};${SGE_LEVEL_PATH};${SGE_LOGIC_LIB};${SGE_OBJECT_LIB};${SGE_PicoPNG_LIB};${SGE_RENDERER_LIB};${SGE_SCENE_LIB};${SGE_UTILS_LIB}")
endif()