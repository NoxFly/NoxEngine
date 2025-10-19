# CMake script to clean up bin/include/NoxEngine directory
# Usage: cmake -P scripts/remove_cpp_from_headers.cmake
#
# This script performs the following operations:
# 1. Remove all .cpp files
# 2. Move files up if filename matches parent directory (e.g., BoxGeometry/BoxGeometry.hpp -> BoxGeometry.hpp)
#    ALL FILES ARE MOVED FIRST before any include updates
# 3. Update ALL #include paths (after all file movements are complete):
#    a. Update paths for files that were moved
#    b. Convert all paths to be relative to INCLUDE_DIR (bin/include/NoxEngine)
# 4. Remove empty directories

set(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../bin/include/NoxEngine")

if(NOT EXISTS ${INCLUDE_DIR})
    message(WARNING "Directory ${INCLUDE_DIR} does not exist")
    return()
endif()

# ============================================================================
# Step 1: Remove all .cpp files
# ============================================================================
message(STATUS "Step 1: Removing .cpp files from ${INCLUDE_DIR}")
file(GLOB_RECURSE CPP_FILES "${INCLUDE_DIR}/*.cpp")

foreach(CPP_FILE ${CPP_FILES})
    file(REMOVE ${CPP_FILE})
    message(STATUS "  Removed: ${CPP_FILE}")
endforeach()

# ============================================================================
# Step 2: Move files up if filename matches parent directory name
# ============================================================================
message(STATUS "Step 2: Moving files up when name matches parent directory")

# Find all header files recursively
file(GLOB_RECURSE ALL_HEADERS 
    "${INCLUDE_DIR}/*.hpp" 
    "${INCLUDE_DIR}/*.h" 
    "${INCLUDE_DIR}/*.inl"
)

# Track moved files: old_path -> new_path
set(MOVED_FILES_MAP "")

foreach(HEADER_FILE ${ALL_HEADERS})
    # Get the filename without extension and the directory name
    get_filename_component(FILE_NAME_WE ${HEADER_FILE} NAME_WE)
    get_filename_component(FILE_NAME ${HEADER_FILE} NAME)
    get_filename_component(PARENT_DIR ${HEADER_FILE} DIRECTORY)
    get_filename_component(PARENT_DIR_NAME ${PARENT_DIR} NAME)
    
    # Check if filename matches parent directory name
    if(FILE_NAME_WE STREQUAL PARENT_DIR_NAME)
        # Get the grandparent directory
        get_filename_component(GRANDPARENT_DIR ${PARENT_DIR} DIRECTORY)
        set(NEW_LOCATION "${GRANDPARENT_DIR}/${FILE_NAME}")
        
        # Only move if not already at the target location
        if(NOT "${HEADER_FILE}" STREQUAL "${NEW_LOCATION}")
            # Calculate relative paths from INCLUDE_DIR for mapping
            file(RELATIVE_PATH OLD_REL_PATH ${INCLUDE_DIR} ${HEADER_FILE})
            file(RELATIVE_PATH NEW_REL_PATH ${INCLUDE_DIR} ${NEW_LOCATION})
            
            # Store the mapping
            list(APPEND MOVED_FILES_MAP "${OLD_REL_PATH}|${NEW_REL_PATH}")
            
            file(RENAME ${HEADER_FILE} ${NEW_LOCATION})
            message(STATUS "  Moved: ${FILE_NAME_WE}/${FILE_NAME} -> ${FILE_NAME}")
        endif()
    endif()
endforeach()

# ============================================================================
# Step 3: Update all #include paths (after all moves are complete)
# ============================================================================
message(STATUS "Step 3: Updating #include paths and converting to relative paths")

# Get all header files (after all moves are done)
file(GLOB_RECURSE ALL_HEADERS_FINAL
    "${INCLUDE_DIR}/*.hpp" 
    "${INCLUDE_DIR}/*.h" 
    "${INCLUDE_DIR}/*.inl"
)

foreach(HEADER_FILE ${ALL_HEADERS_FINAL})
    # Read the file content
    file(READ ${HEADER_FILE} FILE_CONTENT)
    set(MODIFIED FALSE)
    set(NEW_CONTENT "${FILE_CONTENT}")
    
    # --- Part A: First, update paths for moved files ---
    if(MOVED_FILES_MAP)
        foreach(MAPPING ${MOVED_FILES_MAP})
            string(REPLACE "|" ";" MAPPING_PAIR ${MAPPING})
            list(GET MAPPING_PAIR 0 OLD_PATH)
            list(GET MAPPING_PAIR 1 NEW_PATH)
            
            # Normalize paths
            string(REPLACE "\\" "/" OLD_PATH_NORMALIZED ${OLD_PATH})
            string(REPLACE "\\" "/" NEW_PATH_NORMALIZED ${NEW_PATH})
            
            # Check if the old path is referenced in this file
            string(FIND "${NEW_CONTENT}" "${OLD_PATH_NORMALIZED}" FOUND_POS)
            
            if(NOT FOUND_POS EQUAL -1)
                string(REPLACE "${OLD_PATH_NORMALIZED}" "${NEW_PATH_NORMALIZED}" NEW_CONTENT "${NEW_CONTENT}")
                set(MODIFIED TRUE)
            endif()
        endforeach()
    endif()
    
    # --- Part B: Convert ALL includes to relative paths (relative to each file) ---
    # Based on bash script logic: search for files by name in the entire tree
    # This handles files that were moved and includes that might be relative to old locations
    
    # Get the directory of the current file
    get_filename_component(CURRENT_FILE_DIR ${HEADER_FILE} DIRECTORY)
    
    # Find all #include directives with quotes (use NEW_CONTENT)
    string(REGEX MATCHALL "#include[ \t]+\"([^\"]+)\"" INCLUDES "${NEW_CONTENT}")
    
    foreach(INCLUDE_MATCH ${INCLUDES})
        # Extract the included path
        string(REGEX REPLACE "#include[ \t]+\"([^\"]+)\"" "\\1" INCLUDED_PATH "${INCLUDE_MATCH}")
        
        # Skip system includes or empty
        if(INCLUDED_PATH MATCHES "^<" OR INCLUDED_PATH STREQUAL "")
            continue()
        endif()
        
        # NOTE: We DO process relative paths (./ ../) because files may have been moved
        # and their relative includes need to be recalculated
        
        # Normalize path separators
        string(REPLACE "\\" "/" INCLUDED_PATH_NORMALIZED ${INCLUDED_PATH})
        
        # If the path is already relative (./ or ../), try to resolve it first
        # This helps find the actual file even if it was correct before the file was moved
        if(INCLUDED_PATH_NORMALIZED MATCHES "^\\.\\.?/.*")
            # Try to resolve from current file directory
            get_filename_component(RESOLVED_PATH "${CURRENT_FILE_DIR}/${INCLUDED_PATH_NORMALIZED}" ABSOLUTE)
            if(EXISTS ${RESOLVED_PATH})
                # Use the resolved path directly
                set(ABSOLUTE_INCLUDED_PATH ${RESOLVED_PATH})
                set(FOUND_FILES ${RESOLVED_PATH})
            else()
                # File doesn't exist at relative path, extract filename and search
                get_filename_component(INCLUDED_FILENAME ${INCLUDED_PATH_NORMALIZED} NAME)
                file(GLOB_RECURSE FOUND_FILES "${INCLUDE_DIR}/${INCLUDED_FILENAME}")
            endif()
        else()
            # Extract just the filename (like basename in bash)
            get_filename_component(INCLUDED_FILENAME ${INCLUDED_PATH_NORMALIZED} NAME)
            
            # Search for the file in the entire include directory tree (like find in bash)
            file(GLOB_RECURSE FOUND_FILES "${INCLUDE_DIR}/${INCLUDED_FILENAME}")
        endif()
        
        if(NOT FOUND_FILES)
            # Try with ** pattern
            file(GLOB_RECURSE FOUND_FILES "${INCLUDE_DIR}/**/${INCLUDED_FILENAME}")
        endif()
        
        if(FOUND_FILES)
            # Take the first match (should be unique)
            list(GET FOUND_FILES 0 ABSOLUTE_INCLUDED_PATH)
            
            # Calculate the relative path from current file to included file (like realpath --relative-to in bash)
            file(RELATIVE_PATH RELATIVE_INCLUDED_PATH ${CURRENT_FILE_DIR} ${ABSOLUTE_INCLUDED_PATH})
            
            # Normalize the relative path
            string(REPLACE "\\" "/" RELATIVE_INCLUDED_PATH "${RELATIVE_INCLUDED_PATH}")
            
            # Only update if the paths are different
            if(NOT "${INCLUDED_PATH_NORMALIZED}" STREQUAL "${RELATIVE_INCLUDED_PATH}")
                # Replace the old include with the new relative one
                string(REPLACE "#include \"${INCLUDED_PATH}\"" "#include \"${RELATIVE_INCLUDED_PATH}\"" NEW_CONTENT "${NEW_CONTENT}")
                set(MODIFIED TRUE)
            endif()
        else()
            # File not found - it might be a system or external header, just skip silently
            # Only warn if it looks like a local include (no angle brackets)
            if(NOT INCLUDED_PATH MATCHES "^<.*>$")
                file(RELATIVE_PATH REL_HEADER ${INCLUDE_DIR} ${HEADER_FILE})
                message(WARNING "  Cannot find included file '${INCLUDED_FILENAME}' (from '${INCLUDED_PATH}') referenced in ${REL_HEADER}")
            endif()
        endif()
    endforeach()
    
    # Write back if modified
    if(MODIFIED)
        file(WRITE ${HEADER_FILE} "${NEW_CONTENT}")
        file(RELATIVE_PATH REL_HEADER ${INCLUDE_DIR} ${HEADER_FILE})
        message(STATUS "  Converted includes to relative in: ${REL_HEADER}")
    endif()
endforeach()

# ============================================================================
# Step 4: Remove empty directories
# ============================================================================
message(STATUS "Step 4: Removing empty directories")

# Function to check if a directory is empty (only works with execute_process)
# We need to recursively check from deepest to shallowest
function(remove_empty_dirs ROOT_DIR)
    file(GLOB_RECURSE ALL_ITEMS "${ROOT_DIR}/*")
    file(GLOB_RECURSE ALL_DIRS LIST_DIRECTORIES true "${ROOT_DIR}/*")
    
    # Get only directories (filter out files)
    set(DIRS_ONLY "")
    foreach(ITEM ${ALL_DIRS})
        if(IS_DIRECTORY ${ITEM})
            list(APPEND DIRS_ONLY ${ITEM})
        endif()
    endforeach()
    
    # Sort directories by depth (deepest first) for proper cleanup
    list(SORT DIRS_ONLY COMPARE NATURAL ORDER DESCENDING)
    
    foreach(DIR ${DIRS_ONLY})
        # Check if directory is empty
        file(GLOB DIR_CONTENTS "${DIR}/*")
        list(LENGTH DIR_CONTENTS NUM_ITEMS)
        
        if(NUM_ITEMS EQUAL 0)
            file(REMOVE_RECURSE ${DIR})
            message(STATUS "  Removed empty directory: ${DIR}")
        endif()
    endforeach()
endfunction()

# Call the function multiple times to handle nested empty directories
remove_empty_dirs(${INCLUDE_DIR})
remove_empty_dirs(${INCLUDE_DIR})
remove_empty_dirs(${INCLUDE_DIR})

message(STATUS "Cleanup complete!")
message(STATUS "===========================================")
message(STATUS "Public headers are ready in bin/include/")
