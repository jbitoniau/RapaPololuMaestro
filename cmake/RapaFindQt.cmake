CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

# The code below defines the following variables:
# RAPA_QT_FOUND: whether Qt was found (Qt4 or 5 depending on the user choice)
# RAPA_QT_LINK_LIBRARIES: the imported Qt targets to link against
# RAPA_QT_DEBUG_DLLS and RAPA_QT_RELEASE_DLLS: on Windows platform, the DLLs to copy next to the binary to get a self-contained release

OPTION( RAPA_USE_QT5 "Use Qt5 instead of Qt4" ON)

SET( CMAKE_INCLUDE_CURRENT_DIR ON )
SET( CMAKE_AUTOMOC ON )
SET( RAPA_QT_FOUND OFF )

IF( RAPA_USE_QT5 )
	
	FIND_PACKAGE( Qt5Widgets )
	IF( Qt5Widgets_FOUND )

		SET( RAPA_QT_FOUND ON )
		SET( RAPA_QT_LINK_LIBRARIES Qt5::Widgets ) 
		
		# Configure deployment/install on Windows
		IF( CMAKE_SYSTEM_NAME MATCHES "Windows" )
	
			# This is very hacky. We find the location of QtCore and assume a particular directory structure.
			# We get two directories up and consider it as the root folder for Qt
			# From there, we assume that the DLLs we need are in the bin and platform sub-directories
			GET_TARGET_PROPERTY( RAPA_QT_CORE_DEBUG_DLL Qt5::Core LOCATION_DEBUG )
			SET( RAPA_QT_ROOT_DIR "${RAPA_QT_CORE_DEBUG_DLL}/../.." )
			GET_FILENAME_COMPONENT( RAPA_QT_ROOT_DIR ${RAPA_QT_ROOT_DIR} ABSOLUTE)
			SET( RAPA_QT_DEBUG_DLLS 
				${RAPA_QT_ROOT_DIR}/bin/icudt53.dll 
				${RAPA_QT_ROOT_DIR}/bin/icuin53.dll 
				${RAPA_QT_ROOT_DIR}/bin/icuuc53.dll 
				${RAPA_QT_ROOT_DIR}/bin/Qt5Cored.dll 
				${RAPA_QT_ROOT_DIR}/bin/Qt5Guid.dll
				${RAPA_QT_ROOT_DIR}/bin/Qt5Widgetsd.dll )
			SET( RAPA_QT_DEBUG_PLATFORMS_DLLS
				${RAPA_QT_ROOT_DIR}/plugins/platforms/qwindowsd.dll ) 

			SET( RAPA_QT_RELEASE_DLLS 
				${RAPA_QT_ROOT_DIR}/bin/icudt53.dll 
				${RAPA_QT_ROOT_DIR}/bin/icuin53.dll 
				${RAPA_QT_ROOT_DIR}/bin/icuuc53.dll 
				${RAPA_QT_ROOT_DIR}/bin/Qt5Core.dll 
				${RAPA_QT_ROOT_DIR}/bin/Qt5Gui.dll
				${RAPA_QT_ROOT_DIR}/bin/Qt5Widgets.dll )
			SET( RAPA_QT_RELEASE_PLATFORMS_DLLS
				${RAPA_QT_ROOT_DIR}/plugins/platforms/qwindows.dll ) 
		ENDIF()
	ENDIF()

ELSE()
	
	FIND_PACKAGE( Qt4 COMPONENTS QtCore QtGui )
	IF( QT4_FOUND )
		SET( RAPA_QT_FOUND ON )
		SET( RAPA_QT_LINK_LIBRARIES Qt4::QtCore Qt4::QtGui ) 
		
		# Configure deployment/install on Windows
		IF( CMAKE_SYSTEM_NAME MATCHES "Windows" )
			SET( RAPA_QT_DEBUG_DLLS ${QT_BINARY_DIR}/QtCored4.dll ${QT_BINARY_DIR}/QtGuid4.dll )
			SET( RAPA_QT_RELEASE_DLLS ${QT_BINARY_DIR}/QtCore4.dll ${QT_BINARY_DIR}/QtGui4.dll )
		ENDIF()
	ENDIF()

ENDIF()