PROJECT_NAME = nodecast
PROJECT_VERSION = 0.0.1

os2 {
    DEFINES += VERSION=\'\"v$${PROJECT_VERSION}\"\'
} else {
    DEFINES += VERSION=\\\"v$${PROJECT_VERSION}\\\"
}

DEFINES += VERSION_MAJOR=0
DEFINES += VERSION_MINOR=0
DEFINES += VERSION_BUGFIX=1