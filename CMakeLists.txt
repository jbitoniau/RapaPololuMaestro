CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

PROJECT( "RapaPololuMaestro" )

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake" )

SET( HEADERS 
	 include/RPMSerialInterface.h )
		
SET( SOURCES 
	 src/RPMSerialInterface.cpp )

IF( CMAKE_SYSTEM_NAME MATCHES "Windows" )
	IF( MSVC )
		INCLUDE( RapaConfigureVisualStudio )
	ENDIF()

	SET( HEADERS ${HEADERS} 
		 include/RPMSerialInterfaceWindows.h )
	SET( SOURCES ${SOURCES}	
		 src/RPMSerialInterfaceWindows.cpp )

ELSEIF ( CMAKE_SYSTEM_NAME MATCHES "Linux" OR 
         CMAKE_SYSTEM_NAME MATCHES "Darwin" )		
	SET( HEADERS ${HEADERS} 
		 include/RPMSerialInterfacePOSIX.h )
	SET( SOURCES ${SOURCES}	
		 src/RPMSerialInterfacePOSIX.cpp )			# Could also be used on Windows with MinGW

ELSE()
	MESSAGE("${PROJECT_NAME} is only available for Windows, Linux and Darwin")
ENDIF()

INCLUDE_DIRECTORIES( include )
SOURCE_GROUP("" FILES ${HEADERS} ${SOURCES} )		# Avoid "Header Files" and "Source Files" virtual folders in VisualStudio

IF( CMAKE_SYSTEM_NAME MATCHES "Windows" )
	SET(CMAKE_DEBUG_POSTFIX "d")
ENDIF()

ADD_LIBRARY( ${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} )

#
# Install
#
INSTALL(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin )
		#INCLUDES DESTINATION include )		# If uncommented, the ${PROJECT_NAME} target contains INCLUDE_DIRECTORIES information. Importing the target automatically adds this directory to the INCLUDE_DIRECTORIES.
SET( TARGET_NAMESPACE Rapa:: )
INSTALL( FILES ${HEADERS} DESTINATION include COMPONENT Devel )		
EXPORT( EXPORT ${PROJECT_NAME}Targets FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake" NAMESPACE ${TARGET_NAMESPACE} )
CONFIGURE_FILE( cmake/${PROJECT_NAME}Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake" @ONLY )
SET( ConfigPackageLocation lib/cmake/${PROJECT_NAME} )
INSTALL(EXPORT ${PROJECT_NAME}Targets
		FILE ${PROJECT_NAME}Targets.cmake
		NAMESPACE ${TARGET_NAMESPACE}
		DESTINATION ${ConfigPackageLocation} )
INSTALL( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake" DESTINATION ${ConfigPackageLocation} COMPONENT Devel )

ADD_SUBDIRECTORY( samples )
